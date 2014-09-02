/**
 * @file simplexparameterhandler.cpp
 */

#include <simplex/simplexparameterhandler.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>
#include <initpanopt.h>

thread_local const char * SimplexParameterHandler::sm_defaultFilename = "simplex.PAR";

SimplexParameterHandler * SimplexParameterHandler::sm_instance = 0;

SimplexParameterHandler::SimplexParameterHandler()
{
    m_filename = sm_defaultFilename;
}

ParameterHandler& SimplexParameterHandler::getInstance()
{
    //std::cout << "SIMPLEX PARAMETER LOAD" << std::endl;
    try{
        return *sm_instance;
        /*static SimplexParameterHandler * s_instance = 0;
        THREAD_STATIC_DECL bool s_init = false;
        if(!s_init) {

            //InitPanOpt::getInstance().init();

            s_instance = new SimplexParameterHandler;
            s_instance->initParameters();
            s_instance->readParameterFile(sm_defaultFilename);
            s_init = true;
        }
        return *s_instance;*/
    } catch (const ParameterException & exception) {
        LPERROR( "Simplex ParameterException: " <<exception.getMessage() );
        exit(-1);
    }
}

void SimplexParameterHandler::_globalInit() {
    sm_instance = new SimplexParameterHandler;
    sm_instance->initParameters();
    sm_instance->readParameterFile(sm_defaultFilename);
}

