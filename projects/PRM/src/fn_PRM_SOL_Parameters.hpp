/*
 * Copyright (c) 2022 University of Colorado
 * Licensed under the MIT license. See LICENSE.txt file in the MORIS root for details.
 *
 *------------------------------------------------------------------------------------
 *
 * fn_PRM_SOL_Parameters.hpp
 *
 */

#ifndef PROJECTS_PRM_SRC_FN_PRM_SOL_PARAMETERS_HPP_
#define PROJECTS_PRM_SRC_FN_PRM_SOL_PARAMETERS_HPP_

#include "cl_Param_List.hpp"

#include "cl_SOL_Enums.hpp"
#include "cl_NLA_Nonlinear_Solver_Enums.hpp"
#include "cl_TSA_Time_Solver_Enums.hpp"

namespace moris
{
    namespace prm
    {
        //------------------------------------------------------------------------------

        inline ParameterList
        create_solver_warehouse_parameterlist()
        {
            ParameterList tSolverWarehouseList;

            // TPL type. can be epetra or petsc
            tSolverWarehouseList.insert( "SOL_TPL_Type", (uint)( sol::MapType::Epetra ) );

            // save operator to matlab file
            tSolverWarehouseList.insert( "SOL_save_operator_to_matlab", std::string( "" ) );

            // save final solution vector to file
            tSolverWarehouseList.insert( "SOL_save_final_sol_vec_to_file", std::string( "" ) );

            // load final solution vector from file. using this option will skip the assembly and solve
            tSolverWarehouseList.insert( "SOL_load_sol_vec_from_file", std::string( "" ) );

            // HDF5 data group from which final solution vectors are loaded
            tSolverWarehouseList.insert( "SOL_load_sol_vec_data_group", std::string( "LHS" ) );

            // HDF5 data group from which final solution vectors are loaded
            tSolverWarehouseList.insert( "SOL_load_sol_vec_num_vec", (sint)1 );

            // save final adjoint vector to file
            tSolverWarehouseList.insert( "SOL_save_final_adjoint_vec_to_file", std::string( "" ) );

            // initial SOLUTION (i.e. initial condition / previous time step)
            tSolverWarehouseList.insert( "TSA_Initial_Sol_Vec", "" );

            // flag to save solution vectors of various time steps to hdf5 file
            tSolverWarehouseList.insert( "TSA_Save_Sol_Vecs_to_file", "" );

            return tSolverWarehouseList;
        }

        //------------------------------------------------------------------------------
        // P R E C O N I T I O N E R   P A R A M E T E R L I S T //

        inline void
        create_ifpack_preconditioner_parameterlist( ParameterList& aParameterlist )
        {
            // ASSIGN DEFAULT PARAMETER VALUES
            // Robust Algebraic Preconditioners using IFPACK 3.0, SAND REPORT, SAND2005-0662,
            //              https://trilinos.github.io/pdfs/IfpackUserGuide.pdf
            //              https://docs.trilinos.org/dev/packages/ifpack/doc/html/index.html

            // set ifpack preconditioner type
            // options are: point relaxation, block relaxation, ILU, ILUT, IC, ICT, Amesos, SPARSKIT, Krylov
            aParameterlist.insert( "ifpack_prec_type", "" );

            // overlap between processors
            aParameterlist.insert( "overlap-level", 1 );

            // Smoother parameters

            aParameterlist.insert( "relaxation: type", "Jacobi" );

            aParameterlist.insert( "relaxation: sweeps", 1 );

            aParameterlist.insert( "relaxation: damping factor", 1.0 );

            aParameterlist.insert( "relaxation: min diagonal value", 0.0 );

            aParameterlist.insert( "relaxation: zero starting solution", true );

            aParameterlist.insert( "relaxation: backward mode", false );

            aParameterlist.insert( "relaxation: use l1", false );

            aParameterlist.insert( "relaxation: l1 eta", 1.5 );

            // Partitioning parameters

            aParameterlist.insert( "partitioner: type", "greedy" );

            aParameterlist.insert( "partitioner: overlap", 0 );

            aParameterlist.insert( "partitioner: local parts", 1 );

            aParameterlist.insert( "partitioner: print level", 0 );

            aParameterlist.insert( "partitioner: use symmetric graph", true );

            // Direct solver parameters

            // Exact solve via direct solver: "Amesos_Lapack", "Amesos_Klu", "Amesos_Umfpack",
            //                                "Amesos_Superludist", "Amesos_Mumps", "Amesos_Dscpack", "Amesos_Pardiso"
            aParameterlist.insert( "amesos: solver type", "Amesos_Klu" );

            // Incomplete factorization parameters

            // fill level for ILU and IC
            aParameterlist.insert( "fact: level-of-fill", 1 );

            aParameterlist.insert( "fact: ilut level-of-fill", 1.0 );

            aParameterlist.insert( "fact: ict level-of-fill", 1.0 );

            // absolute threshold
            aParameterlist.insert( "fact: absolute threshold", 0.0 );

            // relative threshold (needs to be larger than zero)
            aParameterlist.insert( "fact: relative threshold", 1.0 );

            aParameterlist.insert( "fact: relax value", 0.0 );

            aParameterlist.insert( "fact: drop tolerance", 1e-9 );

            // Schwarz parameters
            aParameterlist.insert( "schwarz: combine mode", "Zero" );

            aParameterlist.insert( "schwarz: compute condest", true );

            aParameterlist.insert( "schwarz: filter singletons", false );

            aParameterlist.insert( "schwarz: reordering type", "rcm" );

            // Sparskit parameters
            aParameterlist.insert( "fact: sparskit: lfil", 0.0 );

            aParameterlist.insert( "fact: sparskit: tol", 0.0 );

            aParameterlist.insert( "fact: sparskit: droptol", 0.0 );

            aParameterlist.insert( "fact: sparskit: permtol", 0.0 );

            aParameterlist.insert( "fact: sparskit: alph", 0.0 );

            aParameterlist.insert( "fact: sparskit: mbloc", 0.0 );

            aParameterlist.insert( "fact: sparskit: type", "" );

            // Krylov parameters
            aParameterlist.insert( "krylov: iterations", 0 );

            aParameterlist.insert( "krylov: tolerance", 0.0 );

            aParameterlist.insert( "krylov: solver", 0 );

            aParameterlist.insert( "krylov: preconditioner", 0 );

            aParameterlist.insert( "krylov: number of sweeps", 0 );

            aParameterlist.insert( "krylov: block size", 0 );

            aParameterlist.insert( "krylov: damping parameter", 0.0 );

            aParameterlist.insert( "krylov: zero starting solution", 0 );

            // Reuse of preconditioner
            aParameterlist.insert( "prec_reuse", false );
        }

