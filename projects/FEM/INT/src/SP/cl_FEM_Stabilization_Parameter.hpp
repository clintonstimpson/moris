/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_FEM_Stabilization_Parameter.hpp
 *
 */

#ifndef SRC_FEM_CL_FEM_STABILIZATION_PARAMETER_HPP_
#define SRC_FEM_CL_FEM_STABILIZATION_PARAMETER_HPP_

//MRS/COR/src
#include "typedefs.hpp"
//#include "linalg_typedefs.hpp"
#include "cl_Cell.hpp"
//LNA/src
#include "cl_Matrix.hpp"
//FEM/INT/src
#include "cl_FEM_Field_Interpolator.hpp"
#include "cl_FEM_Property.hpp"
#include "cl_FEM_Constitutive_Model.hpp"
#include "cl_FEM_Enums.hpp"
#include "fn_FEM_FD_Scheme.hpp"
//FEM/MSI/src
#include "cl_MSI_Dof_Type_Enums.hpp"
//GEN/src
#include "cl_GEN_Pdv_Enums.hpp"
//MTK/src
#include "cl_MTK_Enums.hpp"

namespace moris
{
    namespace fem
    {
        class Cluster;
        class Set;
        class Field_Interpolator_Manager;
        class Cluster_Measure;

        //------------------------------------------------------------------------------
        /**
         * Stabilization_Parameter
         */
        class Stabilization_Parameter
        {
                //------------------------------------------------------------------------------
            protected :

                //------------------------------------------------------------------------------

                // fem set pointer
                fem::Set * mSet = nullptr;

                // field interpolator manager pointer
                Field_Interpolator_Manager * mLeaderFIManager = nullptr;
                Field_Interpolator_Manager * mFollowerFIManager  = nullptr;

                // cluster pointer
                fem::Cluster * mCluster = nullptr;

                // list of parameters
                moris::Cell< Matrix< DDRMat > > mParameters;

                // interpolation order
                uint mOrder = MORIS_UINT_MAX;

                // normal
                Matrix< DDRMat > mNormal;

                // leader and follower dof type lists
                moris::Cell< moris::Cell< MSI::Dof_Type > > mLeaderDofTypes;
                moris::Cell< moris::Cell< MSI::Dof_Type > > mFollowerDofTypes;

                // leader and follower global dof type list
                moris::Cell< moris::Cell< MSI::Dof_Type > > mLeaderGlobalDofTypes;
                moris::Cell< moris::Cell< MSI::Dof_Type > > mFollowerGlobalDofTypes;

                // leader and follower global dof type maps
                Matrix< DDSMat > mLeaderGlobalDofTypeMap;
                Matrix< DDSMat > mFollowerGlobalDofTypeMap;

                // leader and follower dv type lists
                moris::Cell< moris::Cell< PDV_Type > > mLeaderDvTypes;
                moris::Cell< moris::Cell< PDV_Type > > mFollowerDvTypes;

                // leader and follower global dv type list
                moris::Cell< moris::Cell< PDV_Type > > mLeaderGlobalDvTypes;
                moris::Cell< moris::Cell< PDV_Type > > mFollowerGlobalDvTypes;

                // leader and follower global dv type maps
                Matrix< DDSMat > mLeaderGlobalDvTypeMap;
                Matrix< DDSMat > mFollowerGlobalDvTypeMap;

                // leader and follower properties
                moris::Cell< std::shared_ptr< Property > > mLeaderProp;
                moris::Cell< std::shared_ptr< Property > > mFollowerProp;

                // local string to int map for properties
                std::map< std::string, uint > mPropertyMap;

                // leader and follower constitutive models
                moris::Cell< std::shared_ptr< Constitutive_Model > > mLeaderCM;
                moris::Cell< std::shared_ptr< Constitutive_Model > > mFollowerCM;

                // local string to int map for CMs
                std::map< std::string, uint > mConstitutiveMap;

                // storage
                Matrix< DDRMat > mPPVal;
                moris::Cell< Matrix< DDRMat > > mdPPdLeaderDof;
                moris::Cell< Matrix< DDRMat > > mdPPdFollowerDof;
                moris::Cell< Matrix< DDRMat > > mdPPdLeaderDv;
                moris::Cell< Matrix< DDRMat > > mdPPdFollowerDv;

                // spatial dimensions
                uint mSpaceDim;