void SimplexParameterHandler::writeParameterFile(){
    try {
        std::ofstream out;
        out.open(m_filename, std::ios::out | std::ios::binary);

        if (!out.is_open()) throw -1;

        out << "!!! Simplex parameter file for the Pannon Optimizer !!! \n";
        out << "! If this file is present, the values of the given parameters can be changed. \n";

        out << std::scientific << "\n";

        out << "!!! Tolerances !!! \n \n";

        out << "! Pivot tolerance for SSX iterations \n";
        out << "\t" << "e_pivot = " <<writeParameter("e_pivot") << "\n";

        out << "! Feasibility tolerance \n"
               "\t" << "e_feasibility = " << writeParameter("e_feasibility") << "\n";

        out << "! Optimality tolerance \n"
               "\t" << "e_optimality = " << writeParameter("e_optimality") << "\n";

        out << std::fixed << "\n";

        out << "!!! Starting procedures !!! \n\n";

        out << "! Presolve \n"
               "\t" << "presolve = " << writeParameter("presolve") << "\n";

        out << "! Benichou scaling with powers of 2 \n"
               "\t" << "scaling = " << writeParameter("scaling") << "\n";

        out << "! Starting procedures \n"
               "! 0 = Non-basic variables at lower bound \n"
               "! 1 = Non-basic variables at upper bound \n"
               "! 2 = Mixed non-basic variable bounds \n"
               "\t" << "starting_nonbasic_states = " << writeParameter("starting_nonbasic_states") << "\n";

        out << "! Starting basis strategies \n"
               "! 0 = Logical basis \n"
               "! 1 = CRASH basis \n"
               "\t" << "starting_basis_strategy = " << writeParameter("starting_basis_strategy") << "\n";

        out << "\n";

        out << "!!! Basis factorization !!! \n\n";
        out << "! Type of basis factorization \n"
               "! 0 = PFI \n"
               "! 1 = LU factorization \n"
               "\t" << "factorization_type = " << writeParameter("factorization_type") << "\n";

        out << "! PFI specific parameters ! \n";
        out << "! The method used to process the non-triangular kernel \n"
               "! 0 = Simple search for potential pivot positions \n"
               "! 1 = Create a block triangular form and pivot down the diagonal \n"
               "! 2 = Create a block triangular form and order the blocks by column counts before pivoting \n"
               "\t" << "nontriangular_method = " << writeParameter("nontriangular_method") << "\n";

        out << "! Pivot rules for the non-triangular part \n"
               "! 0 = Simple pivot tolerance is used \n"
               "! 1 = Threshold pivoting \n"
               "\t" << "nontriangular_pivot_rule = " << writeParameter("nontriangular_pivot_rule") << "\n";

        out << "! Frequency of reinversions in # of iterations \n"
               "\t" << "reinversion_frequency = " << writeParameter("reinversion_frequency") << "\n";

        out << " ! Constant value used for threshold pivoting \n"
               "\t" << "pivot_threshold = " << writeParameter("pivot_threshold") <<  "\n";

        out << "\n";

        out << "!!! Pricing !!! \n\n";
        out << "! Pricing type \n"
               "! 0 = Dantzig rule \n"
               "! 1 = Devex rule \n"
               "\t" << "pricing_type = " << writeParameter("pricing_type") << "\n";

        out << "! Parameters of the SIMPRI partial pricing framework \n"
               "! The range of vectors are divided into clusters during the pricing, which are \n"
               "! scanned as a round robin in order to find improving candidates. \n"
               "! With partial pricing it can be specified that only a certain number \n"
               "! of clusters are visited to find improving candidates. If no candidates \n"
               "! found, more clusters are involved until necessary. \n"
               "! It is also possible to stop the cluster visit if enough candidates are found. \n"
               "! Parameter valus 0 for improving candidates means this stop condition is turned off. \n"
               "! All the three parameters can be specified both for phase I and phase II. \n"
               "\t" << "simpri_phaseI_clusters = " << writeParameter("simpri_phaseI_clusters") << "\n"
               "\t" << "simpri_phaseI_visit_clusters = " << writeParameter("simpri_phaseI_visit_clusters") << "\n"
               "\t" << "simpri_phaseI_improving_candidates = " << writeParameter("simpri_phaseI_improving_candidates") << "\n"
               "\t" << "simpri_phaseII_clusters = " << writeParameter("simpri_phaseII_clusters") << "\n"
               "\t" << "simpri_phaseII_visit_clusters = " << writeParameter("simpri_phaseII_visit_clusters") << "\n"
               "\t" << "simpri_phaseII_improving_candidates = " << writeParameter("simpri_phaseII_improving_candidates") << "\n";


        out << "\n";

        out << "!!! Ratiotest !!!\n\n";
        out << "! Use the piecewise linear concave function in primal phase I. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_primal_phaseI_function = " << writeParameter("nonlinear_primal_phaseI_function") << "\n";

        out << "! Use the piecewise linear concave function in dual phase I. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_dual_phaseI_function = " << writeParameter("nonlinear_dual_phaseI_function") << "\n";

        out << "! Use the piecewise linear concave function in dual phase II. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_dual_phaseII_function = " << writeParameter("nonlinear_dual_phaseII_function") << "\n";

        out << "! Enable fake feasibility handling in Dual Phase I and II \n"
               "\t! 0 = disabled \n"
               "\t! 1 = enabled \n"
               "\t" << "enable_fake_feasibility = " << writeParameter("enable_fake_feasibility") << "\n";

        out << "! EXPAND multiplier initializing value \n"
               "\t" << "expand_multiplier = " << writeParameter("expand_multiplier") << "\n";

        out << "! EXPAND divider initializing value \n"
               "\t" << "expand_divider = " << writeParameter("expand_divider") << "\n";

        out << "! EXPAND procedure in Dual Phase I \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Harris ratiotest \n"
               "\t! 2 = Expanding tolerance \n"
               "\t" << "expand_dual_phaseI = " << writeParameter("expand_dual_phaseI") << "\n";

        out << "! EXPAND procedure in Dual Phase II \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Harris ratiotest \n"
               "\t! 2 = Expanding tolerance \n"
               "\t" << "expand_dual_phaseII = " << writeParameter("expand_dual_phaseII") << "\n";

        out << "! Cost vector perturbation \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Active for structural variables \n"
               "\t! 2 = Active for structural and logical variables \n"
               "\t" << "perturb_cost_vector = " << writeParameter("perturb_cost_vector") << "\n";

        out << "! Measure of perturbation \n"
               "\t" << "epsilon_cost_vector = " << writeParameter("epsilon_cost_vector") << "\n";

        out << "! RHS perturbation \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Active \n"
               "\t" << "perturb_rhs = " << writeParameter("perturb_rhs") << "\n";

        out << "! Measure of perturbation \n"
               "\t" << "epsilon_rhs = " << writeParameter("epsilon_rhs") << "\n";

        out << "! Bound shifting in primal \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Active \n"
               "\t" << "shift_bounds = " << writeParameter("shift_bounds") << "\n";

        out << "! Measure of shifting \n"
               "\t" << "epsilon_bounds = " << writeParameter("epsilon_bounds") << "\n";

        out << "\n";

        out << "!!! Global !!! \n\n";

        out << "! Starting algorithm of the simplex method \n"
               "! 0 = primal simplex \n"
               "! 1 = dual simplex \n"
               "\t " << "starting_algorithm = " << writeParameter("starting_algorithm") << "\n";

        out << "! Algorithm switching switching during the iterations \n"
               "! 0 = inactive \n"
               "! 1 = switch algorithm before each inversion \n"
               "! 2 = switch algorithm immediately when entering phase-2 \n"
               "! 3 = switch algorithm before each inversion in phase-2 \n"
               "! 4 = switch algorithm if the actual (phase-1 of phase-2) objective value is the same between two inversions \n"
               "\t " << "switch_algorithm = " << writeParameter("switch_algorithm") << "\n";

        out << "! Level of iteration report \n"
               "! 0 = brief problem report    & solution only                   & only solution time \n"
               "! 1 = brief problem report    & one line per inversion          & only solution time \n"
               "! 2 = detailed problem report & one line per iteration          & brief time report \n"
               "! 3 = detailed problem report & one detailed line per iteration & detailed time report \n"
               "\t" << "debug_level = " << writeParameter("debug_level") << "\n";

        out << "! Maximal # of iterations \n"
               "\t" << "iteration_limit = " << writeParameter("iteration_limit") << "\n";

        out << "! Time limit for a problem (sec) \n"
               "\t" << "time_limit = " << writeParameter("time_limit") << "\n";

        out << "! Previously specified states of the solution algorithm can be saved. \n"
               "! After a filename is specified, there are four options. \n"
               "! The file format can be BAS (standard format) or PBF (PanOpt Basis Format) . \n"
               "! The last basis can be marked to be saved as `basis_filename`_last.`format` . \n"
               "! A basis of a given iteration can be marked to be saved or basis can be saved periodically \n"
               "! each output will follow the following syntax: `basis_filename`_`iteration_number`.`format` . \n"
               "! Both parameters are integers, 0 means saving is turned off. \n"
               "\t" << "save_basis = " << writeParameter("save_basis") << "\n"
               "\t" << "save_filename = " << writeParameter("save_filename") << "\n"
               "\t" << "save_format = " << writeParameter("save_format") << "\n"
               "\t" << "save_last_basis = " << writeParameter("save_last_basis") << "\n"
               "\t" << "save_iteration = " << writeParameter("save_iteration") << "\n"
               "\t" << "save_periodically = " << writeParameter("save_periodically") << "\n";

        out << "! The solver can start from a given basis. \n"
               "! The file format can be BAS (standard format) or PBF (PanOpt Basis Format). \n"
               "\t" << "load_basis = " << writeParameter("load_basis") << "\n"
               "\t" << "load_filename = " << writeParameter("load_filename") << "\n"
               "\t" << "load_format = " << writeParameter("load_format") << "\n";

        out << "! The solver can export specific data in CSV format for research purposes.\n"
               "! The export type specifies the set of output to be written.\n"
               "! The export should be clean, it is appended with one line for each problem solved\n"
               "! The available export types are the following: \n"
               "! 0 = Parameter study (reporting numerical problems, tolerances and the solution) \n"
               "! 1 = Ratiotest study (reporting measures of the ratio tests) \n"
               "\t" << "enable_export = " << writeParameter("enable_export") << "\n"
               "\t" << "export_filename = " << writeParameter("export_filename") << "\n"
               "\t" << "export_type = " << writeParameter("export_type") << "\n";


        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " << m_filename;
    }
}

