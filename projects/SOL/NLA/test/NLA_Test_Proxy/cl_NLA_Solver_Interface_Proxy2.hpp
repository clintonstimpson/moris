/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_NLA_Solver_Interface_Proxy2.hpp
 *
 */

#ifndef SRC_DISTLINALG_CL_NLA_SOLVER_INTERFACE_PROXY_2_HPP_
#define SRC_DISTLINALG_CL_NLA_SOLVER_INTERFACE_PROXY_2_HPP_

#include "cl_Matrix.hpp"
#include "linalg_typedefs.hpp"

#include "cl_DLA_Solver_Interface.hpp"

namespace moris
{
    namespace sol
    {
        class Dist_Vector;
    }
    namespace NLA
    {
        class Nonlinear_Algorithm;
        class NLA_Solver_Interface_Proxy_II : public Solver_Interface
        {
            private:
                moris::uint mNumMyDofs;                           // local dimension of the problem
                moris::Matrix< DDSMat > mMyGlobalElements;        // local-to-global map
                moris::Matrix< DDSMat > mMyGlobalElementsOverlapping;        // local-to-global map
                moris::uint mNumElements;                         // number local elements
                moris::Matrix< DDSMat > mEleDofConectivity;       // element - dof conectivities
                moris::Matrix< DDRMat > mElementMatrixValues;     // dense element matrix entries
                moris::Matrix< DDUMat > mMyConstraintDofs;        // constraint dofs
                moris::Matrix< DDRMat > mMyRHSValues;             // Vector with RHS values

                bool mUseMatrixMarketFiles;                       // determines is matrix and RHS comes from MatrixMarket files

                sol::Dist_Vector * mSolutionVector = nullptr;
                Matrix< DDRMat > mMySolVec;

                moris::sint mNX;
                moris::sint mNY;

                moris::Cell< enum MSI::Dof_Type > mListOfDofTypes;

            public :
                NLA_Solver_Interface_Proxy_II();

                NLA_Solver_Interface_Proxy_II( std::shared_ptr< Nonlinear_Algorithm > aNewtonSolver ){};

                // ----------------------------------------------------------------------------------------------
                ~NLA_Solver_Interface_Proxy_II(){};

                // ----------------------------------------------------------------------------------------------

                void set_solution_vector( sol::Dist_Vector * aSolutionVector );

                void free_block_memory( const uint aBlockInd ){};

                // ----------------------------------------------------------------------------------------------

                void set_requested_dof_types( const moris::Cell< enum MSI::Dof_Type > aListOfDofTypes )
                {
                    mListOfDofTypes = aListOfDofTypes;
                };

                const moris::Cell< enum MSI::Dof_Type > & get_requested_dof_types()
                {
                    return mListOfDofTypes;
                };

                void set_secondary_dof_types( const moris::Cell< enum MSI::Dof_Type > aListOfDofTypes )
                {
                };

                // local dimension of the problem
                uint get_max_num_global_dofs(){ return 4; };

                // ----------------------------------------------------------------------------------------------
                // local dimension of the problem
                uint get_num_my_dofs(){ return mNumMyDofs; };

                // ----------------------------------------------------------------------------------------------
                Matrix< DDSMat > get_my_local_global_map()
                {
                    if( mListOfDofTypes.size() == 1)
                    {
                        mMyGlobalElements.resize(2,1);
                        mMyGlobalElements(0,0)=0;                mMyGlobalElements(1,0)=1;
                    }
                    else if( mListOfDofTypes.size() == 2)
                    {
                        mMyGlobalElements.resize(2,1);
                        mMyGlobalElements(0,0)=2;                mMyGlobalElements(1,0)=3;
                    }
                    else if( mListOfDofTypes.size() == 3)
                    {
                        mMyGlobalElements.resize(4,1);
                        mMyGlobalElements(0,0)=0;                mMyGlobalElements(1,0)=1;
                        mMyGlobalElements(2,0)=2;                mMyGlobalElements(3,0)=3;
                    }
                    return mMyGlobalElements;
                };
                // local-to-global map
                Matrix< DDSMat > get_my_local_global_map( const moris::Cell< enum MSI::Dof_Type > & aListOfDofTypes)
                {
                    if( mListOfDofTypes.size() == 1)
                    {
                        mMyGlobalElements.resize(2,1);
                        mMyGlobalElements(0,0)=0;                mMyGlobalElements(1,0)=1;
                    }
                    else if( mListOfDofTypes.size() == 2)
                    {
                        mMyGlobalElements.resize(2,1);
                        mMyGlobalElements(0,0)=2;                mMyGlobalElements(1,0)=3;
                    }
                    else if( mListOfDofTypes.size() == 3)
                    {
                        mMyGlobalElements.resize(4,1);
                        mMyGlobalElements(0,0)=0;                mMyGlobalElements(1,0)=1;
                        mMyGlobalElements(2,0)=2;                mMyGlobalElements(3,0)=3;
                    }
                    return mMyGlobalElements;
                };