                // string for stabilization parameter name
                std::string mName;

                // bool for leader and follower
                bool mHasFollower = false;

            private:

                // bool for global dof type list and map
                bool mGlobalDofBuild = true;
                bool mGlobalDvBuild = true;

                // flag for evaluation
                bool mPPEval = true;
                moris::Matrix< DDBMat > mdPPdLeaderDofEval;
                moris::Matrix< DDBMat > mdPPdFollowerDofEval;
                moris::Matrix< DDBMat > mdPPdLeaderDvEval;
                moris::Matrix< DDBMat > mdPPdFollowerDvEval;

                //------------------------------------------------------------------------------
            public :

                //------------------------------------------------------------------------------
                /**
                 * constructor
                 */
                Stabilization_Parameter(){};

                //------------------------------------------------------------------------------
                /**
                 * virtual destructor
                 */
                virtual ~Stabilization_Parameter(){};

                //------------------------------------------------------------------------------
                /**
                 * set name
                 * param[ in ] aName a string for CM name
                 */
                void set_name( std::string aName )
                {
                    mName = aName;
                }

                //------------------------------------------------------------------------------
                /**
                 * get name
                 * param[ out ] mName a string for CM name
                 */
                std::string get_name()
                {
                    return mName;
                }

                //------------------------------------------------------------------------------
                /**
                 * get name
                 * param[ out ] mHasFollower bool true if CM has a follower
                 */
                bool get_has_follower()
                {
                    return mHasFollower;
                }

                //------------------------------------------------------------------------------
                /**
                 * print names
                 */
                void print_names();

                //------------------------------------------------------------------------------
                /**
                 * set space dimension
                 * @param[ in ] aSpaceDim a spatial dimension
                 */
                virtual void set_space_dim( uint aSpaceDim )
                {
                    // check that space dimension is 1, 2, 3
                    MORIS_ERROR( aSpaceDim > 0 && aSpaceDim < 4,
                            "Stabilization_Parameter::set_space_dim - wrong space dimension." );

                    // set space dimension
                    mSpaceDim = aSpaceDim;
                }

