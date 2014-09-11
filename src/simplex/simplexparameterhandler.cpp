/**
 * @file simplexparameterhandler.cpp
 */

#include <simplex/simplexparameterhandler.h>
#include <simplex/simplexparametercomments.h>

#include <fstream>
#include <iostream>
#include <thread>

#include <defaultparameters.h>
#include <initpanopt.h>

thread_local const char * SimplexParameterHandler::sm_defaultFilename = "simplex.PAR";

SimplexParameterHandler * SimplexParameterHandler::sm_instance = 0;

class Node {
public:
    void addNewParameter(const Parameter & param) {
        std::string name = param.getName();
        std::size_t dotIndex = name.find_first_of(".");
        if (dotIndex == std::string::npos) {
            LPINFO("new parameter: " << name);
            m_values.push_back(param);
            return;
        }
        // block
        std::string blockName = name.substr(0, dotIndex);
        LPINFO("block name: " << blockName);



        Parameter newParameter = param;
        std::string subName = name.substr(dotIndex + 1);
        std::size_t nextDotIndex = subName.find_first_of(".");


        /*if (nextDotIndex == std::string::npos) {
            // next is a value
            newParameter.setName(subName);
            //while (nodeIter != nodeIterEnd && nodeIter)
            Node newNode;
            newNode.m_name = blockName;
            newNode.addNewParameter(newParameter);
            m_nodes.push_back(newNode);

        } else {*/
        //std::string subNodeName = subName.substr( 0, nextDotIndex );
        //LPINFO("\tsubname: " << subName << "   " << subNodeName);
        LPINFO("\tsubname: " << subName);
        newParameter.setName(subName);
        auto nodeIter = m_nodes.begin();
        auto nodeIterEnd = m_nodes.end();

        while (nodeIter != nodeIterEnd && nodeIter->m_name != blockName) {
            nodeIter++;
        }
        if (nodeIter == nodeIterEnd) {
            Node newNode;
            newNode.m_name = blockName;
            newNode.addNewParameter(newParameter);
            m_nodes.push_back(newNode);
        } else {

            nodeIter->addNewParameter(newParameter);
        }
        //}
    }

    void writeToStream(std::ostream & os, unsigned int tabs = 0) const {
        auto valueIter = m_values.begin();
        auto valueIterEnd = m_values.end();
        for (; valueIter != valueIterEnd; valueIter++) {
            // write comments
            std::string comment = valueIter->getComment();
            if (comment.length() > 0) {
                writeTabs(os, tabs);
                os << comment << std::endl;
            }

            writeTabs(os, tabs);
            os << valueIter->getName() << " = ";
            Entry entry = valueIter->getEntry();
            Entry::ENTRY_TYPE type = valueIter->getEntryType();
            switch (type) {
            case Entry::BOOL:
                if (entry.m_bool == false) {
                    os << "false";
                } else {
                    os << "true";
                }
                break;
            case Entry::DOUBLE:
                os << entry.m_double;
                break;
            case Entry::INTEGER:
                os << entry.m_integer;
                break;
            case Entry::STRING:
                if (entry.m_string->find_first_of(".") == std::string::npos) {
                    os << *entry.m_string;
                } else {
                    os << '\"'<< *entry.m_string << '\"';
                }
                break;
            }
            os << std::endl;
        }
        auto nodeIter = m_nodes.begin();
        auto nodeIterEnd = m_nodes.end();
        for (; nodeIter != nodeIterEnd; nodeIter++) {

            writeTabs(os, tabs);
            os << nodeIter->m_name << " { " << std::endl;

            nodeIter->writeToStream(os, tabs+3);
            writeTabs(os, tabs);
            os << "}" << std::endl;

        }


    }

private:
    std::string m_name;
    std::vector<Parameter> m_values;
    std::vector<Node> m_nodes;

    void writeTabs(std::ostream & os, unsigned int tabs) const {
        unsigned int index;
        for (index = 0; index < tabs; index++) {
            os << ' ';
        }
    }
};