        //------------------------------------------------------------------------------

        inline void
        create_ml_preconditioner_parameterlist( ParameterList& aParameterlist )
        {
            // General Parameters

            // Default parameter settings; options are SA, NSSA, DD, DD-ML
            aParameterlist.insert( "ml_prec_type", "" );

            aParameterlist.insert( "PDE equations", 1 );
            aParameterlist.insert( "print unused", 0 );
            aParameterlist.insert( "ML output", 0 );
            aParameterlist.insert( "ML print initial list", -2 );
            aParameterlist.insert( "ML print final list", -2 );
            aParameterlist.insert( "eigen-analysis: type", "cg" );
            aParameterlist.insert( "eigen-analysis: iterations", 10 );

            // Multigrid Cycle Parameters
            aParameterlist.insert( "cycle applications", 1 );
            aParameterlist.insert( "max levels", 10 );
            aParameterlist.insert( "increasing or decreasing", "increasing" );
            aParameterlist.insert( "prec type", "MGV" );

            // Aggregation and Prolongator Parameters
            aParameterlist.insert( "aggregation: type", "Uncoupled" );
            aParameterlist.insert( "aggregation: threshold", 0.0 );
            aParameterlist.insert( "aggregation: damping factor", 1.3333333333333333 );
            aParameterlist.insert( "aggregation: smoothing sweeps", 1 );

            aParameterlist.insert( "aggregation: use tentative restriction", false );
            aParameterlist.insert( "aggregation: symmetrize", false );
            aParameterlist.insert( "aggregation: global aggregates", 1 );
            aParameterlist.insert( "aggregation: local aggregates", 1 );
            aParameterlist.insert( "aggregation: nodes per aggregate", 512 );

            aParameterlist.insert( "energy minimization: enable", false );
            aParameterlist.insert( "energy minimization: type", 2 );
            aParameterlist.insert( "energy minimization: droptol", 0.0 );
            aParameterlist.insert( "energy minimization: cheap", false );

            // Smoother Parameters
            aParameterlist.insert( "smoother: type", "Chebyshev" );
            aParameterlist.insert( "smoother: sweeps", 2 );
            aParameterlist.insert( "smoother: damping factor", 1.0 );
            aParameterlist.insert( "smoother: pre or post", "both" );
            aParameterlist.insert( "smoother: Aztec as solver", false );

            aParameterlist.insert( "smoother: ifpack level-of-fill", 0.0 );
            aParameterlist.insert( "smoother: ifpack overlap", 0 );
            aParameterlist.insert( "smoother: ifpack absolute threshold", 0.0 );
            aParameterlist.insert( "smoother: ifpack relative threshold", 0.0 );

            // Coarsest Grid Parameters
            aParameterlist.insert( "coarse: type", "Amesos-KLU" );
            aParameterlist.insert( "coarse: max size", 128 );
            aParameterlist.insert( "coarse: pre or post", "post" );
            aParameterlist.insert( "coarse: sweeps", 1 );
            aParameterlist.insert( "coarse: damping factor", 1.0 );

            // Load-Balancing Parameters
            aParameterlist.insert( "repartition: enable", 0 );
            aParameterlist.insert( "repartition: partitioner", "Zoltan" );

            // Analysis Parameters
            aParameterlist.insert( "analyze memory", false );
            aParameterlist.insert( "viz: enable", false );
            aParameterlist.insert( "viz: output format", "vtk" );
            aParameterlist.insert( "viz: print starting solution", false );

            // Smoothed Aggregation and the Null Space
            aParameterlist.insert( "null space: type", "default vectors" );
            aParameterlist.insert( "null space: dimension", 1 );
            // aParameterlist.insert( "null space: vectors" , NULL );
            aParameterlist.insert( "null space: vectors to compute", 2 );
            aParameterlist.insert( "null space: add default vectors", true );

            // Reuse of preconditioner
            aParameterlist.insert( "prec_reuse", false );
        }