                //------------------------------------------------------------------------------
                /*
                 * set field interpolator manager pointer
                 * @param[ in ] aFieldInteprolatorManager a field interpolator manager pointer
                 * @param[ in ] aIsLeader enum for leader or follower
                 */
                void set_field_interpolator_manager(
                        Field_Interpolator_Manager * aFieldInterpolatorManager,
                        mtk::Leader_Follower            aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /*
                 * get field interpolator manager pointer
                 * @param[ out ] aFieldInteprolatorManager a field interpolator manager pointer
                 * @param[ in ] aIsLeader enum for leader or follower
                 */
                Field_Interpolator_Manager * get_field_interpolator_manager(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /*
                 * set member set pointer
                 * @param[ in ] aSetPointer a fem set pointer
                 */
                void set_set_pointer( Set * aSetPointer )
                {
                    mSet = aSetPointer;
                }

                //------------------------------------------------------------------------------
                /**
                 * set parameters
                 * @param[ in ] aParameters a list of parameters
                 */
                virtual void set_parameters( moris::Cell< Matrix< DDRMat > > aParameters );

                //------------------------------------------------------------------------------
                /**
                 * set interpolation order
                 * @param[ in ] aOrder an interpolation order
                 */
                void set_interpolation_order( uint aOrder )
                {
                    // set an interpolation order
                    mOrder = aOrder;

                    // reset evaluation flags
                    this->reset_eval_flags();
                }

                //------------------------------------------------------------------------------
                /**
                 * set normal
                 * @param[ in ] aNormal a normal
                 */
                void set_normal( Matrix< DDRMat > aNormal )
                {
                    // set normal
                    mNormal = aNormal;
                }

                //------------------------------------------------------------------------------
                /**
                 * set cluster
                 * @param[ in ] aCluster a fem cluster pointer
                 */
                void set_cluster( fem::Cluster * aCluster );

                //------------------------------------------------------------------------------
                /**
                 * set cluster measure types
                 * @param[ in ] aClusterMeasureTuples list of tuples describing the cluster measure types
                 * @param[ in ] aClusterMeasureTypes  list of strings describing the cluster measure types
                 */
                virtual void set_cluster_measure_type_list(
                        moris::Cell< std::tuple<
                        fem::Measure_Type,
                        mtk::Primary_Void,
                        mtk::Leader_Follower > >      & aClusterMeasureTuples,
                        moris::Cell< std::string > & aClusterMeasureNames )
                {}

                //------------------------------------------------------------------------------
                /**
                 * get cluster measure tuples
                 * @param[ in ] aClusterMeasureTuples list of tuples describing the cluster measure types
                 */
                virtual moris::Cell< std::tuple<
                fem::Measure_Type,
                mtk::Primary_Void,
                mtk::Leader_Follower > > get_cluster_measure_tuple_list()
                {
                    return moris::Cell< std::tuple<
                            fem::Measure_Type,
                            mtk::Primary_Void,
                            mtk::Leader_Follower > >( 0 );
                }

                //------------------------------------------------------------------------------
                /**
                 * reset evaluation flags
                 */
                virtual void reset_eval_flags();

                //------------------------------------------------------------------------------
                /**
                 * set dof types
                 * @param[ in ] aDofTypes a list of group of dof types
                 * @param[ in ] aIsLeader enum for leader or follower
                 */
                void set_dof_type_list(
                        const moris::Cell< moris::Cell< MSI::Dof_Type > > & aDofTypes,
                        mtk::Leader_Follower                                   aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * set dof types
                 * @param[ in ] aDofTypes a cell of cell of dof types
                 * @param[ in ] aDofStrings list of strings describing the dof types
                 * @param[ in ] aIsLeader enum for leader or follower
                 */
                virtual void set_dof_type_list(
                        moris::Cell< moris::Cell< MSI::Dof_Type > > & aDofTypes,
                        moris::Cell< std::string >                  & aDofStrings,
                        mtk::Leader_Follower                             aIsLeader = mtk::Leader_Follower::LEADER )
                {
                    MORIS_ERROR( false, "Stabilization_Parameter::set_dof_type_list - not implemented for base class." );
                }

                //------------------------------------------------------------------------------
                /**
                 * return a cell of dof types
                 * @param[ in ]  aIsLeader enum leader or follower
                 * @param[ out ] aDofTypes a list of group of dof types
                 */
                const moris::Cell< moris::Cell< MSI::Dof_Type > > & get_dof_type_list(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER ) const;

                //------------------------------------------------------------------------------
                /**
                 * set dv types
                 * @param[ in ] aDvTypes a list of group of dv types
                 * @param[ in ] aIsLeader enum for leader or follower
                 */
                void set_dv_type_list(
                        moris::Cell< moris::Cell< PDV_Type > > & aDvTypes,
                        mtk::Leader_Follower                        aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * set dv types
                 * @param[ in ] aDvTypes   a list of group of dv types
                 * @param[ in ] aDvStrings list of strings describing the dv types
                 * @param[ in ] aIsLeader  enum for leader or follower
                 */
                virtual void set_dv_type_list(
                        moris::Cell< moris::Cell< PDV_Type > > & aDvTypes,
                        moris::Cell< std::string >             & aDvStrings,
                        mtk::Leader_Follower                        aIsLeader = mtk::Leader_Follower::LEADER )
                {
                    MORIS_ERROR( false, "Stabilization_Parameter::set_dv_type_list - not implemented for base class." );
                }

                //------------------------------------------------------------------------------
                /**
                 * return a cell of dv types
                 * @param[ in ]  aIsLeader enum leader or follower
                 * @param[ out ] aDvTypes a list of group of dv types
                 */
                const moris::Cell< moris::Cell< PDV_Type > > & get_dv_type_list(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER ) const;

                //------------------------------------------------------------------------------
                /**
                 * get global dof type list
                 * @param[ in ]  aIsLeader enum leader or follower
                 * @param[ out ] mGlobalDofTypes global list of dof type
                 */
                const moris::Cell< moris::Cell< MSI::Dof_Type > > & get_global_dof_type_list(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * get a non unique list of dof type including
                 * property, constitutive and stabilization dependencies
                 * for both leader and follower
                 */
                void get_non_unique_dof_types( moris::Cell< MSI::Dof_Type > & aDofTypes );
                void get_non_unique_dof_and_dv_types(
                        moris::Cell< MSI::Dof_Type > & aDofTypes,
                        moris::Cell< PDV_Type >      & aDvTypes );

                //------------------------------------------------------------------------------
                /**
                 * create a global dof type list including constitutive and property dependencies
                 */
                virtual void build_global_dof_type_list();

                //------------------------------------------------------------------------------
                /**
                 * build global dof type map
                 */
                void build_global_dof_type_map();

                //------------------------------------------------------------------------------
                /**
                 * get global dof type map
                 * @param[ in ]  aIsLeader         enum leader or follower
                 * @param[ out ] mGlobalDofTypeMap a global dof type map
                 */
                const Matrix< DDSMat > & get_global_dof_type_map(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * check dependency on a given group of dof types
                 * @param[ in ]  aDofType       a group of dof types
                 * @param[ in ]  aIsLeader      enum leader or follower
                 * @param[ out ] tDofDependency a bool true if dependency on dof type
                 */
                bool check_dof_dependency(
                        const moris::Cell< MSI::Dof_Type > & aDofType,
                        mtk::Leader_Follower                    aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * set leader or follower constitutive model
                 * @param[ in ] aConstitutiveModel  CM shared pointer to set
                 * @param[ in ] aConstitutiveString string describing the CM to set
                 * @param[ in ] aIsLeader           enum leader or follower
                 */
                void set_constitutive_model(
                        std::shared_ptr< Constitutive_Model > aConstitutiveModel,
                        std::string                           aConstitutiveString,
                        mtk::Leader_Follower                     aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * get leader or follower constitutive models
                 * @param[ in ]  aIsLeader enum leader or follower
                 * @param[ out ] mProp     a list of CM pointers
                 */
                moris::Cell< std::shared_ptr< Constitutive_Model > > & get_constitutive_models(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * set leader or follower property
                 * @param[ in ] aProperty       property shared pointer
                 * @param[ in ] aPropertyString string describing the property to set
                 * @param[ in ] aIsLeader       enum leader or follower
                 */
                void set_property(
                        std::shared_ptr< Property > aProperty,
                        std::string                 aPropertyString,
                        mtk::Leader_Follower           aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * get leader or follower properties
                 * @param[ in ]  aIsLeader enum leader or follower
                 * @param[ out ] mProp     a list of property pointers
                 */
                moris::Cell< std::shared_ptr< Property > > & get_properties(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * get global dv type list
                 * @param[ out ] mGlobalDvTypes global list of dv type
                 */
                const moris::Cell< moris::Cell< PDV_Type > > & get_global_dv_type_list(
                        mtk::Leader_Follower aIsLeader = mtk::Leader_Follower::LEADER );

                //------------------------------------------------------------------------------
                /**
                 * create a global dv type list including constitutive and property dependencies
                 */
                void build_global_dv_type_list();

                //------------------------------------------------------------------------------
                /**
                 * build global dv type map
                 */
                void build_global_dv_type_map();

                //------------------------------------------------------------------------------
                /**
                 * check dependency on a given group of leader dv types
                 * @param[ in ]  aDvType       a group of dv types
                 * @param[ out ] tDvDependency a bool true if dependency on dv type
                 */
                bool check_leader_dv_dependency( const moris::Cell< PDV_Type > & aDvType );

                //------------------------------------------------------------------------------
                /**
                 * check dependency on a given group of follower dv types
                 * @param[ in ]  aDvType       a group of dv types
                 * @param[ out ] tDvDependency a bool true if dependency on dv type
                 *
                 */
                bool check_follower_dv_dependency( const moris::Cell< PDV_Type > & aDvType );

                //------------------------------------------------------------------------------
                /**
                 * get the stabilization parameter value
                 * @param[ out ] mSPVal stabilization parameter value
                 */
                const Matrix< DDRMat > & val();

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter value
                 */
                virtual void eval_SP()
                {
                    MORIS_ERROR( false, " Stabilization_Parameter::eval_SP - Not implemented for base class. " );
                }

                //------------------------------------------------------------------------------
                /**
                 * get the penalty parameter derivative wrt leader dof
                 * @param[ in ]  aDofTypes      a dof type wrt which the derivative is evaluated
                 * @param[ out ] mdPPdLeaderDof penalty parameter derivative wrt leader dof
                 */
                const Matrix< DDRMat > & dSPdLeaderDOF(
                        const moris::Cell< MSI::Dof_Type > & aDofType );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt leader dof
                 * @param[ in ] aDofTypes dof type wrt which the derivative is evaluated
                 */
                virtual void eval_dSPdLeaderDOF(
                        const moris::Cell< MSI::Dof_Type > & aDofTypes )
                {
                    MORIS_ERROR( false, " Stabilization_Parameter::eval_dSPdLeaderDOF - Not implemented for base class. " );
                }

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt leader dof
                 * by finite difference
                 * @param[ in ] aDofTypes     dof type wrt which the derivative is evaluated
                 * @param[ in ] adSPdDOF_FD   matrix to fill with SP derivative wrt dof
                 * @param[ in ] aPerturbation relative perturbation for finite difference
                 * @param[ in ] aFDSchemeType enum for FD scheme
                 */
                void eval_dSPdLeaderDOF_FD(
                        const moris::Cell< MSI::Dof_Type > & aDofTypes,
                        Matrix< DDRMat >                   & adSPdDOF_FD,
                        real                                 aPerturbation,
                        fem::FDScheme_Type                   aFDSchemeType = fem::FDScheme_Type::POINT_5 );

                //------------------------------------------------------------------------------
                /**
                 * get the stabilization parameter derivative wrt follower dof
                 * @param[ in ]  aDofTypes     dof type wrt which the derivative is evaluated
                 * @param[ out ] mdSPdFollowerDof stabilization parameter derivative wrt follower dof
                 */
                const Matrix< DDRMat > & dSPdFollowerDOF(
                        const moris::Cell< MSI::Dof_Type > & aDofType );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt follower dof
                 * @param[ in ] aDofTypes dof type wrt which the derivative is evaluated
                 */
                virtual void eval_dSPdFollowerDOF(
                        const moris::Cell< MSI::Dof_Type > & aDofTypes )
                {
                    MORIS_ERROR( false, " Stabilization_Parameter::eval_dSPdFollowerDOF - Not implemented for base class. " );
                }

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt leader/follower dof
                 * by finite difference
                 * @param[ in ] aDofTypes     dof type wrt which the derivative is evaluated
                 * @param[ in ] adSPdDOF_FD   matrix to fill with SP derivative wrt dof
                 * @param[ in ] aPerturbation relative perturbation for finite difference
                 * @param[ in ] aFDSchemeType enum for FD scheme
                 */
                void eval_dSPdFollowerDOF_FD(
                        const moris::Cell< MSI::Dof_Type > & aDofTypes,
                        Matrix< DDRMat >                   & adSPdDOF_FD,
                        real                                 aPerturbation,
                        fem::FDScheme_Type                   aFDSchemeType = fem::FDScheme_Type::POINT_5 );

                //------------------------------------------------------------------------------
                /**
                 * get the stabilization parameter derivative wrt leader dv
                 * @param[ in ]  aDvTypes      dv type wrt which the derivative is evaluated
                 * @param[ out ] mdSPdLeaderDv stabilization parameter derivative wrt leader dv
                 */
                const Matrix< DDRMat > & dSPdLeaderDV( const moris::Cell< PDV_Type > & aDvTypes );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt leader dv
                 * @param[ in ] aDvTypes dv type wrt which the derivative is evaluated
                 */
                virtual void eval_dSPdLeaderDV( const moris::Cell< PDV_Type > & aDvTypes )
                {
                    MORIS_ERROR( false, " Stabilization_Parameter::eval_dSPdLeaderDV - Not implemented for base class. " );
                }

                //------------------------------------------------------------------------------
                /**
                 * get the stabilization parameter derivative wrt follower dv
                 * @param[ in ]  aDvTypes     a dv type wrt which the derivative is evaluated
                 * @param[ out ] mdSPdFollowerDv stabilization parameter derivative wrt leader dv
                 */
                const Matrix< DDRMat > & dSPdFollowerDV( const moris::Cell< PDV_Type > & aDvTypes );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the stabilization parameter derivative wrt follower dv
                 * @param[ in ] aDvTypes dv type wrt which the derivative is evaluated
                 */
                virtual void eval_dSPdFollowerDV( const moris::Cell< PDV_Type > & aDvTypes )
                {
                    MORIS_ERROR( false, " Stabilization_Parameter::eval_dSPdFollowerDV - Not implemented for base class. " );
                }

                //------------------------------------------------------------------------------
        };

        //------------------------------------------------------------------------------
    } /* namespace fem */
} /* namespace moris */
#endif /* SRC_FEM_CL_FEM_STABILIZATION_PARAMETER_HPP_ */

