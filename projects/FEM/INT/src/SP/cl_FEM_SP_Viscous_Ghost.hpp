/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_FEM_SP_Viscous_Ghost.hpp
 *
 */

#ifndef SRC_FEM_CL_FEM_SP_VISCOUS_GHOST_HPP_
#define SRC_FEM_CL_FEM_SP_VISCOUS_GHOST_HPP_

#include <map>
// MRS/CNT/src
#include "typedefs.hpp"
#include "cl_Cell.hpp"
// LINALG/src
#include "cl_Matrix.hpp"
#include "linalg_typedefs.hpp"
// FEM/INT/src
#include "cl_FEM_Field_Interpolator.hpp"
#include "cl_FEM_Constitutive_Model.hpp"
#include "cl_FEM_Stabilization_Parameter.hpp"
#include "cl_FEM_Cluster.hpp"

namespace moris
{
    namespace fem
    {
        //------------------------------------------------------------------------------
        /*
         * Stabilization parameter for face-oriented viscous ghost-penalty
         * gamma_mu = alpha_mu * viscosity * h^(2*(i-1)+1)
         * where i = interpolation order
         * from Schott et al. (2015)
         *
         * and contribution from inverse permeability
         *
         * gamma_mu = gamma_mu + alpha_mu * impermeability * h^(2*(i-1)+3)
         *
         */
        class SP_Viscous_Ghost : public Stabilization_Parameter
        {

            //------------------------------------------------------------------------------

          private:
            // default tuple for element size to define cluster measure
            std::tuple< fem::Measure_Type, mtk::Primary_Void, mtk::Leader_Follower > mElementSizeTuple =
                    std::make_tuple(
                            fem::Measure_Type::CELL_LENGTH_MEASURE,
                            mtk::Primary_Void::PRIMARY,
                            mtk::Leader_Follower::LEADER );

            // property type for the SP
            enum class Property_Type
            {
                VISCOSITY,    // fluid viscosity
                INV_PERMEABILITY,
                MAX_ENUM
            };

          public:
            /*
             * Rem: mParameters( 0 ) - alpha_mu
             */

            //------------------------------------------------------------------------------
            /*
             * constructor
             */
            SP_Viscous_Ghost();

            //------------------------------------------------------------------------------
            /**
             * trivial destructor
             */
            ~SP_Viscous_Ghost(){};

            //------------------------------------------------------------------------------
            /**
             * set dof types
             * @param[ in ] aDofTypes a cell of cell of dof types
             * @param[ in ] aDofStrings list of strings describing the dof types
             * @param[ in ] aIsLeader enum for leader or follower
             */
            void
            set_dof_type_list(
                    moris::Cell< moris::Cell< MSI::Dof_Type > >& aDofTypes,
                    moris::Cell< std::string >&                  aDofStrings,
                    mtk::Leader_Follower                            aIsLeader = mtk::Leader_Follower::LEADER )
            {
                Stabilization_Parameter::set_dof_type_list( aDofTypes, aIsLeader );
            }

            //------------------------------------------------------------------------------
            /**
             * set dv types
             * @param[ in ] aDvTypes   a cell of group of dv types
             * @param[ in ] aDvStrings list of strings describing the dv types
             * @param[ in ] aIsLeader enum for leader or follower
             */
            void
            set_dv_type_list(
                    moris::Cell< moris::Cell< PDV_Type > >& aDvTypes,
                    moris::Cell< std::string >&             aDvStrings,
                    mtk::Leader_Follower                       aIsLeader = mtk::Leader_Follower::LEADER )
            {
                Stabilization_Parameter::set_dv_type_list( aDvTypes, aIsLeader );
            }

            //------------------------------------------------------------------------------
            /**
             * get cluster measure tuples
             * @param[ in ] aClusterMeasureTuples list of tuples describing the cluster measure types
             */
            moris::Cell< std::tuple<
                    fem::Measure_Type,
                    mtk::Primary_Void,
                    mtk::Leader_Follower > >
            get_cluster_measure_tuple_list();

            //------------------------------------------------------------------------------
            /**
             * evaluate the stabilization parameter value
             */
            void eval_SP();

            //------------------------------------------------------------------------------
            /**
             * evaluate the stabilization parameter derivative wrt to a leader dof type
             * @param[ in ] aDofTypes a dof type wrt which the derivative is evaluated
             */
            void eval_dSPdLeaderDOF( const moris::Cell< MSI::Dof_Type >& aDofTypes );

            //------------------------------------------------------------------------------
            /**
             * evaluate the penalty parameter derivative wrt to a leader dv type
             * @param[ in ] aDvTypes a dv type wrt which the derivative is evaluated
             */
            void
            eval_dSPdLeaderDV( const moris::Cell< PDV_Type >& aDvTypes )
            {
                MORIS_ERROR( false, "SP_Viscous_Ghost::eval_dSPdLeaderDV - not implemented." );
            }

            //------------------------------------------------------------------------------
        };
        //------------------------------------------------------------------------------
    } /* namespace fem */
} /* namespace moris */

#endif /* SRC_FEM_CL_FEM_SP_VISCOUS_GHOST_HPP_ */