        // //------------------------------------------------------------------------------

        inline ParameterList
        create_eigen_algorithm_parameter_list()
        {
            ParameterList mEigAlgoParameterList;

            enum moris::sol::SolverType tType = moris::sol::SolverType::EIGEN_SOLVER;

            mEigAlgoParameterList.insert( "Solver_Implementation", static_cast< uint >( tType ) );

            // ASSIGN DEFAULT PARAMETER VALUES
            // Anasazi User Manual: Chapter 13, SAND2004-2189, https://trilinos.github.io/pdfs/Trilinos10.12Tutorial.pdf

            // Determine which solver algorithm is used
            // options are: EIGALG_BLOCK_DAVIDSON, EIGALG_GENERALIZED_DAVIDSON, EIGALG_BLOCK_KRYLOV_SCHUR, EIGALG_BLOCK_KRYLOV_SCHUR_AMESOS
            mEigAlgoParameterList.insert( "Eigen_Algorithm", std::string( "" ) );

            // Verbosite flag, true or false
            mEigAlgoParameterList.insert( "Verbosity", false );

            // Which parameter sorts eigenvalues in increasing or decreasing order of magnitudes
            // options are: SM: Increasing Order ; SR: Increasing order of real part ; SI: Increasing order of imaginary part
            //              LM: Decreasing Order ; LR: Decreasing order of real part ; LI: Decreasing order of imaginary part
            mEigAlgoParameterList.insert( "Which", "SM" );

            // Sets Block size of integer type
            mEigAlgoParameterList.insert( "Block_Size", INT_MAX );

            // Sets Total DOFs of system of integer type
            mEigAlgoParameterList.insert( "NumFreeDofs", INT_MAX );

            // Request number of eigenvalues as integer type; Num_Eig_Vals = Block_Size
            mEigAlgoParameterList.insert( "Num_Eig_Vals", INT_MAX );

            // Number of blocks as integer type
            mEigAlgoParameterList.insert( "Num_Blocks", INT_MAX );

            // Maximum subspace dimensions; 3*Block_Size*Num_Eig_Vals
            mEigAlgoParameterList.insert( "MaxSubSpaceDims", INT_MAX );

            // Initial Guess required only for Block Krylov Schur Algorithm; integer type
            mEigAlgoParameterList.insert( "Initial_Guess", INT_MAX );

            // Sets maximum restart level as integer type
            mEigAlgoParameterList.insert( "MaxRestarts", INT_MAX );

            // sets convergence tolerance for given algorithm
            mEigAlgoParameterList.insert( "Convergence_Tolerance", 1e-013 );

            // sets relative convergence tolerance as bool type
            mEigAlgoParameterList.insert( "Relative_Convergence_Tolerance", true );

            // Update flag for vismesh
            mEigAlgoParameterList.insert( "Update_Flag", true );

            // add parameters from ifpack preconditioner
            create_ifpack_preconditioner_parameterlist( mEigAlgoParameterList );

            // add parameters from ml preconditioner
            create_ml_preconditioner_parameterlist( mEigAlgoParameterList );

            return mEigAlgoParameterList;
        }

        //------------------------------------------------------------------------------

