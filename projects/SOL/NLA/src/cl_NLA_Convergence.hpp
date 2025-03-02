/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * cl_NLA_Convergence.hpp
 *
 */
#ifndef SRC_FEM_CL_NLA_CONVERGENCE_HPP_
#define SRC_FEM_CL_NLA_CONVERGENCE_HPP_

#include "typedefs.hpp"
#include "cl_NLA_Nonlinear_Solver.hpp"
#include "cl_NLA_Nonlinear_Algorithm.hpp"

namespace moris
{
    namespace sol
    {
        class Dist_Vector;
    }
    namespace NLA
    {
        class Convergence
        {
          private:

          public:
            Convergence(){};

            ~Convergence(){};

            bool check_for_convergence(
                    Nonlinear_Algorithm* tNonLinSolver,
                    moris::sint          aIt,
                    moris::sint          aMaxIter,
                    bool&                aHartBreak );

            bool check_for_convergence(
                    Nonlinear_Algorithm* tNonLinSolver,
                    moris::sint          aIt,
                    bool&                aHartBreak );
        };
    }    // namespace NLA
}    // namespace moris

#endif /* SRC_FEM_CL_NLA_CONVERGENCE_HPP_ */
