/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_HMR_T_Matrix_Base.cpp
 *
 */

#include "cl_HMR_T_Matrix.hpp"    //HMR/src
#include "HMR_Globals.hpp"    //HMR/src
#include "HMR_Tools.hpp"
#include "fn_eye.hpp"
#include "fn_inv.hpp"      //LINALG/src

namespace moris::hmr
{

    //-------------------------------------------------------------------------------

    T_Matrix_Base::T_Matrix_Base(
            Lagrange_Mesh_Base* aLagrangeMesh,
            BSpline_Mesh_Base*  aBSplineMesh,
            bool                aTruncate )
            : mLagrangeMesh( aLagrangeMesh )
            , mBSplineMesh( aBSplineMesh )
    {
        this->init_lagrange_coefficients();

        // set function pointer
        if ( aTruncate )
        {
            mTMatrixFunction = &T_Matrix_Base::calculate_truncated_t_matrix;
        }
        else
        {
            mTMatrixFunction = &T_Matrix_Base::calculate_untruncated_t_matrix;
        }
    }

    //-------------------------------------------------------------------------------

    T_Matrix_Base::~T_Matrix_Base()
    {
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::calculate_t_matrix(
            luint             aMemoryIndex,
            Matrix< DDRMat >& aTMatrixTransposed,
            Cell< Basis* >&   aDOFs )
    {
        ( this->*mTMatrixFunction )( aMemoryIndex,
                aTMatrixTransposed,
                aDOFs );
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::calculate_untruncated_t_matrix(
            luint      aMemoryIndex,
            Matrix< DDRMat >& aTMatrixTransposed,
            Cell< Basis* >&   aDOFs )
    {
        aDOFs.clear();

        Element* tElement = mBSplineMesh->get_element_by_memory_index( aMemoryIndex );

        // get level of element
        uint tLevel = tElement->get_level();

        // get number of basis per element
        uint tNumberOfBasisPerElement = mBSplineMesh->get_number_of_basis_per_element();

        // help index for total number of basis
        uint tMaxNumberOfBasis = ( tLevel + 1 ) * tNumberOfBasisPerElement;

        // allocate max memory for matrix
        aTMatrixTransposed.set_size( tNumberOfBasisPerElement, tMaxNumberOfBasis, 0 );

        // allocate max memory for Basis
        aDOFs.resize( tMaxNumberOfBasis, nullptr );

        // write unity into level matrix
        Matrix< DDRMat > tT( mEye );

        // counter for basis
        uint tBasisCount = 0;

        // get pointer to parent
        Element* tParent = tElement;

        // loop over all levels and assemble transposed T-Matrix
        for ( uint iLevelIndex = 0; iLevelIndex <= tLevel; iLevelIndex++ )
        {
            // copy basis indices
            for ( uint iBasisIndex = 0; iBasisIndex < tNumberOfBasisPerElement; iBasisIndex++ )
            {
                // get pointer to basis
                Basis* tBasis = tParent->get_basis( iBasisIndex );

                // test if basis is active
                if ( tBasis->is_active() )
                {
                    // copy columns into matrix
                    aTMatrixTransposed.set_column( tBasisCount, tT.get_column( iBasisIndex ) );

                    // copy pointer to basis into output array
                    aDOFs( tBasisCount++ ) = tBasis;
                }
            }

            // left-multiply T-Matrix with child matrix
            tT = tT * mChild( tParent->get_background_element()->get_child_index() );

            // jump to next
            tParent = mBSplineMesh->get_parent_of_element( tParent );
        }

        // Shrink memory to exact size
        aDOFs.resize( tBasisCount, nullptr );
        aTMatrixTransposed.resize( tNumberOfBasisPerElement, tBasisCount );
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::calculate_truncated_t_matrix(
            luint             aMemoryIndex,
            Matrix< DDRMat >& aTMatrixTransposed,
            Cell< Basis* >&   aDOFs )
    {
        // Clear adofs
        aDOFs.clear();

        // Get element from memory
        Element* tElement = mBSplineMesh->get_element_by_memory_index( aMemoryIndex );

        // get level of element
        uint tLevel = tElement->get_level();

        // get number of basis per element
        uint tNumberOfBasisPerElement = mBSplineMesh->get_number_of_basis_per_element();

        // help index for total number of basis
        uint tNumberOfBasis = ( tLevel + 1 ) * tNumberOfBasisPerElement;

        // initialize counter
        uint tDOFCount = 0;
        uint tBasisCount = 0;

        // container for basis levels
        moris::Cell< Basis* > tAllBasis( tNumberOfBasis, nullptr );

        // size T-matrix
        aTMatrixTransposed.set_size( tNumberOfBasisPerElement, tNumberOfBasis, 0.0 );
        aDOFs.resize( tNumberOfBasis, nullptr );

        // copy basis on lowest level into output
        for ( uint iBasisIndex = 0; iBasisIndex < tNumberOfBasisPerElement; iBasisIndex++ )
        {
            Basis* tBasis = tElement->get_basis( iBasisIndex );
            tAllBasis( tBasisCount++ ) = tBasis;

            if ( tBasis->is_active() )
            {
                aTMatrixTransposed.set_column( tDOFCount, mEye.get_column( iBasisIndex ) );
                aDOFs( tDOFCount++ ) = tBasis;
            }
        }

        // ask B-Spline mesh for number of children per basis
        auto tNumberOfChildrenPerBasis = mBSplineMesh->get_number_of_children_per_basis();

        // initialize child indices and size counter
        Cell< uint > tChildIndices;
        uint                tSizeCounter = 1;

        // jump to next parent
        if ( tLevel > 0 )
        {
            Element* tParent = mBSplineMesh->get_parent_of_element( tElement );

            // Size child indices/matrix
            tChildIndices.resize( tSizeCounter++, tParent->get_background_element()->get_child_index() );
            Matrix< DDRMat > const & tChildMatrix = this->get_child_matrix( tChildIndices );

            // loop over all basis of this level
            for ( uint iBasisIndex = 0; iBasisIndex < tNumberOfBasisPerElement; iBasisIndex++ )
            {
                // get pointer to basis
                Basis* tBasis = tParent->get_basis( iBasisIndex );

                // test if basis is active
                if ( tBasis->is_active() )
                {
                    // loop over all children of this basis
                    for ( uint iChildNumber = 0; iChildNumber < tNumberOfChildrenPerBasis; iChildNumber++ )
                    {
                        // get pointer to child of basis
                        Basis* tChild = tBasis->get_child( iChildNumber );

                        // test if child exists
                        if ( tChild != nullptr )
                        {
                            // test if child is not active
                            if ( !tChild->is_active() && !tChild->is_refined() )
                            {
                                // get memory index of child
                                luint tChildBasisIndex = tChild->get_memory_index();

                                // search for child in element
                                for ( uint iBasisSearchIndex = 0; iBasisSearchIndex < tNumberOfBasisPerElement; iBasisSearchIndex++ )
                                {
                                    if ( tAllBasis( iBasisSearchIndex )->get_memory_index() == tChildBasisIndex )
                                    {
                                        // fixme: this operation is supposed to work the same way for both Armadillo and Eigen.
#ifdef MORIS_USE_EIGEN
                                        //                                                aTMatrixTransposed.set_column( tCount,
                                        //                                                        aTMatrixTransposed.get_column( tCount ).matrix_data()
                                        //                                                        + mTruncationWeights( j ) * tTmatrixTransposed.get_column( i ).matrix_data() );
                                        // aTMatrixTransposed.set_column( tCount,
                                        //         aTMatrixTransposed.get_column( tCount ).matrix_data()
                                        //         + mTruncationWeights( j ) * tTmatrixTransposed.get_column( i ).matrix_data() );
                                        aTMatrixTransposed.set_column( tCount,
                                                aTMatrixTransposed.get_column( tCount ).matrix_data()
                                                        + mTruncationWeights( j ) * tChildMatrix.get_column( tCounter_2 ).matrix_data() );
#else
                                        /*                                                tTMatrixTruncatedTransposed.set_column( tCount,
                                                tTMatrixTruncatedTransposed.get_column( tCount )
                                                + mTruncationWeights( j ) * tTmatrixTransposed.get_column( i ) ); */
                                        // try direct arma access, see how much time we loose
                                        //                                                aTMatrixTransposed.matrix_data().col( tCount ) = aTMatrixTransposed.matrix_data().col( tCount )
                                        //                                                          + mTruncationWeights( j ) * tTmatrixTransposed.matrix_data().col( i );
                                        aTMatrixTransposed.matrix_data().col( tDOFCount ) = aTMatrixTransposed.matrix_data().col( tDOFCount )
                                                                                       + mTruncationWeights( iChildNumber ) * tChildMatrix.matrix_data().col( iBasisSearchIndex );
#endif
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if ( aTMatrixTransposed.get_column( tDOFCount ).min() < -gEpsilon || aTMatrixTransposed.get_column( tDOFCount ).max() > gEpsilon )
                    {
                        // copy parent index
                        aDOFs( tDOFCount++ ) = tBasis;
                    }
                    else
                    {
                        // reset this column to zero
                        aTMatrixTransposed.set_column( tDOFCount, mZero.get_column( 0 ) );
                    }
                }

                tAllBasis( tBasisCount++ ) = tBasis;
            }
        }

        // Shrink output to exact size
        aTMatrixTransposed.resize( tNumberOfBasisPerElement, tDOFCount );
        aDOFs.resize( tDOFCount, nullptr );
    }

    //-------------------------------------------------------------------------------

    const Matrix< DDRMat >& T_Matrix_Base::get_child_matrix( const Cell< uint >& aChildIndices )
    {
        uint tSize = aChildIndices.size();
        if ( tSize == 1 )
        {
            return mEye;
        }
        else if ( tSize == 2 )
        {
            return mChild( aChildIndices( 0 ) );
        }
        else if ( tSize == 3 )
        {
            return mChildMultiplied( aChildIndices( 0 ) )( aChildIndices( 1 ) );
        }
        else
        {
            MORIS_ERROR( false, "only child matrices for level 1 and 2 implemented yet." );
            return mChild( 0 );
        }
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::init_lagrange_coefficients()
    {
        // number of Lagrange nodes per direction
        mLagrangeOrder = mLagrangeMesh->get_order();

        // Step 2: first, we initialize the parameter coordinates

        uint tNumberOfNodes = mLagrangeOrder + 1;

        // matrix containing parameter coordinates for points
        Matrix< DDRMat > tXi( tNumberOfNodes, 1, 0.0 );

        // stepwidth
        real tDeltaXi = 2.0 / mLagrangeOrder;

        // first value
        tXi( 0 ) = -1.0;

        // intermediate values
        for ( uint iOrder = 1; iOrder < mLagrangeOrder; iOrder++ )
        {
            tXi( iOrder ) = tXi( iOrder - 1 ) + tDeltaXi;
        }

        // last value
        tXi( mLagrangeOrder ) = 1.0;

        // Step 3: we build a Vandermonde matrix
        Matrix< DDRMat > tVandermonde( tNumberOfNodes, tNumberOfNodes, 0.0 );

        for ( uint iRowIndex = 0; iRowIndex < tNumberOfNodes; iRowIndex++ )
        {
            for ( uint iColIndex = 0; iColIndex < tNumberOfNodes; iColIndex++ )
            {
                tVandermonde( iRowIndex, iColIndex ) = std::pow( tXi( iRowIndex ), tNumberOfNodes - iColIndex - 1 );
            }
        }

        // invert the Vandermonde matrix and store coefficients
        mLagrangeCoefficients = inv( tVandermonde );
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::evaluate(
            const uint aBSplineMeshIndex,
            const bool aBool )
    {
        // get B-Spline pattern of this mesh
        auto tBSplinePattern = mBSplineMesh->get_activation_pattern();

        // select pattern
        mLagrangeMesh->select_activation_pattern();

        // get number of elements on this Lagrange mesh
        luint tNumberOfElements = mLagrangeMesh->get_number_of_elements();

        // unflag all nodes on this mesh
        mLagrangeMesh->unflag_all_basis();

        // number of nodes per element
        uint tNumberOfNodesPerElement = mLagrangeMesh->get_number_of_basis_per_element();

        // unity matrix
        Matrix< DDRMat > tEye;
        eye( tNumberOfNodesPerElement, tNumberOfNodesPerElement, tEye );

        // calculate transposed Lagrange T-Matrix
        Matrix< DDRMat > tL( this->get_lagrange_matrix() );

        // loop over all elements
        for ( luint iElementIndex = 0; iElementIndex < tNumberOfElements; iElementIndex++ )
        {
            // get pointer to element
            auto tLagrangeElement = mLagrangeMesh->get_element( iElementIndex );

            // FIXME : activate this flag
            // if ( tLagrangeElement->get_t_matrix_flag() )
            {
                // get pointer to background element
                auto tBackgroundElement = tLagrangeElement->get_background_element();

                // initialize refinement Matrix
                Matrix< DDRMat > tR;

                bool tLagrangeEqualBspline    = false;
                bool tFirstLagrangeRefinement = true;

                while ( !tBackgroundElement->is_active( tBSplinePattern ) )
                {
                    if ( tFirstLagrangeRefinement )
                    {
                        // right multiply refinement matrix
                        tR                       = this->get_refinement_matrix( tBackgroundElement->get_child_index() );
                        tFirstLagrangeRefinement = false;
                        tLagrangeEqualBspline    = true;
                    }
                    else
                    {
                        tR = tR * this->get_refinement_matrix( tBackgroundElement->get_child_index() );
                    }

                    // jump to parent
                    tBackgroundElement = tBackgroundElement->get_parent();
                }

                // calculate the B-Spline T-Matrix
                Matrix< DDRMat > tB;
                Cell< Basis* >   tDOFs;

                this->calculate_t_matrix(
                        tBackgroundElement->get_memory_index(),
                        tB,
                        tDOFs );

                Matrix< DDRMat > tT;

                if ( tLagrangeEqualBspline )
                {
                    // transposed T-Matrix
                    tT = tR * tL * tB;
                }
                else
                {
                    tT = tL * tB;
                }

                // number of columns in T-Matrix
                uint tNCols = tT.n_cols();

                // epsilon to count T-Matrix
                real tEpsilon = 1e-12;

                // loop over all nodes of this element
                for ( uint iNodeIndex = 0; iNodeIndex < tNumberOfNodesPerElement; iNodeIndex++ )
                {
                    // pointer to node
                    auto tNode = tLagrangeElement->get_basis( iNodeIndex );

                    // test if node is flagged
                    if ( !tNode->is_flagged() )
                    {
                        // initialize counter
                        uint tNodeCount = 0;

                        // reserve DOF cell
                        Cell< mtk::Vertex* > tNodeDOFs( tNCols, nullptr );

                        // reserve matrix with coefficients
                        Matrix< DDRMat > tCoefficients( tNCols, 1 );

                        // loop over all nonzero entries
                        for ( uint iColIndex = 0; iColIndex < tNCols; ++iColIndex )
                        {
                            if ( std::abs( tT( iNodeIndex, iColIndex ) ) > tEpsilon )
                            {
                                // copy entry of T-Matrix
                                tCoefficients( tNodeCount ) = tT( iNodeIndex, iColIndex );

                                // copy pointer of dof and convert to mtk::Vertex
                                tNodeDOFs( tNodeCount++ ) = tDOFs( iColIndex );

                                // flag this DOF
                                tDOFs( iColIndex )->flag();
                            }
                        }

                        tCoefficients.resize( tNodeCount, 1 );
                        tNodeDOFs.resize( tNodeCount );

                        if ( aBool )
                        {
                            // init interpolation container for this node
                            tNode->init_interpolation( aBSplineMeshIndex );

                            // store the coefficients
                            tNode->set_weights( aBSplineMeshIndex, tCoefficients );

                            // store pointers to the DOFs
                            tNode->set_coefficients( aBSplineMeshIndex, tNodeDOFs );
                        }
                        // flag this node as processed
                        tNode->flag();
                    }
                }    // end loop over all nodes of this element
            }        // end if element is flagged
        }            // end loop over all elements
    }

    //-------------------------------------------------------------------------------

    void T_Matrix_Base::evaluate_trivial(
            const uint aBSplineMeshIndex,
            const bool aBool )
    {
        // select pattern
        mLagrangeMesh->select_activation_pattern();

        // get number of elements on this Lagrange mesh
        luint tNumberOfElements = mLagrangeMesh->get_number_of_elements();

        // unflag all nodes on this mesh
        mLagrangeMesh->unflag_all_basis();

        // number of nodes per element
        uint tNumberOfNodesPerElement = mLagrangeMesh->get_number_of_basis_per_element();

        // loop over all elements
        for ( luint iElementIndex = 0; iElementIndex < tNumberOfElements; iElementIndex++ )
        {
            // get pointer to element
            auto tLagrangeElement = mLagrangeMesh->get_element( iElementIndex );

            // loop over all nodes of this element
            for ( uint iNodeIndex = 0; iNodeIndex < tNumberOfNodesPerElement; iNodeIndex++ )
            {
                // pointer to node
                auto tNode = tLagrangeElement->get_basis( iNodeIndex );

                // test if node is flagged
                if ( !tNode->is_flagged() )
                {
                    // reserve matrix with coefficients
                    Matrix< DDRMat > tCoefficients( 1, 1, 1.0 );

                    // reserve DOF cell
                    Cell< mtk::Vertex* > tNodeDOFs( 1, tNode );

                    if ( aBool )
                    {
                        // init interpolation container for this node
                        tNode->init_interpolation( aBSplineMeshIndex );

                        // store the coefficients
                        tNode->set_weights( aBSplineMeshIndex, tCoefficients );

                        // store pointers to the DOFs
                        tNode->set_coefficients( aBSplineMeshIndex, tNodeDOFs );
                    }
                    // flag this node as processed
                    tNode->flag();
                }
            }    // end loop over all nodes of this element
        }        // end loop over all elements
    }

    //-------------------------------------------------------------------------------

    void
    T_Matrix_Base::evaluate_extended_t_matrix(
            Element*                                    aBsplineElement,
            Element*                                    aLagrangeElement,
            moris::Cell< moris::Cell< mtk::Vertex* > >& aBsplineBasis,
            moris::Cell< Matrix< DDRMat > >&            aWeights )
    {
        Background_Element_Base* aBSpBackgroundElement = aBsplineElement->get_background_element();

        // evaluate
        this->init_modified_lagrange_parameter_coordinates( aLagrangeElement, aBsplineElement );
        this->recompute_lagrange_matrix();

        // get B-Spline pattern of this mesh
        uint tBSplinePattern = mBSplineMesh->get_activation_pattern();

        // select pattern
        mLagrangeMesh->select_activation_pattern();

        // unflag all nodes on this mesh
        mLagrangeMesh->unflag_all_basis();

        // number of nodes per element
        uint tNumberOfNodesPerElement = mLagrangeMesh->get_number_of_basis_per_element();

        // unity matrix
        Matrix< DDRMat > tEye;
        eye( tNumberOfNodesPerElement, tNumberOfNodesPerElement, tEye );

        // calculate transposed Lagrange T-Matrix
        Matrix< DDRMat > tL( mTMatrixLagrangeModified );

        // get pointer to background element
        auto tBackgroundElement = aLagrangeElement->get_background_element();

        // initialize refinement Matrix
        Matrix< DDRMat > tR;

        bool tLagrangeEqualBspline    = false;
        bool tFirstLagrangeRefinement = true;

        while ( !tBackgroundElement->is_active( tBSplinePattern ) )
        {
            if ( tFirstLagrangeRefinement )
            {
                // right multiply refinement matrix
                tR                       = this->get_refinement_matrix( tBackgroundElement->get_child_index() );
                tFirstLagrangeRefinement = false;
                tLagrangeEqualBspline    = true;
            }
            else
            {
                tR = tR * this->get_refinement_matrix( tBackgroundElement->get_child_index() );
            }

            // jump to parent
            tBackgroundElement = tBackgroundElement->get_parent();
        }

        // calculate the B-Spline T-Matrix
        Matrix< DDRMat > tB;
        Cell< Basis* >   tDOFs;

        this->calculate_t_matrix(
                aBSpBackgroundElement->get_memory_index(),
                tB,
                tDOFs );

        Matrix< DDRMat > tT;

        if ( tLagrangeEqualBspline )
        {
            // transposed T-Matrix
            tT = tR * tL * tB;
        }
        else
        {
            tT = tL * tB;
        }

        // number of columns in T-Matrix
        uint tNCols = tT.n_cols();

        // epsilon to count T-Matrix
        real tEpsilon = 1e-12;

        // resize the arrays
        aBsplineBasis.resize(tNumberOfNodesPerElement );
        aWeights.resize(tNumberOfNodesPerElement );

        // loop over all nodes of this element
        for ( uint k = 0; k < tNumberOfNodesPerElement; ++k )
        {
            // initialize counter
            uint tCount = 0;

            // reserve DOF cell
            Cell< mtk::Vertex* > tNodeDOFs( tNCols, nullptr );

            // reserve matrix with coefficients
            Matrix< DDRMat > tCoefficients( tNCols, 1 );

            // loop over all nonzero entries
            for ( uint i = 0; i < tNCols; ++i )
            {
                if ( std::abs( tT( k, i ) ) > tEpsilon )
                {
                    // copy entry of T-Matrix
                    tCoefficients( tCount ) = tT( k, i );

                    // copy pointer of dof and convert to mtk::Vertex
                    tNodeDOFs( tCount ) = tDOFs( i );

                    // flag this DOF
                    tDOFs( i )->flag();

                    // increment counter
                    ++tCount;
                }
            }

            tCoefficients.resize( tCount, 1 );
            tNodeDOFs.resize( tCount );

            aBsplineBasis(k ) = tNodeDOFs;
            aWeights(k )      = tCoefficients;
        }
    }

    //-------------------------------------------------------------------------------

}
