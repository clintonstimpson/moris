/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_FEM_SP_Incompressible_Flow.cpp
 *
 */

#include "cl_FEM_SP_Incompressible_Flow.hpp"        //FEM/INT/src
#include "cl_FEM_Cluster.hpp"                       //FEM/INT/src
#include "cl_FEM_Field_Interpolator_Manager.hpp"    //FEM/INT/src

#include "fn_trans.hpp"
#include "fn_norm.hpp"
#include "fn_eye.hpp"
#include "fn_dot.hpp"
#include "fn_inv.hpp"
#include "op_div.hpp"
#include "fn_diag_vec.hpp"

namespace moris
{
    namespace fem
    {

        //--------------------------------------------------------------------------------------------------------------

        SP_Incompressible_Flow::SP_Incompressible_Flow()
        {
            // set the property pointer cell size
            mLeaderProp.resize( static_cast< uint >( Property_Type::MAX_ENUM ), nullptr );

            // populate the map
            mPropertyMap[ "Density" ]         = static_cast< uint >( Property_Type::DENSITY );
            mPropertyMap[ "Viscosity" ]       = static_cast< uint >( Property_Type::VISCOSITY );
            mPropertyMap[ "InvPermeability" ] = static_cast< uint >( Property_Type::INV_PERMEABILITY );
        }

        //--------------------------------------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::set_space_dim( uint aSpaceDim )
        {
            // check that space dimension is 1, 2, 3
            MORIS_ERROR( aSpaceDim > 0 && aSpaceDim < 4,
                    "SP_Incompressible_Flow::set_space_dim - wrong space dimension." );

            // set space dimension
            mSpaceDim = aSpaceDim;

            // set function pointer
            this->set_function_pointers();
        }