void SimplexParameterHandler::initParameters()
{
    //Tolerances
    createParameter("e_pivot", Entry::DOUBLE);
    setParameterValue("e_pivot", DefaultParameters::E_PIVOT);
    createParameter("e_feasibility", Entry::DOUBLE);
    setParameterValue("e_feasibility", DefaultParameters::E_FEASIBILITY);
    createParameter("e_optimality", Entry::DOUBLE);
    setParameterValue("e_optimality", DefaultParameters::E_OPTIMALITY);

    //Starting procedures
    createParameter("presolve", Entry::BOOL);
    setParameterValue("presolve", DefaultParameters::PRESOLVE);
    createParameter("scaling", Entry::INTEGER);
    setParameterValue("scaling", DefaultParameters::SCALING);
    createParameter("starting_nonbasic_states", Entry::INTEGER);
    setParameterValue("starting_nonbasic_states", DefaultParameters::STARTING_NONBASIC_STATES);
    createParameter("starting_basis_strategy", Entry::INTEGER);
    setParameterValue("starting_basis_strategy", DefaultParameters::STARTING_BASIS_STRATEGY);

    //Basis factorization
    createParameter("factorization_type", Entry::INTEGER);
    setParameterValue("factorization_type", DefaultParameters::FACTORIZATION_TYPE);
    createParameter("nontriangular_method", Entry::INTEGER);
    setParameterValue("nontriangular_method", DefaultParameters::NONTRIANGULAR_METHOD);
    createParameter("nontriangular_pivot_rule", Entry::INTEGER);
    setParameterValue("nontriangular_pivot_rule", DefaultParameters::NONTRIANGULAR_PIVOT_RULE);
    createParameter("reinversion_frequency", Entry::INTEGER);
    setParameterValue("reinversion_frequency", DefaultParameters::REINVERSION_FREQUENCY);
    createParameter("pivot_threshold", Entry::DOUBLE);
    setParameterValue("pivot_threshold", DefaultParameters::PIVOT_THRESHOLD);

    //Pricing
    createParameter("pricing_type", Entry::INTEGER);
    setParameterValue("pricing_type", DefaultParameters::PRICING_TYPE);
    createParameter("simpri_phaseI_clusters", Entry::INTEGER);
    setParameterValue("simpri_phaseI_clusters", DefaultParameters::SIMPRI_PHASEI_CLUSTERS);
    createParameter("simpri_phaseI_visit_clusters", Entry::INTEGER);
    setParameterValue("simpri_phaseI_visit_clusters", DefaultParameters::SIMPRI_PHASEI_VISIT_CLUSTERS);
    createParameter("simpri_phaseI_improving_candidates", Entry::INTEGER);
    setParameterValue("simpri_phaseI_improving_candidates", DefaultParameters::SIMPRI_PHASEI_IMPROVING_CANDIDATES);
    createParameter("simpri_phaseII_clusters", Entry::INTEGER);
    setParameterValue("simpri_phaseII_clusters", DefaultParameters::SIMPRI_PHASEII_CLUSTERS);
    createParameter("simpri_phaseII_visit_clusters", Entry::INTEGER);
    setParameterValue("simpri_phaseII_visit_clusters", DefaultParameters::SIMPRI_PHASEII_VISIT_CLUSTERS);
    createParameter("simpri_phaseII_improving_candidates", Entry::INTEGER);
    setParameterValue("simpri_phaseII_improving_candidates", DefaultParameters::SIMPRI_PHASEII_IMPROVING_CANDIDATES);

    //Ratiotest
    createParameter("nonlinear_primal_phaseI_function", Entry::INTEGER);
    setParameterValue("nonlinear_primal_phaseI_function", DefaultParameters::NONLINEAR_PRIMAL_PHASEI_FUNCTION);
    createParameter("nonlinear_dual_phaseI_function", Entry::INTEGER);
    setParameterValue("nonlinear_dual_phaseI_function", DefaultParameters::NONLINEAR_DUAL_PHASEI_FUNCTION);
    createParameter("nonlinear_dual_phaseII_function", Entry::INTEGER);
    setParameterValue("nonlinear_dual_phaseII_function", DefaultParameters::NONLINEAR_DUAL_PHASEII_FUNCTION);
    createParameter("enable_fake_feasibility", Entry::INTEGER);
    setParameterValue("enable_fake_feasibility", DefaultParameters::ENABLE_FAKE_FEASIBILITY);

    createParameter("expand_multiplier", Entry::DOUBLE);
    setParameterValue("expand_multiplier", DefaultParameters::EXPAND_MULTIPLIER);
    createParameter("expand_divider", Entry::INTEGER);
    setParameterValue("expand_divider", DefaultParameters::EXPAND_DIVIDER);
    createParameter("expand_dual_phaseI", Entry::INTEGER);
    setParameterValue("expand_dual_phaseI", DefaultParameters::EXPAND_DUAL_PHASEI);
    createParameter("expand_dual_phaseII", Entry::INTEGER);
    setParameterValue("expand_dual_phaseII", DefaultParameters::EXPAND_DUAL_PHASEII);

    //Perturbation
    createParameter("perturb_cost_vector",Entry::INTEGER);
    setParameterValue("perturb_cost_vector",DefaultParameters::PERTURB_COST_VECTOR);
    createParameter("epsilon_cost_vector",Entry::DOUBLE);
    setParameterValue("epsilon_cost_vector",DefaultParameters::EPSILON_COST_VECTOR);
    createParameter("perturb_rhs",Entry::INTEGER);
    setParameterValue("perturb_rhs",DefaultParameters::PERTURB_RHS);
    createParameter("epsilon_rhs",Entry::DOUBLE);
    setParameterValue("epsilon_rhs",DefaultParameters::EPSILON_RHS);
    createParameter("shift_bounds",Entry::INTEGER);
    setParameterValue("shift_bounds",DefaultParameters::SHIFT_BOUNDS);
    createParameter("epsilon_bounds",Entry::DOUBLE);
    setParameterValue("epsilon_bounds",DefaultParameters::EPSILON_BOUNDS);

    //Global
    createParameter("starting_algorithm",Entry::INTEGER);
    setParameterValue("starting_algorithm",DefaultParameters::STARTING_ALGORITHM);
    createParameter("switch_algorithm",Entry::INTEGER);
    setParameterValue("switch_algorithm",DefaultParameters::SWITCH_ALGORITHM);

    createParameter("debug_level", Entry::INTEGER);
    setParameterValue("debug_level", DefaultParameters::DEBUG_LEVEL);
    createParameter("iteration_limit", Entry::INTEGER);
    setParameterValue("iteration_limit", DefaultParameters::ITERATION_LIMIT);
    createParameter("time_limit", Entry::DOUBLE);
    setParameterValue("time_limit", DefaultParameters::TIME_LIMIT);

    createParameter("save_basis", Entry::BOOL);
    setParameterValue("save_basis", DefaultParameters::SAVE_BASIS);
    createParameter("save_filename", Entry::STRING);
    setParameterValue("save_filename", std::string(DefaultParameters::SAVE_FILENAME));
    createParameter("save_format", Entry::STRING);
    setParameterValue("save_format", std::string(DefaultParameters::SAVE_FORMAT));
    createParameter("save_last_basis", Entry::BOOL);
    setParameterValue("save_last_basis", DefaultParameters::SAVE_LAST_BASIS);
    createParameter("save_iteration", Entry::INTEGER);
    setParameterValue("save_iteration", DefaultParameters::SAVE_ITERATION);
    createParameter("save_periodically", Entry::INTEGER);
    setParameterValue("save_periodically", DefaultParameters::SAVE_PERIODICALLY);

    createParameter("load_basis", Entry::BOOL);
    setParameterValue("load_basis", DefaultParameters::LOAD_BASIS);
    createParameter("load_filename", Entry::STRING);
    setParameterValue("load_filename", std::string(DefaultParameters::LOAD_FILENAME));
    createParameter("load_format", Entry::STRING);
    setParameterValue("load_format", std::string(DefaultParameters::LOAD_FORMAT));

    createParameter("enable_export", Entry::BOOL);
    setParameterValue("enable_export", DefaultParameters::ENABLE_EXPORT);
    createParameter("export_filename", Entry::STRING);
    setParameterValue("export_filename", std::string(DefaultParameters::EXPORT_FILENAME));
    createParameter("export_type", Entry::INTEGER);
    setParameterValue("export_type", DefaultParameters::EXPORT_TYPE);
}