SimplexParameterHandler::SimplexParameterHandler()
{
    m_filename = sm_defaultFilename;
    m_enableParallelization = false;
    m_getParallelOptions = false;
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
        // TODO: ezt majd le kell kodolni normalisan is


        Node root;
        auto iter = m_values.begin();
        auto iterEnd = m_values.end();
        unsigned int idx = 0;
        for (; iter != iterEnd; iter++, idx++) {
            root.addNewParameter(iter->second);

        }
        root.writeToStream(std::cout);

        out << R"(# Simplex parameter file for the Pannon Optimizer
       # If this file is present, the values of the given parameters can be changed.

       # Tolerances #

               Tolerances {
       # Pivot tolerance for SSX iterations
               e_pivot = 1e-06

        # Feasibility tolerance
                e_feasibility = 1e-08

        # Optimality tolerance
                e_optimality = 1e-08
    }

       # Starting procedures

               Starting {
       # Presolve
               Presolve {
               enable = false
    }

       # Scaling parameters
               Scaling {
               enable = false

        # BENICHOU: Benichou scaling with powers of 2
                type = BENICHOU
    }

       # Starting basis procedures
               Basis {
       # NONBASIC_TO_LOWER: Non-basic variables at lower bound
       # NONBASIC_TO_UPPER: Non-basic variables at upper bound
       # NONBASIC_MIXED: Mixed non-basic variable bounds
               starting_nonbasic_states = NONBASIC_TO_LOWER

        # Starting basis strategies
        # LOGICAL: Logical basis
        # CRASH: CRASH basis
                starting_basis_strategy = LOGICAL
    }
    }

       # Basis factorization

               Factorization {
       # Type of basis factorization
       # PFI: Product form of the inverse
       # LU: LU factorization
               type = PFI

        # Frequency of reinversions in number of iterations
                reinversion_frequency = 30
        # Constant value used for threshold pivoting
                pivot_threshold = 0.1

        # PFI specific parameters
                PFI {
        # The method used to process the non-triangular kernel
        # SIMPLE_KERNEL: Simple search for potential pivot positions
        # BLOCK_PIVOT: Create a block triangular form and pivot down the diagonal
        # BLOCK_ORDER = Create a block triangular form and order the blocks by column counts before pivoting
                nontriangular_method = BLOCK_ORDER

        # Pivot rules for the non-triangular part
        # SIMPLE_PIVOT = Simple pivot tolerance is used
        # THRESHOLD_PIVOT = Threshold pivoting
                nontriangular_pivot_rule = THRESHOLD_PIVOT
    }
    }

       # Pricing

               Pricing {
       # Pricing type
       # DANTZIG: Dantzig rule
       # DEVEX: Devex rule
       # STEEPEST_EDGE: Steepest edge rule
               type = DANTZIG

        # Parameters of the SIMPRI partial pricing framework
        # The range of vectors are divided into clusters during the pricing, which are
        # scanned as a round robin in order to find improving candidates.
        # With partial pricing it can be specified that only a certain number
        # of clusters are visited to find improving candidates. If no candidates
        # found, more clusters are involved until necessary.
        # It is also possible to stop the cluster visit if enough candidates are found.
        # Parameter valus 0 for improving candidates means this stop condition is turned off.
        # All the three parameters can be specified both for phase I and phase II.

                Simpri {
                phaseI_clusters = 1
                phaseI_visit_clusters = 1
                phaseI_improving_candidates = 0
                phaseII_clusters = 1
                phaseII_visit_clusters = 1
                phaseII_improving_candidates = 0
    }
    }

       # Ratiotest

               Ratiotest {
       # Use the piecewise linear concave function in primal phase I.
       # TRADITIONAL: Traditional one step method
       # PIECEWISE: Piecewise linear function
       # PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold
               nonlinear_primal_phaseI_function = PIECEWISE

        # Use the piecewise linear concave function in dual phase I.
        # TRADITIONAL: Traditional one step method
        # PIECEWISE: Piecewise linear function
        # PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold
                nonlinear_dual_phaseI_function = PIECEWISE

        # Use the piecewise linear concave function in dual phase II.
        # TRADITIONAL: Traditional one step method
        # PIECEWIESE: Piecewise linear function
        # PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold
                nonlinear_dual_phaseII_function = PIECEWISE

        # Enable fake feasibility handling in Dual Phase I and II
                enable_fake_feasibility = false

                Expand {

        # EXPAND procedure
        # INACTIVE: Inactive
        # HARRIS: Harris ratiotest
        # EXPANDING = Expanding tolerance
                type = EXPANDING

        # EXPAND multiplier initializing value
                multiplier = 0.01

        # EXPAND divider initializing value
                divider = 10000

    }

    }


               Perturbation {

       # Cost vector perturbation
       # INACTIVE: Inactive
       # STRUCTURAL: Active for structural variables
       # STRUCTURAL_AND_LOGICAL: Active for structural and logical variables
               perturb_cost_vector = INACTIVE

        # Measure of perturbation
                e_cost_vector = 1e-06

        # RHS perturbation
                perturb_rhs = false

        # Measure of perturbation
                e_rhs = 1e-06

        # Bound shifting in primal
                shift_bounds = false

        # Measure of shifting
                e_bounds = 1e-06

    }

       # Global

               Global {

       # Starting algorithm of the simplex method
       # PRIMAL: primal simplex
       # DUAL: dual simplex
               starting_algorithm = DUAL

        # Algorithm switching switching during the iterations
        # INACTIVE: inactive
        # SWITCH_BEFORE_INV: switch algorithm before each inversion
        # SWITCH_WHEN_ENTER_PH2: switch algorithm immediately when entering phase-2
        # SWITCH_BEFORE_INV_PH2: switch algorithm before each inversion in phase-2
        # SWITCH_WHEN_NO_IMPR: switch algorithm if the actual (phase-1 of phase-2) objective value is the same between two inversions
                switch_algorithm = INACTIVE

        # Level of iteration report
        # 0 = brief problem report    & solution only                   & only solution time
        # 1 = brief problem report    & one line per inversion          & only solution time
        # 2 = detailed problem report & one line per iteration          & brief time report 	b
        # 3 = detailed problem report & one detailed line per iteration & detailed time report
                debug_level = 1

        # Maximal number of iterations
                iteration_limit = 200000

        # Time limit for a problem (sec)
                time_limit = 3600

        # Previously specified states of the solution algorithm can be saved.
        # After a filename is specified, there are four options.
        # The file format can be BAS (standard format) or PBF (PanOpt Basis Format) .
        # The last basis can be marked to be saved as `basis_filename`_last.`format` .
        # A basis of a given iteration can be marked to be saved or basis can be saved periodically
        # each output will follow the following syntax: `basis_filename`_`iteration_number`.`format` .
        # Both parameters are integers, 0 means saving is turned off.

                SaveBasis {
                basis = false
                filename = basis
                format = PBF
                last_basis = false
                iteration = 0
                periodically = 0
    }

        # The solver can start from a given basis.
        # The file format can be BAS (standard format) or PBF (PanOpt Basis Format).

                LoadBasis {
                basis = false
                filename = basis
                format = PBF
    }

        # The solver can export specific data in CSV format for research purposes.
        # The export type specifies the set of output to be written.
        # The export should be clean, it is appended with one line for each problem solved
        # The available export types are the following:
        # PARAMETER_STUDY = Parameter study (reporting numerical problems, tolerances and the solution)
        # RATIOTEST_STUDY = Ratiotest study (reporting measures of the ratio tests)

                Export {
                enable = false
                filename = "exported_result.txt"
                type = PARAMETER_STUDY
    }
    }

               enableParallelization = true

                Parallel {

                Thread{
                Pricing {
                type = DANTZIG
    }
    }

                Thread{

                Pricing {
                type = DEVEX
    }

    }

    })";


                /* out << "!!! Simplex parameter file for the Pannon Optimizer !!! \n";
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

                                                out << "! EXPAND procedure \n"
                                                       "\t! 0 = Inactive \n"
                                                       "\t! 1 = Harris ratiotest \n"
                                                       "\t! 2 = Expanding tolerance \n"
                                                       "\t" << "expand = " << writeParameter("expand") << "\n";

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
                                                       "\t" << "export_type = " << writeParameter("export_type") << "\n";*/


                out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " << m_filename;
    }
}

