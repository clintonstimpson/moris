/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * fn_diag_mat.hpp
 *
 */

#ifndef PROJECTS_LINALG_SRC_FN_DIAG_MAT_HPP_
#define PROJECTS_LINALG_SRC_FN_DIAG_MAT_HPP_

#include "cl_Matrix.hpp"

#ifdef MORIS_USE_EIGEN
#include "Eigen_Impl/fn_diag_mat_Eigen.hpp"
#endif

#ifdef MORIS_USE_ARMA
#include "fn_diag_mat_Arma.hpp"
#endif

namespace moris
{
/**
 * @brief Extract the k-th diagonal from matrix A.
 *
 * @param[in] aA Matrix.
 * @param[in] ak Diagonal index.
 * The argument k is optional; by default the main diagonal
 * is extracted (k=0).\n
 * For k > 0, the k-th super-diagonal is extracted
 * (top-right corner).\n
 * For k < 0, the k-th sub-diagonal is extracted
 * (bottom-left corner).\n
 *
 * @return Creates a vector from the diagonal of a matrix such that
 * @f$ \mathbf{v}_{i}=\mathbf{A}_{ii}@f$ \n
 * The extracted diagonal is interpreted as a column vector.
 *
 */

template< typename Matrix_Type >
auto
diag_mat(   Matrix< Matrix_Type >  & aA,
        size_t     const & ak = 0 )
->decltype( diag_mat( aA.matrix_data(), ak ) )
{
    return diag_mat( aA.matrix_data(), ak );
}

}

#endif /* PROJECTS_LINALG_SRC_FN_DIAG_MAT_HPP_ */