        //--------------------------------------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::set_function_pointers()
        {
            // switch on space dimensions
            switch ( mSpaceDim )
            {
                // if 2D
                case 2:
                {
                    mEvalGFunc = this->eval_G_2d;
                    break;
                }

                // if 3D
                case 3:
                {
                    mEvalGFunc = this->eval_G_3d;
                    break;
                }

                // default
                default:
                    MORIS_ERROR( false, "SP_Incompressible_Flow::set_function_pointers - only support 2 and 3D." );
                    break;
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::set_parameters( moris::Cell< Matrix< DDRMat > > aParameters )
        {
            // set mParameters
            mParameters = aParameters;

            // get number of parameters
            uint tParamSize = aParameters.size();

            // check for proper size of constant function parameters
            MORIS_ERROR( tParamSize >= 1 && tParamSize < 3,
                    "SP_Incompressible_Flow::set_parameters - either 1 or 2 constant parameter need to be set." );

            // check for proper parameter type; here just a scalar
            MORIS_ERROR( aParameters( 0 ).numel() == 1,
                    "SP_Incompressible_Flow::set_parameters - 1st parameter is not a scalar but a vector." );

            // check for proper parameter value
            MORIS_ERROR( mParameters( 0 )( 0 ) > 0.0,
                    "SP_Incompressible_Flow::set_parameters - CI parameter needs to be larger than zero." );

            // set CI
            mCI = mParameters( 0 )( 0 );

            // if time
            if ( tParamSize > 1 )
            {
                // check for proper parameter type; here just a scalar
                MORIS_ERROR( aParameters( 1 ).numel() == 1,
                        "SP_Incompressible_Flow::set_parameters - 2nd parameter is not a scalar but a vector." );

                mBetaTime = aParameters( 1 )( 0 );

                // set beta time flag to true
                if ( std::abs( mBetaTime ) > MORIS_REAL_EPS )
                {
                    mSetBetaTime = true;
                }
            }
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::set_dof_type_list(
                moris::Cell< moris::Cell< MSI::Dof_Type > >& aDofTypes,
                moris::Cell< std::string >&                  aDofStrings,
                mtk::Leader_Follower                            aIsLeader )
        {
            // switch on leader follower
            switch ( aIsLeader )
            {
                case mtk::Leader_Follower::LEADER:
                {
                    // set dof type list
                    mLeaderDofTypes = aDofTypes;

                    // loop on dof type
                    for ( uint iDof = 0; iDof < aDofTypes.size(); iDof++ )
                    {
                        // get dof string
                        std::string tDofString = aDofStrings( iDof );

                        // get dof type
                        MSI::Dof_Type tDofType = aDofTypes( iDof )( 0 );

                        // if velocity
                        if ( tDofString == "Velocity" )
                        {
                            mLeaderDofVelocity = tDofType;
                        }
                        else if ( tDofString == "Pressure" )
                        {
                            mLeaderDofPressure = tDofType;
                        }
                        else
                        {
                            // error unknown dof string
                            MORIS_ERROR( false,
                                    "SP_Incompressible_Flow::set_dof_type_list - Unknown aDofString : %s \n",
                                    tDofString.c_str() );
                        }
                    }
                    break;
                }

                case mtk::Leader_Follower::FOLLOWER:
                {
                    // set dof type list
                    mFollowerDofTypes = aDofTypes;
                    break;
                }

                default:
                    MORIS_ERROR( false, "SP_Incompressible_Flow::set_dof_type_list - unknown leader follower type." );
                    break;
            }
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::eval_SP()
        {
            // set size for SP values
            mPPVal.set_size( 2, 1 );

            // get the velocity and pressure FIs
            Field_Interpolator* tVelocityFI =
                    mLeaderFIManager->get_field_interpolators_for_type( mLeaderDofVelocity );

            // get the density and viscosity properties
            const std::shared_ptr< Property >& tDensityProp =
                    mLeaderProp( static_cast< uint >( Property_Type::DENSITY ) );

            const std::shared_ptr< Property >& tViscosityProp =
                    mLeaderProp( static_cast< uint >( Property_Type::VISCOSITY ) );

            const std::shared_ptr< Property >& tInvPermeabProp =
                    mLeaderProp( static_cast< uint >( Property_Type::INV_PERMEABILITY ) );

            // get the density and viscosity value
            real tDensity   = tDensityProp->val()( 0 );
            real tViscosity = tViscosityProp->val()( 0 );

            // get impermeability
            real tInvPermeab = 0.0;

            if ( tInvPermeabProp != nullptr )
            {
                tInvPermeab = tInvPermeabProp->val()( 0 );
            }

            // evaluate Gij = sum_k dxi_k/dx_i dxi_k/dx_j
            Matrix< DDRMat > tG;
            this->eval_G( tG );

            // get flattened G to row vector
            Matrix< DDRMat > tFlatG = trans( vectorize( tG ) );

            // get trace of G
            real tTrG = sum( diag_vec( tG ) );

            // evaluate tauM = mPPVal( 0 )
            Matrix< DDRMat > tvivjGij = trans( tVelocityFI->val() ) * tG * tVelocityFI->val();
            Matrix< DDRMat > tGijGij  = tFlatG * trans( tFlatG );

            real tPPVal =
                    std::pow( tDensity, 2.0 ) * tvivjGij( 0 )             //
                    + mCI * std::pow( tViscosity, 2.0 ) * tGijGij( 0 )    //
                    + std::pow( tInvPermeab, 2.0 );

            // if time solve
            if ( mSetBetaTime )
            {
                // get the time step
                real tDeltaT = mBetaTime * mLeaderFIManager->get_IP_geometry_interpolator()->get_time_step();

                // add time contribution
                tPPVal += std::pow( 2.0 * tDensity / tDeltaT, 2.0 );
            }

            // threshold tPPVal
            tPPVal = std::max( tPPVal, mEpsilon );

            // evaluate tauM
            real tTauM = std::pow( tPPVal, -0.5 );

            // threshold tauM
            mPPVal( 0 ) = std::max( tTauM, mEpsilon );

            // evaluate tauC = mPPVal( 1 )
            mPPVal( 1 ) = 1.0 / ( mPPVal( 0 ) * tTrG );
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::eval_dSPdLeaderDOF(
                const moris::Cell< MSI::Dof_Type >& aDofTypes )
        {
            // get the dof type index
            uint tDofIndex = mLeaderGlobalDofTypeMap( static_cast< uint >( aDofTypes( 0 ) ) );

            // get the dof type FI
            Field_Interpolator* tFI = mLeaderFIManager->get_field_interpolators_for_type( aDofTypes( 0 ) );

            // set matrix size and initialize
            mdPPdLeaderDof( tDofIndex ).set_size( 2, tFI->get_number_of_space_time_coefficients() );

            // get the velocity FI
            Field_Interpolator* tVelocityFI =
                    mLeaderFIManager->get_field_interpolators_for_type( mLeaderDofVelocity );

            // get the density and viscosity properties
            const std::shared_ptr< Property >& tDensityProp =
                    mLeaderProp( static_cast< uint >( Property_Type::DENSITY ) );

            const std::shared_ptr< Property >& tViscosityProp =
                    mLeaderProp( static_cast< uint >( Property_Type::VISCOSITY ) );

            const std::shared_ptr< Property >& tInvPermeabProp =
                    mLeaderProp( static_cast< uint >( Property_Type::INV_PERMEABILITY ) );

            // get the density and viscosity value
            real tDensity   = tDensityProp->val()( 0 );
            real tViscosity = tViscosityProp->val()( 0 );

            // get impermeability
            real tInvPermeab = 0.0;

            if ( tInvPermeabProp != nullptr )
            {
                tInvPermeab = tInvPermeabProp->val()( 0 );
            }

            // evaluate Gij = sum_d dxi_d/dx_i dxi_d/dx_j
            Matrix< DDRMat > tG;
            this->eval_G( tG );

            // get flattened G to row vector
            Matrix< DDRMat > tFlatG = trans( vectorize( tG ) );

            // get trace of G
            real tTrG = sum( diag_vec( tG ) );

            // evaluate
            Matrix< DDRMat > tvivjGij = trans( tVelocityFI->val() ) * tG * tVelocityFI->val();
            Matrix< DDRMat > tGijGij  = tFlatG * trans( tFlatG );

            real tPPVal =
                    std::pow( tDensity, 2.0 ) * tvivjGij( 0 )             //
                    + mCI * std::pow( tViscosity, 2.0 ) * tGijGij( 0 )    //
                    + std::pow( tInvPermeab, 2.0 );

            // if time solve
            if ( mSetBetaTime )
            {
                // get the time step
                real tDeltaT = mBetaTime * mLeaderFIManager->get_IP_geometry_interpolator()->get_time_step();

                // add time contribution
                tPPVal += std::pow( 2.0 * tDensity / tDeltaT, 2.0 );
            }

            // compute derivatives if tPPVal is not thresholded
            if ( tPPVal > mEpsilon )
            {
                // common factor for evaluations below
                real tPreFactor = -0.5 * std::pow( tPPVal, -1.5 );

                // if velocity
                if ( aDofTypes( 0 ) == mLeaderDofVelocity )
                {
                    mdPPdLeaderDof( tDofIndex ).get_row( 0 ) =
                            tPreFactor * std::pow( tDensity, 2.0 ) * ( 2.0 * trans( tFI->val() ) * tG * tFI->N() );
                }
                else
                {
                    mdPPdLeaderDof( tDofIndex ).fill( 0.0 );
                }

                // if density
                if ( tDensityProp->check_dof_dependency( aDofTypes ) )
                {
                    mdPPdLeaderDof( tDofIndex ).get_row( 0 ) +=
                            tPreFactor * ( 2.0 * tDensity * tvivjGij( 0 ) ) * tDensityProp->dPropdDOF( aDofTypes );

                    // if time solve
                    if ( mSetBetaTime )
                    {
                        // get the time step
                        real tDeltaT = mBetaTime * mLeaderFIManager->get_IP_geometry_interpolator()->get_time_step();

                        // add time contribution
                        mdPPdLeaderDof( tDofIndex ).get_row( 0 ) +=
                                tPreFactor * ( 8.0 * tDensity / tDeltaT / tDeltaT ) * tDensityProp->dPropdDOF( aDofTypes );
                    }
                }

                // if viscosity
                if ( tViscosityProp->check_dof_dependency( aDofTypes ) )
                {
                    mdPPdLeaderDof( tDofIndex ).get_row( 0 ) +=
                            tPreFactor * ( 2.0 * mCI * tViscosity * tGijGij * tViscosityProp->dPropdDOF( aDofTypes ) );
                }

                // if permeability
                if ( tInvPermeabProp != nullptr )
                {
                    if ( tInvPermeabProp->check_dof_dependency( aDofTypes ) )
                    {
                        mdPPdLeaderDof( tDofIndex ).get_row( 0 ) +=
                                tPreFactor * ( 2.0 * tInvPermeabProp->val()( 0 ) * tInvPermeabProp->dPropdDOF( aDofTypes ) );
                    }
                }

                // evaluate tauM
                real tTauM = std::pow( tPPVal, -0.5 );

                // dtauCdDOF
                if ( tTauM > mEpsilon )
                {
                    mdPPdLeaderDof( tDofIndex ).get_row( 1 ) =
                            -mdPPdLeaderDof( tDofIndex ).get_row( 0 ) / ( tTrG * std::pow( tTauM, 2.0 ) );
                }
            }
            else
            {
                mdPPdLeaderDof( tDofIndex ).fill( 0.0 );
            }
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::eval_G( Matrix< DDRMat >& aG )
        {
            // get the space jacobian from IP geometry interpolator
            const Matrix< DDRMat >& tInvSpaceJacobian =
                    mLeaderFIManager->get_IP_geometry_interpolator()->inverse_space_jacobian();

            // evaluate Gij = sum_d dxi_d/dx_i dxi_d/dx_j
            this->mEvalGFunc( aG, tInvSpaceJacobian );
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::eval_G_2d(
                Matrix< DDRMat >&       aG,
                const Matrix< DDRMat >& aInvSpaceJacobian )
        {
            // set size for aG
            aG.set_size( 2, 2 );

            // fill aGij = sum_d dxi_d/dx_i dxi_d/dx_j
            aG( 0, 0 ) = std::pow( aInvSpaceJacobian( 0, 0 ), 2.0 ) + std::pow( aInvSpaceJacobian( 0, 1 ), 2.0 );
            aG( 0, 1 ) = aInvSpaceJacobian( 0, 0 ) * aInvSpaceJacobian( 1, 0 ) + aInvSpaceJacobian( 0, 1 ) * aInvSpaceJacobian( 1, 1 );
            aG( 1, 0 ) = aG( 0, 1 );
            aG( 1, 1 ) = std::pow( aInvSpaceJacobian( 1, 0 ), 2.0 ) + std::pow( aInvSpaceJacobian( 1, 1 ), 2.0 );
        }

        //------------------------------------------------------------------------------

        void
        SP_Incompressible_Flow::eval_G_3d(
                Matrix< DDRMat >&       aG,
                const Matrix< DDRMat >& aInvSpaceJacobian )
        {
            // set size for aG
            aG.set_size( 3, 3 );

            // fill aGij = sum_d dxi_d/dx_i dxi_d/dx_j
            aG( 0, 0 ) = std::pow( aInvSpaceJacobian( 0, 0 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 0, 1 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 0, 2 ), 2.0 );
            aG( 0, 1 ) = aInvSpaceJacobian( 0, 0 ) * aInvSpaceJacobian( 1, 0 )
                       + aInvSpaceJacobian( 0, 1 ) * aInvSpaceJacobian( 1, 1 )
                       + aInvSpaceJacobian( 0, 2 ) * aInvSpaceJacobian( 1, 2 );
            aG( 0, 2 ) = aInvSpaceJacobian( 0, 0 ) * aInvSpaceJacobian( 2, 0 )
                       + aInvSpaceJacobian( 0, 1 ) * aInvSpaceJacobian( 2, 1 )
                       + aInvSpaceJacobian( 0, 2 ) * aInvSpaceJacobian( 2, 2 );
            aG( 1, 0 ) = aG( 0, 1 );
            aG( 1, 1 ) = std::pow( aInvSpaceJacobian( 1, 0 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 1, 1 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 1, 2 ), 2.0 );
            aG( 1, 2 ) = aInvSpaceJacobian( 1, 0 ) * aInvSpaceJacobian( 2, 0 )
                       + aInvSpaceJacobian( 1, 1 ) * aInvSpaceJacobian( 2, 1 )
                       + aInvSpaceJacobian( 1, 2 ) * aInvSpaceJacobian( 2, 2 );
            aG( 2, 0 ) = aG( 0, 2 );
            aG( 2, 1 ) = aG( 1, 2 );
            aG( 2, 2 ) = std::pow( aInvSpaceJacobian( 2, 0 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 2, 1 ), 2.0 )
                       + std::pow( aInvSpaceJacobian( 2, 2 ), 2.0 );
        }

        //------------------------------------------------------------------------------
    } /* namespace fem */
} /* namespace moris */