        // creates a parameter list with default inputs
        inline ParameterList
        create_linear_algorithm_parameter_list_aztec()
        {
            ParameterList tLinAlgorithmParameterList;

            enum moris::sol::SolverType tType = moris::sol::SolverType::AZTEC_IMPL;

            tLinAlgorithmParameterList.insert( "Solver_Implementation", (uint)( tType ) );

            // ASSIGN DEFAULT PARAMETER VALUES
            // AztecOO User Guide, SAND REPORT, SAND2004-3796, https://trilinos.org/oldsite/packages/aztecoo/AztecOOUserGuide.pdf

            // Determine which solver is used
            // options are: AZ_gmres, AZ_gmres_condnum, AZ_cg, AZ_cg_condnum, AZ_cgs, AZ_tfqmr, AZ_bicgstab
            tLinAlgorithmParameterList.insert( "AZ_solver", INT_MAX );

            // Allowable Aztec solver iterations
            tLinAlgorithmParameterList.insert( "AZ_max_iter", INT_MAX );

            // Allowable Aztec iterative residual
            tLinAlgorithmParameterList.insert( "rel_residual", 1e-08 );

            // set Az_conv -convergence criteria
            // options are AZ_r0, AZ_rhs, AZ_Anorm, AZ_noscaled, AZ_sol
            tLinAlgorithmParameterList.insert( "AZ_conv", INT_MAX );

            // set Az_diagnostic parameters
            // Set whether or not diagnostics for every linear iteration are printed or not. options are AZ_all, AZ_none
            tLinAlgorithmParameterList.insert( "AZ_diagnostics", INT_MAX );

            // set AZ_output options
            // options are AZ_all, AZ_none, AZ_warnings, AZ_last, AZ_summary
            tLinAlgorithmParameterList.insert( "AZ_output", INT_MAX );

            // Determines the sub-matrices factored with the domain decomposition algorithms
            // Option to specify with how many rows from other processors each processor's local submatrix is augmented.
            tLinAlgorithmParameterList.insert( "AZ_overlap", INT_MAX );

            // Determines how overlapping subdomain results are combined when different processors have computed different values for the same unknown.
            // Options are AZ_standard, AZ_symmetric
            tLinAlgorithmParameterList.insert( "AZ_type_overlap", INT_MAX );

            // Determines whether RCM reordering will be done in conjunction with domain decomposition incomplete factorizations.
            // Option to enable (=1) or disable (=0) the Reverse Cuthill-McKee (RCM) algorithm to reorder system equations for smaller bandwidth
            tLinAlgorithmParameterList.insert( "AZ_reorder", INT_MAX );

            // Use preconditioner from a previous Iterate() call
            // Option are AZ_calc, AZ_recalc, AZ_reuse
            tLinAlgorithmParameterList.insert( "AZ_pre_calc", INT_MAX );

            // Determines  whether  matrix  factorization  information will be kept after this solve
            // for example for preconditioner_recalculation
            tLinAlgorithmParameterList.insert( "AZ_keep_info", INT_MAX );

            //--------------------------GMRES specific solver parameters---------------------------
            // Set AZ_kspace
            // Krylov subspace size for restarted GMRES
            // Setting mKrylovSpace larger improves the robustness, decreases iteration count, but increases memory consumption.
            // For very difficult problems, set it equal to the maximum number of iterations.
            tLinAlgorithmParameterList.insert( "AZ_kspace", INT_MAX );

            // Set AZ_orthog
            // AZ_classic or AZ_modified
            tLinAlgorithmParameterList.insert( "AZ_orthog", INT_MAX );

            // Set AZ_rthresh
            // Parameter used to modify the relative magnitude of the diagonal entries of the
            // matrix that is used to compute any of the incomplete factorization preconditioners
            tLinAlgorithmParameterList.insert( "AZ_rthresh", -1.0 );

            // Set AZ_athresh
            // Parameter used to modify the absolute magnitude of the diagonal entries of the
            // matrix that is used to compute any of the incomplete factorization preconditioners
            tLinAlgorithmParameterList.insert( "AZ_athresh", -1.0 );

            //--------------------------Preconditioner specific parameters--------------------------
            // Determine which preconditioner is used
            // Options are AZ_none, AZ_Jacobi, AZ_sym_GS, AZ_Neumann, AZ_ls, AZ_dom_decomp,
            tLinAlgorithmParameterList.insert( "AZ_precond", INT_MAX );

            // Set preconditioner subdomain solve - direct solve or incomplete
            // Options are AZ_lu, AZ_ilut, , AZ_rilu, AZ_bilu, AZ_icc
            tLinAlgorithmParameterList.insert( "AZ_subdomain_solve", INT_MAX );

            // Set preconditioner polynomial order - polynomial preconditioning, Gauss-Seidel, Jacobi
            tLinAlgorithmParameterList.insert( "AZ_poly_ord", INT_MAX );

            // Set drop tolerance - for LU, ILUT
            tLinAlgorithmParameterList.insert( "AZ_drop", -1.0 );

            // Set level of graph fill in - for ilu(k), icc(k), bilu(k)
            tLinAlgorithmParameterList.insert( "AZ_graph_fill", INT_MAX );

            // Set ilut fill
            tLinAlgorithmParameterList.insert( "AZ_ilut_fill", -1.0 );

            // Set Damping or relaxation parameter used for RILU
            tLinAlgorithmParameterList.insert( "AZ_omega", -1.0 );

            // add parameters from ifpack preconditioner
            create_ifpack_preconditioner_parameterlist( tLinAlgorithmParameterList );

            // add parameters from ml preconditioner
            create_ml_preconditioner_parameterlist( tLinAlgorithmParameterList );

            return tLinAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        // creates a parameter list with default inputs
        inline ParameterList
        create_linear_algorithm_parameter_list_amesos()
        {
            // ASSIGN DEFAULT PARAMETER VALUES
            // Amesos 2.0 Reference Guide, SANDIA REPORT, SAND2004-4820, https://trilinos.org/oldsite/packages/amesos/AmesosReferenceGuide.pdf

            ParameterList tLinAlgorithmParameterList;

            enum moris::sol::SolverType tType = moris::sol::SolverType::AMESOS_IMPL;

            tLinAlgorithmParameterList.insert( "Solver_Implementation", (uint)( tType ) );

#ifdef MORIS_USE_PARDISO
            tLinAlgorithmParameterList.insert( "Solver_Type", "Amesos_Pardiso" );
#else
            tLinAlgorithmParameterList.insert( "Solver_Type", "Amesos_Umfpack" );
#endif

            tLinAlgorithmParameterList.insert( "PrintStatus", false );

            tLinAlgorithmParameterList.insert( "PrintTiming", false );

            tLinAlgorithmParameterList.insert( "ComputeVectorNorms", false );

            tLinAlgorithmParameterList.insert( "ComputeTrueResidual", false );

            tLinAlgorithmParameterList.insert( "Reindex", false );

            tLinAlgorithmParameterList.insert( "Refactorize", false );

            tLinAlgorithmParameterList.insert( "AddZeroToDiag", false );

            tLinAlgorithmParameterList.insert( "RcondThreshold", -1.0 );

            tLinAlgorithmParameterList.insert( "OutputLevel", INT_MAX );

            tLinAlgorithmParameterList.insert( "DebugLevel", INT_MAX );

            tLinAlgorithmParameterList.insert( "ComputeConditionNumber", false );

            return tLinAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        // creates a parameter list with default inputs
        inline ParameterList
        create_linear_algorithm_parameter_list_belos()
        {
            ParameterList tLinAlgorithmParameterList;

            enum moris::sol::SolverType tType = moris::sol::SolverType::BELOS_IMPL;

            tLinAlgorithmParameterList.insert( "Solver_Implementation", (uint)( tType ) );

            // ASSIGN DEFAULT PARAMETER VALUES
            // https://docs.trilinos.org/dev/packages/belos/doc/html/classBelos_1_1SolverFactory.html#ad86e61fb180a73c6dd5dbf458df6a86f

            // Examples
            // https://docs.trilinos.org/dev/packages/belos/doc/html/examples.html

            // Determine which solver is used by string
            // options are: GMRES, Flexible GMRES, Block CG , PseudoBlockCG, Stochastic CG, Recycling GMRES, Recycling CG, MINRES, LSQR, TFQMR
            //              Pseudoblock TFQMR, Seed GMRES, Seed CG
            tLinAlgorithmParameterList.insert( "Solver Type", "GMRES" );

            // Level of output details
            // Belos::Errors + Belos::Warnings + Belos::TimingDetails + Belos::StatusTestDetails
            tLinAlgorithmParameterList.insert( "Verbosity", INT_MAX );

            // Maximum number of blocks in Krylov factorization
            tLinAlgorithmParameterList.insert( "Num Blocks", INT_MAX );

            // Block size to be used by iterative solver
            tLinAlgorithmParameterList.insert( "Block Size", INT_MAX );

            // Allowable Belos solver iterations
            tLinAlgorithmParameterList.insert( "Maximum Iterations", INT_MAX );

            // Allowable Belos solver iterations
            tLinAlgorithmParameterList.insert( "Maximum Restarts", INT_MAX );

            // set convergence criteria
            tLinAlgorithmParameterList.insert( "Convergence Tolerance", 1e-08 );

            // left or right preconditioner
            tLinAlgorithmParameterList.insert( "Left-right Preconditioner", "left" );

            // frequency of output
            tLinAlgorithmParameterList.insert( "Output Frequency", -1 );

            // add parameters from ifpack preconditioner
            create_ifpack_preconditioner_parameterlist( tLinAlgorithmParameterList );

            // add parameters from ml preconditioner
            create_ml_preconditioner_parameterlist( tLinAlgorithmParameterList );

            return tLinAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        // creates a parameter list with default inputs
        inline ParameterList
        create_linear_algorithm_parameter_list_petsc()
        {
            ParameterList tLinAlgorithmParameterList;

            enum moris::sol::SolverType tType = moris::sol::SolverType::PETSC;

            tLinAlgorithmParameterList.insert( "Solver_Implementation", (uint)( tType ) );

            // Set KSP type
            tLinAlgorithmParameterList.insert( "KSPType", std::string( "gmres" ) );

            // Set default preconditioner
            tLinAlgorithmParameterList.insert( "PCType", std::string( "ilu" ) );

            // Sets maximal iters for KSP
            tLinAlgorithmParameterList.insert( "KSPMaxits", 1000 );

            // Sets KSP gmres restart
            tLinAlgorithmParameterList.insert( "KSPMGMRESRestart", 500 );

            // Sets tolerance for determining happy breakdown in GMRES, FGMRES and LGMRES
            tLinAlgorithmParameterList.insert( "KSPGMRESHapTol", 1e-10 );

            // Sets tolerance for KSP
            tLinAlgorithmParameterList.insert( "KSPTol", 1e-10 );

            // Sets the number of levels of fill to use for ILU
            tLinAlgorithmParameterList.insert( "ILUFill", 0 );

            // Sets drop tolerance for ilu
            tLinAlgorithmParameterList.insert( "ILUTol", 1e-6 );

            // Set multigrid levels
            tLinAlgorithmParameterList.insert( "MultigridLevels", 3 );

            // Schwarz preconditioner volume fraction threshold
            tLinAlgorithmParameterList.insert( "MG_use_schwarz_smoother", false );

            // Schwarz smoothing iterations
            tLinAlgorithmParameterList.insert( "MG_schwarz_smoothing_iters", 1 );

            // Schwarz preconditioner volume fraction threshold
            tLinAlgorithmParameterList.insert( "ASM_volume_fraction_threshold", 0.1 );

            // number of eigen values to be outputted
            tLinAlgorithmParameterList.insert( "ouput_eigenspectrum", (uint)0 );

            // blocks in addtive Schwartz algorthim
            tLinAlgorithmParameterList.insert( "ASM_blocks_output_filename", "" );

            return tLinAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        inline ParameterList
        create_linear_solver_parameter_list()
        {
            ParameterList tLinSolverParameterList;

            tLinSolverParameterList.insert( "DLA_Linear_solver_algorithms", "0" );

            // Maximal number of linear solver restarts on fail
            tLinSolverParameterList.insert( "DLA_max_lin_solver_restarts", 0 );

            // Maximal number of linear solver restarts on fail
            tLinSolverParameterList.insert( "DLA_hard_break", true );

            // Determines if linear solve should restart on fail
            tLinSolverParameterList.insert( "DLA_rebuild_lin_solver_on_fail", false );

            // output left hand side in linear solver, if specified by user
            tLinSolverParameterList.insert( "DLA_LHS_output_filename", "" );

            // Flag for RHS Matrix Type if Eigen Solver is set to true
            tLinSolverParameterList.insert( "RHS_Matrix_Type", std::string( "" ) );

            return tLinSolverParameterList;
        }

        //------------------------------------------------------------------------------

        inline ParameterList
        create_nonlinear_algorithm_parameter_list()
        {
            ParameterList tNonLinAlgorithmParameterList;

            enum moris::NLA::NonlinearSolverType NonlinearSolverType = moris::NLA::NonlinearSolverType::NEWTON_SOLVER;

            tNonLinAlgorithmParameterList.insert( "NLA_Solver_Implementation", (uint)( NonlinearSolverType ) );

            tNonLinAlgorithmParameterList.insert( "NLA_Linear_solver", 0 );

            tNonLinAlgorithmParameterList.insert( "NLA_linear_solver_for_adjoint_solve", -1 );

            // Allowable Newton solver iterations
            tNonLinAlgorithmParameterList.insert( "NLA_max_iter", 10 );

            // Allowable Newton solver iterations
            tNonLinAlgorithmParameterList.insert( "NLA_restart", 0 );

            // Desired total residual norm drop
            tNonLinAlgorithmParameterList.insert( "NLA_rel_res_norm_drop", 1e-08 );

            // Desired total residual norm
            tNonLinAlgorithmParameterList.insert( "NLA_tot_res_norm", 1e-12 );

            // Maximal residual norm
            tNonLinAlgorithmParameterList.insert( "NLA_max_rel_res_norm", 1e12 );

            // Maximal number of linear solver restarts on fail
            tNonLinAlgorithmParameterList.insert( "NLA_max_lin_solver_restarts", 0 );

            // Relaxation strategy
            tNonLinAlgorithmParameterList.insert( "NLA_relaxation_strategy", (uint)( sol::SolverRelaxationType::Constant ) );

            // Relaxation parameter
            tNonLinAlgorithmParameterList.insert( "NLA_relaxation_parameter", 1.0 );

            // Relaxation parameter
            tNonLinAlgorithmParameterList.insert( "NLA_relaxation_damping", 0.5 );

            // Load control strategy
            tNonLinAlgorithmParameterList.insert( "NLA_load_control_strategy", (uint)( sol::SolverLoadControlType::Constant ) );

            // Initial load factor
            tNonLinAlgorithmParameterList.insert( "NLA_load_control_factor", 1.0 );

            // Maximum number of load steps
            tNonLinAlgorithmParameterList.insert( "NLA_load_control_steps", 1 );

            // Required relative residual norm for load factor to be increased
            tNonLinAlgorithmParameterList.insert( "NLA_load_control_relres", 0.0 );

            // Exponent for exponential load factor growth strategy
            tNonLinAlgorithmParameterList.insert( "NLA_load_control_exponent", 1.0 );

            // Pseudo time control strategy
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_control_strategy", (uint)( sol::SolverPseudoTimeControlType::None ) );

            // Constant time step size
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_constant", 0.0 );

            // Pre-factor for time step index-based increase of time step
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_step_index_factor", 0.0 );

            // Exponent for time step index-based increase
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_step_index_exponent", 1.0 );

            // Pre-factor for residual-based increase of time step
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_residual_factor", 0.0 );

            // Exponent for time step index-based increase
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_residual_exponent", 1.0 );

            // Comsol parameter (laminar: 20, 2D turbulent: 25, 3D turbulent: 30)
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_comsol_1", 20.0 );

            // Comsol parameter (laminar: 40, 2D turbulent: 50, 3D turbulent: 60)
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_comsol_2", 40.0 );

            // Maximum number of pseudo time step size
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_max_num_steps", 1 );

            // Maximum time step size
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_max_step_size", MORIS_REAL_MAX );

            // Required pseudo time step size needed for convergence
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_rel_res_norm_drop", -1.0 );

            // Required relative residual norm for time step to be updated
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_rel_res_norm_update", -1.0 );

            // Relative static residual norm for switching to steady state computation
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_steady_rel_res_norm", -1.0 );

            // Time step size used once maximum number of step has been reached
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_steady_step_size", MORIS_REAL_MAX );

            // Time offsets for outputting pseudo time steps; if offset is zero no output is written
            tNonLinAlgorithmParameterList.insert( "NLA_pseudo_time_offset", 0.0 );

            // Maximal number of linear solver restarts on fail
            tNonLinAlgorithmParameterList.insert( "NLA_hard_break", false );

            // Determines if linear solve should restart on fail
            tNonLinAlgorithmParameterList.insert( "NLA_rebuild_lin_solv_on_fail", false );

            // Determines if jacobian is rebuild for every nonlinear iteration
            tNonLinAlgorithmParameterList.insert( "NLA_rebuild_jacobian", true );

            // Determines if linear solve should restart on fail
            tNonLinAlgorithmParameterList.insert( "NLA_combined_res_jac_assembly", true );

            // Determines if Newton should restart on fail
            tNonLinAlgorithmParameterList.insert( "NLA_rebuild_nonlin_solv_on_fail", false );

            // Specifying the number of Newton retries
            tNonLinAlgorithmParameterList.insert( "NLA_num_nonlin_rebuild_iterations", 1 );

            // Determines relaxation multiplier
            tNonLinAlgorithmParameterList.insert( "NLA_relaxation_multiplier_on_fail", 0.5 );

            // Determines Newton maxits multiplier
            tNonLinAlgorithmParameterList.insert( "NLA_maxits_multiplier_on_fail", 2 );

            return tNonLinAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        inline ParameterList
        create_nonlinear_solver_parameter_list()
        {
            ParameterList tNonLinSolverParameterList;

            enum moris::NLA::NonlinearSolverType NonlinearSolverType = moris::NLA::NonlinearSolverType::NEWTON_SOLVER;

            tNonLinSolverParameterList.insert( "NLA_Solver_Implementation", (uint)( NonlinearSolverType ) );

            tNonLinSolverParameterList.insert( "NLA_DofTypes", "UNDEFINED" );

            tNonLinSolverParameterList.insert( "NLA_Secondary_DofTypes", "UNDEFINED" );

            tNonLinSolverParameterList.insert( "NLA_Sub_Nonlinear_Solver", "" );

            tNonLinSolverParameterList.insert( "NLA_Nonlinear_solver_algorithms", "0" );

            // Maximal number of linear solver restarts on fail
            tNonLinSolverParameterList.insert( "NLA_max_non_lin_solver_restarts", 0 );

            return tNonLinSolverParameterList;
        }

        //------------------------------------------------------------------------------

        inline ParameterList
        create_time_solver_algorithm_parameter_list()
        {
            ParameterList tTimeAlgorithmParameterList;

            enum moris::tsa::TimeSolverType tType = tsa::TimeSolverType::MONOLITHIC;

            tTimeAlgorithmParameterList.insert( "TSA_Solver_Implementation", (uint)( tType ) );

            tTimeAlgorithmParameterList.insert( "TSA_Nonlinear_solver", 0 );

            tTimeAlgorithmParameterList.insert( "TSA_nonlinear_solver_for_adjoint_solve", -1 );

            // Number of time steps
            tTimeAlgorithmParameterList.insert( "TSA_Num_Time_Steps", 1 );

            // Time Frame
            tTimeAlgorithmParameterList.insert( "TSA_Time_Frame", 1.0 );

            return tTimeAlgorithmParameterList;
        }

        //------------------------------------------------------------------------------

        inline ParameterList
        create_time_solver_parameter_list()
        {
            ParameterList tTimeParameterList;

            tTimeParameterList.insert( "TSA_TPL_Type", (uint)( sol::MapType::Epetra ) );

            tTimeParameterList.insert( "TSA_Solver_algorithms", "0" );

            tTimeParameterList.insert( "TSA_DofTypes", "UNDEFINED" );

            // Maximal number of linear solver restarts on fail
            tTimeParameterList.insert( "TSA_Max_Time_Solver_Restarts", 0 );

            tTimeParameterList.insert( "TSA_Output_Indices", "0" );

            tTimeParameterList.insert( "TSA_Output_Criteria", "Default_Output_Criterion" );

            tTimeParameterList.insert( "TSA_Initialize_Sol_Vec", "" );    // initial GUESS

            tTimeParameterList.insert( "TSA_time_level_per_type", "" );

            return tTimeParameterList;
        }

        //------------------------------------------------------------------------------

        // creates a parameter list with default inputs
        inline ParameterList
        create_linear_algorithm_parameter_list(
                const enum moris::sol::SolverType aSolverType,
                const uint                        aIndex = 0 )
        {
            ParameterList tParameterList;

            switch ( aSolverType )
            {
                case sol::SolverType::AZTEC_IMPL:
                    return create_linear_algorithm_parameter_list_aztec();
                    break;
                case sol::SolverType::AMESOS_IMPL:
                    return create_linear_algorithm_parameter_list_amesos();
                    break;
                case sol::SolverType::BELOS_IMPL:
                    return create_linear_algorithm_parameter_list_belos();
                    break;
                case sol::SolverType::PETSC:
                    return create_linear_algorithm_parameter_list_petsc();
                    break;
                case sol::SolverType::EIGEN_SOLVER:
                    return create_eigen_algorithm_parameter_list();
                    break;
                default:
                    MORIS_ERROR( false, "Parameter list for this solver not implemented yet" );
                    break;
            }
            return tParameterList;
        }

        //------------------------------------------------------------------------------

        // inline
        //    // creates a parameter list with default inputs
        //    ParameterList create_nonlinear_algorithm_parameter_list( const enum moris::NLA::NonlinearSolverType aSolverType,
        //                                                             const uint                        aIndex = 0 )
        //    {
        //        ParameterList tParameterList;
        //
        //        switch( aSolverType )
        //        {
        //        case ( moris::NLA::NonlinearSolverType::NEWTON_SOLVER ):
        //            return create_linear_algorithm_parameter_list_aztec( );
        //            break;
        //        case ( moris::NLA::NonlinearSolverType::NLBGS_SOLVER ):
        //    		MORIS_ERROR( false, "No implemented yet" );
        //            break;
        //
        //        default:
        //            MORIS_ERROR( false, "Parameterlist for this solver not implemented yet" );
        //            break;
        //        }
        //
        //    return tParameterList;
        //}

        //------------------------------------------------------------------------------

    }    // namespace prm
}    // namespace moris

#endif    // PROJECTS_PRM_SRC_FN_PRM_SOL_PARAMETERS_HPP_
