/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * example_test_case.cpp.template
 *
 */

#include <catch.hpp>

#include "cl_Logger.hpp"                  // MRS/IOS/src
#include "cl_MTK_Exodus_IO_Helper.hpp"    // MTK/src
#include "cl_Communication_Tools.hpp"     // MRS/COM/src

#include "cl_Matrix.hpp"
#include "fn_norm.hpp"

using namespace moris;

//---------------------------------------------------------------

// global variable for interpolation order
uint gInterpolationOrder;

// flag to print reference values
bool gPrintReferenceValues = false;

//---------------------------------------------------------------

int fn_WRK_Workflow_Main_Interface( int argc, char * argv[] );

//---------------------------------------------------------------

extern "C" void
check_results(
        std::string aExoFileName,
        uint        aTestCaseIndex )
{
    MORIS_LOG_INFO( "" );
    MORIS_LOG_INFO( "Checking Results - Test Case %d on %i processor.", aTestCaseIndex, par_size() );
    MORIS_LOG_INFO( "" );

    // open and query exodus output file (set verbose to true to get basic mesh information)
    moris::mtk::Exodus_IO_Helper tExoIO( aExoFileName.c_str(), 0, false, true );

    // define reference node IDs
    Cell< uint > tReferenceNodeId = { 10, 140 };

    if ( gPrintReferenceValues )
    {
        std::cout << "Test case index: " << aTestCaseIndex << std::endl;

        uint tNumDims  = tExoIO.get_number_of_dimensions();
        uint tNumNodes = tExoIO.get_number_of_nodes();
        uint tNumElems = tExoIO.get_number_of_elements();

        std::cout << "Number of dimensions: " << tNumDims << std::endl;
        std::cout << "Number of nodes     : " << tNumNodes << std::endl;
        std::cout << "Number of elements  : " << tNumElems << std::endl;

        // coordinates of reference point
        moris::print( tExoIO.get_nodal_coordinate( tReferenceNodeId( aTestCaseIndex ) ), "Coordinates of reference point" );

        // time value for reference time step
        std::cout << "Time value: " << std::scientific << std::setprecision( 15 ) << tExoIO.get_time_value() << std::endl;

        // solution of reference point at reference time step
        std::cout << "Temperature at reference point: " << std::scientific << std::setprecision( 15 ) << tExoIO.get_nodal_field_value( tReferenceNodeId( aTestCaseIndex ), 2, 0 ) << std::endl;

        // value of IQI at reference time step
        std::cout << "IQI value: " << std::scientific << std::setprecision( 15 ) << tExoIO.get_global_variable( 0, 0 ) << std::endl;

        return;
    }

    // define reference values for dimension, number of nodes and number of elements
    Cell< uint > tReferenceNumDims  = { 2, 2 };
    Cell< uint > tReferenceNumNodes = { 126, 25 };
    Cell< uint > tReferenceNumElems = { 52, 4};

    // check dimension, number of nodes and number of elements
    uint tNumDims  = tExoIO.get_number_of_dimensions();
    uint tNumNodes = tExoIO.get_number_of_nodes();
    uint tNumElems = tExoIO.get_number_of_elements();

    MORIS_LOG_INFO( "Check number of dimensions: reference %12d, actual %12d, percent error %12.5e.",
            tReferenceNumDims( aTestCaseIndex ),
            tNumDims,
            std::abs( ( tNumDims - tReferenceNumDims( aTestCaseIndex ) ) / tReferenceNumDims( aTestCaseIndex ) * 100.0 ) );
    MORIS_LOG_INFO( "Check number of nodes:      reference %12d, actual %12d, percent error %12.5e.",
            tReferenceNumNodes( aTestCaseIndex ),
            tNumNodes,
            std::abs( ( tNumNodes - tReferenceNumNodes( aTestCaseIndex ) ) / tReferenceNumNodes( aTestCaseIndex ) * 100.0 ) );
    MORIS_LOG_INFO( "Check number of elements:   reference %12d, actual %12d, percent error %12.5e.",
            tReferenceNumElems( aTestCaseIndex ),
            tNumElems,
            std::abs( ( tNumElems - tReferenceNumElems( aTestCaseIndex ) ) / tReferenceNumElems( aTestCaseIndex ) * 100.0 ) );

    CHECK( tNumDims == tReferenceNumDims( aTestCaseIndex ) );
    CHECK( tNumNodes == tReferenceNumNodes( aTestCaseIndex ) );
    CHECK( tNumElems == tReferenceNumElems( aTestCaseIndex ) );

    real tL2ErrorActual       = tExoIO.get_global_variable( 0, 0 );
    real tDiffusiveFluxActual = tExoIO.get_global_variable( 1, 0 );

    Cell<real> tL2ErrorReference = { 6.65582e-07, 6.65582e-07 };
    Cell<real> tDiffusiveFluxReference = { 0.0504293, 0.0504293 };

    real tRelL2Difference = ( tL2ErrorActual - tL2ErrorReference( aTestCaseIndex ) ) /  tL2ErrorReference( aTestCaseIndex ) ;
    real tRelFluxDifference = ( tDiffusiveFluxActual - tDiffusiveFluxReference( aTestCaseIndex ) ) /  tDiffusiveFluxReference( aTestCaseIndex ) ;

    MORIS_LOG_INFO( "Check nodal temperature:  reference %12.5e, actual %12.5e, percent error %12.5e.",
            tL2ErrorReference( aTestCaseIndex ) ,
            tL2ErrorActual,
            tRelL2Difference * 100.0 );

    MORIS_LOG_INFO( "Check nodal temperature:  reference %12.5e, actual %12.5e, percent error %12.5e.",
            tDiffusiveFluxReference( aTestCaseIndex ),
            tDiffusiveFluxActual,
            tRelFluxDifference * 100.0 );

    CHECK( tRelL2Difference < 1.0e-6 );
    CHECK( tRelFluxDifference < 1.0e-6 );
}

TEST_CASE("Laplace",
        "[moris],[example],[thermal],[Laplace_2D],[Laplace]")
{
    // define command line call
    int argc = 2;

    char tString1[] = "";
    char tString2[] = "./Laplace.so";

    char * argv[2] = {tString1,tString2};

    // call to performance manager main interface
    int tRet = fn_WRK_Workflow_Main_Interface( argc, argv );

    // check
    REQUIRE( tRet ==  0 );

     // set interpolation order
    gInterpolationOrder = 1;

    MORIS_LOG_INFO( "" );
    MORIS_LOG_INFO( "Executing HeatConduction: Interpolation order 1 - %i Processors.", par_size() );
    MORIS_LOG_INFO( "" );

    // check results
    switch ( par_size() )
    {
        // Test Case 0
        case 1:
        {
            // perform check
            check_results( "Laplace.exo.e-s.0000", 0 );
            break;
        }
        // Test Case 1
        case 4:
        {
            if ( par_rank() == 1 )
            {
                // set screen output processor
                gLogger.set_screen_output_rank( 1 );

                // perform check
                check_results( "Laplace.exo.e-s.0000", 1 );

                // reset screen output processor
                gLogger.set_screen_output_rank( 0 );
            }
            break;
        }
        default:
        {
            MORIS_ERROR( false, "Example problem not configured for %d processors.", par_size() );
        }
    }
}