                moris::Matrix< DDSMat > get_my_local_global_overlapping_map( )
                {
                    mMyGlobalElementsOverlapping.resize(4,1);
                    mMyGlobalElementsOverlapping(0,0)=0;                mMyGlobalElementsOverlapping(1,0)=1;
                    mMyGlobalElementsOverlapping(2,0)=2;                mMyGlobalElementsOverlapping(3,0)=3;

                    return mMyGlobalElementsOverlapping;
                };

                // ----------------------------------------------------------------------------------------------
                // number of elements on proc
                uint get_num_my_elements()
                {
                    return mNumElements=1;
                };

                uint get_num_my_blocks(){return 1; };

                uint get_num_equation_objects_on_set( uint aBlockInd){return mNumElements=1; };

                // ----------------------------------------------------------------------------------------------
                void get_equation_object_operator(const uint             & aMyElementInd,
                        Matrix< DDRMat > & aElementMatrix);

                void get_equation_object_operator( const uint             & aMyBlockInd,
                        const uint             & aMyElementInd,
                        Matrix< DDRMat > & aElementMatrix);

                // ----------------------------------------------------------------------------------------------
                void  get_element_topology(const uint             & aMyElementInd,
                        Matrix< DDSMat > & aElementTopology)
                {
                    if( mListOfDofTypes.size() == 1)
                    {
                        aElementTopology.resize(2,1);
                        aElementTopology(0,0)=0;                aElementTopology(1,0)=1;
                    }
                    else if( mListOfDofTypes.size() == 2)
                    {
                        aElementTopology.resize(2,1);
                        aElementTopology(0,0)=2;                aElementTopology(1,0)=3;
                    }
                    else if( mListOfDofTypes.size() == 3)
                    {
                        aElementTopology.resize(4,1);
                        aElementTopology(0,0)=0;                aElementTopology(1,0)=1;
                        aElementTopology(2,0)=2;                aElementTopology(3,0)=3;
                    }
                }

                void  get_element_topology(const uint             & aMyBlockInd,
                        const uint             & aMyElementInd,
                        Matrix< DDSMat > & aElementTopology)
                {
                    if( mListOfDofTypes.size() == 1)
                    {
                        aElementTopology.resize(2,1);
                        aElementTopology(0,0)=0;                aElementTopology(1,0)=1;
                    }
                    else if( mListOfDofTypes.size() == 2)
                    {
                        aElementTopology.resize(2,1);
                        aElementTopology(0,0)=2;                aElementTopology(1,0)=3;
                    }
                    else if( mListOfDofTypes.size() == 3)
                    {
                        aElementTopology.resize(4,1);
                        aElementTopology(0,0)=0;                aElementTopology(1,0)=1;
                        aElementTopology(2,0)=2;                aElementTopology(3,0)=3;
                    }
                };

                // ----------------------------------------------------------------------------------------------
                Matrix< DDUMat > get_constrained_Ids(){ return mMyConstraintDofs; };

                // ----------------------------------------------------------------------------------------------
                void get_equation_object_rhs(
                        const uint                     & aMyElementInd,
                        Cell< Matrix< DDRMat > > & aElementRHS );

                void get_equation_object_rhs(
                        const uint                     & aMyBlockInd,
                        const uint                     & aMyElementInd,
                        Cell< Matrix< DDRMat > > & aElementRHS );

                //------------------------------------------------------------------------------

                void get_equation_object_operator_and_rhs(
                        const moris::uint        & aMyElementInd,
                        Matrix< DDRMat >         & aElementMatrix,
                        Cell< Matrix< DDRMat > > & aElementRHS );

                void get_equation_object_operator_and_rhs(
                        const moris::uint        & aMyEquSetInd,
                        const moris::uint        & aMyElementInd,
                        Matrix< DDRMat >         & aElementMatrix,
                        Cell< Matrix< DDRMat > > & aElementRHS );

                // ----------------------------------------------------------------------------------------------

                void use_matrix_market_files( )
                {
                    mUseMatrixMarketFiles = true;
                };

                // ----------------------------------------------------------------------------------------------

                const char* get_matrix_market_path( )
                {
                    if ( mUseMatrixMarketFiles == true )
                    {
                        const char* tFilePath ="/home/schmidt/codes/MORIS/test/src/distlinalg/";
                        return tFilePath;
                    }
                    else
                    {
                        return NULL;
                    }
                };

                // ----------------------------------------------------------------------------------------------

                uint get_num_rhs()
                {
                    return 1;
                }
        };
    }
}
#endif /* SRC_DISTLINALG_CL_NLA_SOLVER_INTERFACE_PROXY_2_HPP_ */

