/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_FEM_CM_Diffusion_Linear_Isotropic_Phase_Change.hpp
 *
 */

#ifndef SRC_FEM_CL_FEM_CM_DIFFUSION_LINEAR_ISOTROPIC_PHASE_CHANGE_HPP_
#define SRC_FEM_CL_FEM_CM_DIFFUSION_LINEAR_ISOTROPIC_PHASE_CHANGE_HPP_

#include <map>

#include "typedefs.hpp"                     //MRS/COR/src
#include "cl_Cell.hpp"                      //MRS/CNT/src

#include "cl_Matrix.hpp"                    //LINALG/src
#include "linalg_typedefs.hpp"              //LINALG/src

#include "cl_FEM_Field_Interpolator.hpp"    //FEM/INT/src
#include "cl_FEM_Constitutive_Model.hpp"    //FEM/INT/src
#include "cl_FEM_CM_Diffusion_Linear_Isotropic.hpp"

namespace moris
{
    namespace fem
    {
        //------------------------------------------------------------------------------

        class CM_Diffusion_Linear_Isotropic_Phase_Change : public CM_Diffusion_Linear_Isotropic
        {

                //------------------------------------------------------------------------------
            protected:

                // default local properties
                std::shared_ptr< Property > mPropLatentHeat = nullptr;
                std::shared_ptr< Property > mPropPCTemp     = nullptr;
                std::shared_ptr< Property > mPropPSFunc     = nullptr;
                std::shared_ptr< Property > mPropPCConst    = nullptr;

            private:

                // default dof type for CM
                MSI::Dof_Type mTempDof = MSI::Dof_Type::TEMP;

                // property type for CM
                enum class CM_Property_Type
                {
                    CONDUCTIVITY,
                    HEAT_CAPACITY,
                    DENSITY,
                    LATENT_HEAT,
                    PC_TEMP,
                    PHASE_STATE_FUNCTION,
                    PHASE_CHANGE_CONST,
                    MAX_ENUM
                };

                //------------------------------------------------------------------------------
            public:
                /*
                 * trivial constructor
                 */
                CM_Diffusion_Linear_Isotropic_Phase_Change();

                //------------------------------------------------------------------------------
                /**
                 * trivial destructor
                 */
                ~CM_Diffusion_Linear_Isotropic_Phase_Change(){};

                //------------------------------------------------------------------------------
                /*
                 * @return constitutive_type
                 */
                Constitutive_Type
                get_constitutive_type() const
                {
                    return Constitutive_Type::DIFF_LIN_ISO_PC;
                }

                //------------------------------------------------------------------------------
                /**
                 * set constitutive model dof types
                 * @param[ in ] aDofTypes a list of group of dof types
                 * @param[ in ] aDofStrings a list of strings to describe the dof types
                 */
                void set_dof_type_list(
                        moris::Cell< moris::Cell< MSI::Dof_Type > > aDofTypes,
                        moris::Cell< std::string >                  aDofStrings );

                //------------------------------------------------------------------------------
                /**
                 * set constitutive model dv types
                 * @param[ in ] aDvTypes a list of group of dv types
                 * @param[ in ] aDvStrings a list of strings to describe the dv types
                 */
                void set_dv_type_list(
                        moris::Cell< moris::Cell< PDV_Type > > aDvTypes,
                        moris::Cell< std::string >             aDvStrings )
                {
                    Constitutive_Model::set_dv_type_list( aDvTypes );
                }

                //------------------------------------------------------------------------------
                /**
                 * set local properties
                 */
                void set_local_properties();

                //------------------------------------------------------------------------------
                /**
                 * evaluates the constitutive model enthalpy
                 */
                void eval_Energy();

                //------------------------------------------------------------------------------
                /**
                 * evaluates the constitutive model change rate of enthalpy
                 */
                void eval_EnergyDot();

                //------------------------------------------------------------------------------
                /**
                 * evaluates the constitutive model change rate of spatial gradient of enthalpy (needed for GGLS-stabilization)
                 */
                void eval_gradEnergy();

                //------------------------------------------------------------------------------
                /**
                 * evaluates the constitutive model change rate of spatial gradient of enthalpy (needed for GGLS-stabilization)
                 */
                void eval_gradEnergyDot();

                //------------------------------------------------------------------------------
                /**
                 * evaluate the constitutive model enthalpy wrt to a dof type
                 * @param[ in ] aDofTypes a dof type wrt which the derivative is evaluated
                 * dEnergydDOF ( 1 x numDerDof )
                 */
                void eval_dEnergydDOF( const moris::Cell< MSI::Dof_Type > & aDofTypes );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the constitutive model enthalpy change rate wrt to a dof type
                 * @param[ in ] aDofTypes a dof type wrt which the derivative is evaluated
                 * dEnergyDotdDOF ( 1 x numDerDof )
                 */
                void eval_dEnergyDotdDOF( const moris::Cell< MSI::Dof_Type > & aDofTypes );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the constitutive model gradient of enthalpy wrt to a dof type
                 * @param[ in ] aDofTypes a dof type wrt which the derivative is evaluated
                 * dGradEnergydDOF ( mSpaceDim x numDerDof )
                 */
                void eval_dGradEnergydDOF( const moris::Cell< MSI::Dof_Type > & aDofTypes );

                //------------------------------------------------------------------------------
                /**
                 * evaluate the constitutive model gradient of enthalpy change rate wrt to a dof type
                 * @param[ in ] aDofTypes a dof type wrt which the derivative is evaluated
                 * dgradEnergyDotdDOF ( mSpaceDim x numDerDof )
                 */
                void eval_dGradEnergyDotdDOF( const moris::Cell< MSI::Dof_Type > & aDofTypes );

        };
        //------------------------------------------------------------------------------
    } /* namespace fem */
} /* namespace moris */

#endif /* SRC_FEM_CL_FEM_CM_DIFFUSION_LINEAR_ISOTROPIC_PHASE_CHANGE_HPP_ */

