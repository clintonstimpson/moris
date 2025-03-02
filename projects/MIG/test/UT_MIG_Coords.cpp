/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * UT_MIG_Coords.cpp
 *
 */

#include "catch.hpp"
#include "paths.hpp"
#include "cl_Matrix.hpp"
#include "cl_Communication_Tools.hpp"
#include "op_equal_equal.hpp"

namespace moris::pc
{
    TEST_CASE("Dummy Test", "[MIG],[MIG_Dummy]")
    {
        if ( par_size() <= 1 )
        {
             Matrix <DDRMat> tMat = {{1.0}};

             CHECK( tMat(0)== 1.0);
        }
    }
} // namespace moris::pc