void SimplexParameterHandler::initParameters()
{
    //Tolerances
    createParameter("Tolerances.e_pivot",
                    Entry::DOUBLE,
                    TOLERANCES_PIVOT_COMMENT);
    setParameterValue("Tolerances.e_pivot",
                      DefaultParameters::Tolerances::E_PIVOT);

    createParameter("Tolerances.e_feasibility",
                    Entry::DOUBLE,
                    TOLERANCES_E_FEASIBILITY_COMMENT);
    setParameterValue("Tolerances.e_feasibility",
                      DefaultParameters::Tolerances::E_FEASIBILITY);

    createParameter("Tolerances.e_optimality",
                    Entry::DOUBLE,
                    TOLERANCES_E_OPTIMALITY_COMMENT);
    setParameterValue("Tolerances.e_optimality",
                      DefaultParameters::Tolerances::E_OPTIMALITY);

    //Starting procedures
    createParameter("Starting.Presolve.enable",
                    Entry::BOOL, STARTING_PRESOLVE_ENABLE_COMMENT);
    setParameterValue("Starting.Presolve.enable",
                      DefaultParameters::Starting::Presolve::ENABLE);

    createParameter("Starting.Scaling.enable",
                    Entry::BOOL,
                    STARTING_SCALING_ENABLE_COMMENT);
    setParameterValue("Starting.Scaling.enable",
                      DefaultParameters::Starting::Scaling::ENABLE);

    createParameter("Starting.Scaling.type",
                    Entry::STRING,
                    STARTING_SCALING_TYPE_COMMENT);
    setParameterValue("Starting.Scaling.type",
                      DefaultParameters::Starting::Scaling::TYPE);

    createParameter("Starting.Basis.starting_nonbasic_states",
                    Entry::STRING,
                    STARTING_BASIS_STARTING_NONBASIC_STATES_COMMENT);
    setParameterValue("Starting.Basis.starting_nonbasic_states", DefaultParameters::Starting::Basis::STARTING_NONBASIC_STATES);

    createParameter("Starting.Basis.starting_basis_strategy",
                    Entry::STRING,
                    STARTING_BASIS_STARTING_BASIS_STRATEGY_COMMENT);
    setParameterValue("Starting.Basis.starting_basis_strategy",
                      DefaultParameters::Starting::Basis::STARTING_BASIS_STRATEGY);

    //Basis factorization
    createParameter("Factorization.type",
                    Entry::STRING,
                    FACTORIZATION_TYPE_COMMENT);
    setParameterValue("Factorization.type",
                      DefaultParameters::Factorization::TYPE);

    createParameter("Factorization.reinversion_frequency",
                    Entry::INTEGER,
                    FACTORIZATION_REINVERSION_FREQUENCY_COMMENT);
    setParameterValue("Factorization.reinversion_frequency",
                      DefaultParameters::Factorization::REINVERSION_FREQUENCY);

    createParameter("Factorization.pivot_threshold",
                    Entry::DOUBLE,
                    FACTORIZATION_PIVOT_THRESHOLD_COMMENT);
    setParameterValue("Factorization.pivot_threshold",
                      DefaultParameters::Factorization::PIVOT_THRESHOLD);

    createParameter("Factorization.PFI.nontriangular_method",
                    Entry::STRING,
                    FACTORIZATION_PFI_NONTRIANGULAR_METHOD_COMMENT);
    setParameterValue("Factorization.PFI.nontriangular_method",
                      DefaultParameters::Factorization::PFI::NONTRIANGULAR_METHOD);

    createParameter("Factorization.PFI.nontriangular_pivot_rule",
                    Entry::STRING,
                    FACTORIZATION_PFI_NONTRIANGULAR_PIVOT_RULE_COMMENT);
    setParameterValue("Factorization.PFI.nontriangular_pivot_rule",
                      DefaultParameters::Factorization::PFI::NONTRIANGULAR_PIVOT_RULE);

    //Pricing
    createParameter("Pricing.type",
                    Entry::STRING,
                    PRICING_TYPE_COMMENT);
    setParameterValue("Pricing.type",
                      DefaultParameters::Pricing::TYPE);

    createParameter("Pricing.Simpri.phaseI_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEI_CLUSTERS);

    createParameter("Pricing.Simpri.phaseI_visit_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_VISIT_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_visit_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEI_VISIT_CLUSTERS);

    createParameter("Pricing.Simpri.phaseI_improving_candidates",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_IMPROVING_CANDIDADES_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_improving_candidates",
                      DefaultParameters::Pricing::Simpri::PHASEI_IMPROVING_CANDIDATES);

    createParameter("Pricing.Simpri.phaseII_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEII_CLUSTERS);

    createParameter("Pricing.Simpri.phaseII_visit_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_VISIT_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_visit_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEII_VISIT_CLUSTERS);

    createParameter("Pricing.Simpri.phaseII_improving_candidates",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_IMPROVING_CANDIDADES_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_improving_candidates",
                      DefaultParameters::Pricing::Simpri::PHASEII_IMPROVING_CANDIDATES);

    //Ratiotest
    createParameter("Ratiotest.nonlinear_primal_phaseI_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_PRIMAL_PHASEI_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_primal_phaseI_function",
                      DefaultParameters::Ratiotest::NONLINEAR_PRIMAL_PHASEI_FUNCTION);

    createParameter("Ratiotest.nonlinear_dual_phaseI_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_DUAL_PHASEI_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_dual_phaseI_function",
                      DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEI_FUNCTION);

    createParameter("Ratiotest.nonlinear_dual_phaseII_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_DUAL_PHASEII_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_dual_phaseII_function",
                      DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEII_FUNCTION);

    createParameter("Ratiotest.enable_fake_feasibility",
                    Entry::BOOL,
                    RATIOTEST_ENABLE_FAKE_FEASIBILITY_COMMENT);
    setParameterValue("Ratiotest.enable_fake_feasibility",
                      DefaultParameters::Ratiotest::ENABLE_FAKE_FEASIBILITY);

    createParameter("Ratiotest.Expand.multiplier",
                    Entry::DOUBLE,
                    RATIOTEST_EXPAND_MULTIPLIER_COMMENT);
    setParameterValue("Ratiotest.Expand.multiplier",
                      DefaultParameters::Ratiotest::Expand::MULTIPLIER);

    createParameter("Ratiotest.Expand.divider",
                    Entry::INTEGER,
                    RATIOTEST_EXPAND_DIVIDER_COMMENT);
    setParameterValue("Ratiotest.Expand.divider",
                      DefaultParameters::Ratiotest::Expand::DIVIDER);

    createParameter("Ratiotest.Expand.type",
                    Entry::STRING,
                    RATIOTEST_EXPAND_TYPE_COMMENT);
    setParameterValue("Ratiotest.Expand.type",
                      DefaultParameters::Ratiotest::Expand::TYPE);

    //Perturbation
    createParameter("Perturbation.perturb_cost_vector",
                    Entry::STRING,
                    PERTURBATION_PERTURB_COST_VECTOR_COMMENT);
    setParameterValue("Perturbation.perturb_cost_vector",
                      DefaultParameters::Perturbation::PERTURB_COST_VECTOR);

    createParameter("Perturbation.e_cost_vector",
                    Entry::DOUBLE,
                    PERTURBATION_E_COST_VECTOR_COMMENT);
    setParameterValue("Perturbation.e_cost_vector",
                      DefaultParameters::Perturbation::E_COST_VECTOR);

    createParameter("Perturbation.perturb_rhs",
                    Entry::BOOL,
                    PERTURBATION_PERTURB_RHS_COMMENT);
    setParameterValue("Perturbation.perturb_rhs",
                      DefaultParameters::Perturbation::PERTURB_RHS);

    createParameter("Perturbation.e_rhs",
                    Entry::DOUBLE,
                    PERTURBATION_E_RHS_COMMENT);
    setParameterValue("Perturbation.e_rhs",
                      DefaultParameters::Perturbation::E_RHS);

    createParameter("Perturbation.shift_bounds",
                    Entry::BOOL,
                    PERTURBATION_SHIFT_BOUNDS_COMMENT);
    setParameterValue("Perturbation.shift_bounds",
                      DefaultParameters::Perturbation::SHIFT_BOUNDS);

    createParameter("Perturbation.e_bounds",
                    Entry::DOUBLE,
                    PERTURBATION_E_BOUNDS_COMMENT);
    setParameterValue("Perturbation.e_bounds",
                      DefaultParameters::Perturbation::E_BOUNDS);

    //Global
    createParameter("Global.starting_algorithm",
                    Entry::STRING,
                    GLOBAL_STARTING_ALGORITHM_COMMENT);
    setParameterValue("Global.starting_algorithm",
                      DefaultParameters::Global::STARTING_ALGORITHM);

    createParameter("Global.switch_algorithm",
                    Entry::STRING,
                    GLOBAL_SWITCH_ALGORITHM_COMMENT);
    setParameterValue("Global.switch_algorithm",
                      DefaultParameters::Global::SWITCH_ALGORITHM);

    createParameter("Global.debug_level",
                    Entry::INTEGER,
                    GLOBAL_DEBUG_LEVEL_COMMENT);
    setParameterValue("Global.debug_level",
                      DefaultParameters::Global::DEBUG_LEVEL);

    createParameter("Global.iteration_limit",
                    Entry::INTEGER,
                    GLOBAL_ITERATION_LIMIT_COMMENT);
    setParameterValue("Global.iteration_limit",
                      DefaultParameters::Global::ITERATION_LIMIT);

    createParameter("Global.time_limit",
                    Entry::DOUBLE,
                    GLOBAL_TIME_LIMIT_COMMENT);
    setParameterValue("Global.time_limit",
                      DefaultParameters::Global::TIME_LIMIT);

    createParameter("Global.SaveBasis.basis",
                    Entry::BOOL,
                    GLOBAL_SAVE_BASIS_BASIS_COMMENT);
    setParameterValue("Global.SaveBasis.basis",
                      DefaultParameters::Global::SaveBasis::BASIS);

    createParameter("Global.SaveBasis.filename",
                    Entry::STRING,
                    GLOBAL_SAVE_BASIS_FILENAME_COMMENT);
    setParameterValue("Global.SaveBasis.filename",
                      std::string(DefaultParameters::Global::SaveBasis::FILENAME));

    createParameter("Global.SaveBasis.format",
                    Entry::STRING,
                    GLOBAL_SAVE_BASIS_FORMAT_COMMENT);
    setParameterValue("Global.SaveBasis.format",
                      std::string(DefaultParameters::Global::SaveBasis::FORMAT));

    createParameter("Global.SaveBasis.last_basis",
                    Entry::BOOL,
                    GLOBAL_SAVE_BASIS_LAST_BASIS_COMMENT);
    setParameterValue("Global.SaveBasis.last_basis",
                      DefaultParameters::Global::SaveBasis::LAST_BASIS);

    createParameter("Global.SaveBasis.iteration",
                    Entry::INTEGER,
                    GLOBAL_SAVE_BASIS_ITERATION_COMMENT);
    setParameterValue("Global.SaveBasis.iteration",
                      DefaultParameters::Global::SaveBasis::ITERATION);

    createParameter("Global.SaveBasis.periodically",
                    Entry::INTEGER,
                    GLOBAL_SAVE_BASIS_PERIODICALLY_COMMENT);
    setParameterValue("Global.SaveBasis.periodically",
                      DefaultParameters::Global::SaveBasis::PERIODICALLY);

    createParameter("Global.LoadBasis.basis",
                    Entry::BOOL,
                    GLOBAL_LOAD_BASIS_BASIS_COMMENT);
    setParameterValue("Global.LoadBasis.basis",
                      DefaultParameters::Global::LoadBasis::BASIS);

    createParameter("Global.LoadBasis.filename",
                    Entry::STRING,
                    GLOBAL_LOAD_BASIS_FILENAME_COMMENT);
    setParameterValue("Global.LoadBasis.filename",
                      std::string(DefaultParameters::Global::LoadBasis::FILENAME));

    createParameter("Global.LoadBasis.format",
                    Entry::STRING,
                    GLOBAL_LOAD_BASIS_FORMAT_COMMENT);
    setParameterValue("Global.LoadBasis.format",
                      std::string(DefaultParameters::Global::LoadBasis::FORMAT));

    createParameter("Global.Export.enable",
                    Entry::BOOL,
                    GLOBAL_EXPORT_ENABLE_COMMENT);
    setParameterValue("Global.Export.enable",
                      DefaultParameters::Global::Export::ENABLE);

    createParameter("Global.Export.filename",
                    Entry::STRING,
                    GLOBAL_EXPORT_FILENAME_COMMENT);
    setParameterValue("Global.Export.filename",
                      std::string(DefaultParameters::Global::Export::FILENAME));

    createParameter("Global.Export.type",
                    Entry::STRING,
                    GLOBAL_EXPORT_TYPE_COMMENT);
    setParameterValue("Global.Export.type",
                      DefaultParameters::Global::Export::TYPE);

    createParameter("enableParallelization",
                    Entry::BOOL,
                    ENABLE_PARALLELIZATION_COMMENT);
    setParameterValue("enableParallelization",
                      DefaultParameters::ENABLE_PARALLELIZATION);


}

void SimplexParameterHandler::processParallelNode(const NodeFile::Node &node,
                                                  const std::string &name,
                                                  std::map<std::string, Parameter> *threadParameters)
{
    std::set<std::string> valueNames = node.getValueNames();
    auto valueIter = valueNames.begin();
    auto valueIterEnd = valueNames.end();
    for (; valueIter != valueIterEnd; valueIter++) {
        std::string parameterName = name + *valueIter;
        std::string value = node.getValue(*valueIter);
        //LPINFO("parameter: " << parameterName << " = " << value);
        Parameter parameter = m_values.at( parameterName );
        Entry::ENTRY_TYPE type = parameter.getEntryType();
        switch (type) {
        case Entry::BOOL:
            //LPINFO("\tBOOL");
            if (value == "true") {
                parameter.setBoolValue(true);
            } else if (value == "false") {
                parameter.setBoolValue(false);
            } else {
                throw ParameterException(std::string("Parameter error in parameter file: ").append(parameterName));
            }
            break;
        case Entry::DOUBLE:
            parameter.setDoubleValue( atof(value.c_str()) );
            //LPINFO("\tDOUBLE");
            break;
        case Entry::INTEGER:
            parameter.setIntegerValue( atoi(value.c_str()) );
            //LPINFO("\tINTEGER");
            break;
        case Entry::STRING:
            if (value[0] == '\"' && value[ value.length() - 1 ] == '\"') {
                value = value.substr(1, value.length() - 2);
            }
            parameter.setStringValue(value);
            //LPINFO("\tSTRING");
            break;
        }
        threadParameters->insert(std::make_pair(parameterName, parameter));

    }

    std::set<std::string> names = node.getNodeNames();
    auto nameIter = names.begin();
    auto nameIterEnd = names.end();
    for (; nameIter != nameIterEnd; nameIter++) {
        //LPINFO(*nameIter);
        std::vector<NodeFile::Node>::const_iterator nodeIter;
        std::vector<NodeFile::Node>::const_iterator nodeIterEnd;
        node.getNodes(*nameIter, &nodeIter, &nodeIterEnd);
        for (; nodeIter != nodeIterEnd; nodeIter++) {
            processParallelNode(*nodeIter, *nameIter + ".", threadParameters);
        }

    }
}

const Parameter &SimplexParameterHandler::getParameter(const std::string &name) const
{
    if (m_enableParallelization) {
        unsigned int threadId = ThreadSupervisor::getThreadId();
        auto iter = m_threadParameters[threadId].find(name);
        if (iter == m_threadParameters[threadId].end()) {
            return m_values.find(name)->second;
        }
        return iter->second;

    } else {
        auto iter = m_values.find(name);
        return iter->second;
    }
}

Parameter &SimplexParameterHandler::getParameter(const std::string & name)
{
    if (m_enableParallelization) {
        unsigned int threadId = ThreadSupervisor::getThreadId();
        auto iter = m_threadParameters[threadId].find(name);
        if (iter == m_threadParameters[threadId].end()) {
            return m_values.find(name)->second;
        }
        return iter->second;

    } else {
        auto iter = m_values.find(name);
        return iter->second;
    }}

void SimplexParameterHandler::loadValuesFromFile(std::ifstream &in)
{
    ParameterHandler::loadValuesFromFile(in);

    // process parallel options
    NodeFile::Node root = m_nodeFile.getDocumentNode();
    m_enableParallelization = false;
    try {
        m_enableParallelization = root.getValue("enableParallelization") == "true";
    } catch (...) {}
    std::vector<NodeFile::Node>::const_iterator nodeIter;
    std::vector<NodeFile::Node>::const_iterator nodeIterEnd;
    root.getNodes("Parallel", &nodeIter, &nodeIterEnd);

    std::vector<NodeFile::Node>::const_iterator threadIter;
    std::vector<NodeFile::Node>::const_iterator threadIterEnd;

    nodeIter->getNodes("Thread", &threadIter, &threadIterEnd);
    for (; threadIter != threadIterEnd; threadIter++) {
        //LPWARNING(threadIter->getName());
        std::map<std::string, Parameter> threadParameters;
        processParallelNode(*threadIter, "", &threadParameters);
        m_threadParameters.push_back(threadParameters);
    }

}

void SimplexParameterHandler::enableParallelOptions(bool value)
{
    m_enableParallelization = value;
}

const std::string &SimplexParameterHandler::getStringParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return *(parameter.getEntry().m_string);
}

const int &SimplexParameterHandler::getIntegerParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_integer;
}

const double &SimplexParameterHandler::getDoubleParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_double;
}

const bool &SimplexParameterHandler::getBoolParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_bool;
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const double value){
    m_values.at(name).setDoubleValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const int value){
    m_values.at(name).setIntegerValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const std::string value){
    m_values.at(name).setStringValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const char *value){
    m_values.at(name).setStringValue(std::string(value));
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const bool value){
    m_values.at(name).setBoolValue(value);
}
