#include <simplex/simplexparameterhandler.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>

static const std::string filename = "simplex.PAR";

SimplexParameterHandler::SimplexParameterHandler()
{

}

ParameterHandler& SimplexParameterHandler::getInstance()
{
    static SimplexParameterHandler s_instance;
    static bool s_init = false;
    if(!s_init) {
        s_instance.initParameters();
        s_instance.readParameterFile(filename);
        s_init = true;
    }
    return s_instance;
}

void SimplexParameterHandler::writeParameterFile()
{try {
        std::ofstream out(filename.data());
        if (!out.is_open()) throw -1;

        out << "!!! Simplex parameter file for the Pannon Optimizer !!!" << std::endl;
        out << "!\tIf this file is present, the values of the given parameters can be changed." << std::endl;

        out << std::scientific << std::endl;
        out << "!!! Tolerances !!!" << std::endl << std::endl;

        out << "! Pivot tolerance for SSX iterations" << std::endl;
        out << "\t" << "e_pivot = " << m_values["e_pivot"].getValue() << std::endl;

        out << "! Harris ratio tolerance" << std::endl;
        out << "\t" << "e_harris = " << m_values["e_harris"].getValue() << std::endl;

        out << "! Feasibility tolerance" << std::endl;
        out << "\t" << "e_feasibility = " << m_values["e_feasibility"].getValue() << std::endl;

        out << "! Optimality tolerance" << std::endl;
        out << "\t" << "e_optimality = " << m_values["e_optimality"].getValue() << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Starting procedures !!!" << std::endl << std::endl;

        out << "! Presolve: 0.0 = no, 1.0 = yes" << std::endl;
        out << "\t" << "presolve = " << m_values["presolve"].getValue() << std::endl;

        out << "! Scaling: 0.0 = no, 1.0 = yes" << std::endl;
        out << "\t" << "scaling = " << m_values["scaling"].getValue() << std::endl;
        out << "! Starting procedures" << std::endl;

        out << "! 0.0 = Lower logical\n! 1.0 = Upper logical\n! 2.0 = Mixed logical\n"
               "! 3.0 = Symbolic crash\n! 4.0 = LTSF Crash\n! 5.0 = ADG Crash\n"
               "! 6.0 = ADG+LTSF Crash" << std::endl;
        out << "\t" << "starting_basis = " << m_values["starting_basis"].getValue() << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Basis factorization !!!" << std::endl << std::endl;

        out << "! Type of basis factorization\n! 0.0 = PFI\n"
               "! 1.0 = LU Decomposition --- NOT AVAILABLE YET" << std::endl;
        out << "\t" << "factorization_type = " << m_values["factorization_type"].getValue() << std::endl;

        out << "\t! PFI specific parameters\n"
               "! The method used to identify the triangular parts of the basis\n"
               "\t! 0.0 = Iterative search\n"
               "\t! 1.0 = Linked lists based on the row and column counts" << std::endl;

        out << "\t" << "triangular_method = " << m_values["triangular_method"].getValue() << std::endl;
        out << "\t! The method used to process the non-triangular kernel\n"
               "\t! 0.0 = Simple search for potential pivot positions\n"
               "\t! 1.0 = Create a block triangular form and pivot down the diagonal\n"
               "\t! 2.0 = Create a block triangular form and order the blocks by column counts before pivoting"<< std::endl;
        out << "\t\t" << "nontriangular_method = " << m_values["nontriangular_method"].getValue() <<  std::endl;

        out << "\t! Pivot tolerance for the triangular part\n\t! 0.0 = Absolute tolerance\n"
               "\t! 1.0 = Relative tolerance" <<  std::endl;
        out << "\t\t" << "triangular_pivot_rule = " << m_values["triangular_pivot_rule"].getValue() <<  std::endl;

        out << "\t! Pivot rules for the non-triangular part\n\t! 0.0 = Simple pivot tolerance is used\n"
               "\t! 1.0 = Threshold pivoting" <<  std::endl;
        out << "\t\t" << "nontriangular_pivot_rule = " << m_values["nontriangular_pivot_rule"].getValue() <<  std::endl;

        out << "\t ! Constant value used for threshold pivoting" <<  std::endl;
        out << "\t\t" << "pivot_threshold = " << m_values["pivot_threshold"].getValue() <<  std::endl;

        out << "! Frequency of reinversions in # of iterations" <<  std::endl;
        out << "\t" << "reinversion_frequency = " << m_values["reinversion_frequency"].getValue() <<  std::endl;


        out << std::fixed << std::endl;
        out << "!!! Pricing !!!" <<  std::endl <<  std::endl;

        out << "! Pricing type\n"
               "! 0.0 = Dantzig\n"
               "! 1.0 = Exact steepest edge"
               "! 2.0 = Another steepest edge"
               "! 3.0 = Devex" << std::endl;
        out << "\t" << "pricing_type = " << m_values["pricing_type"].getValue() << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Global !!!" <<  std::endl <<  std::endl;
        out << "! Level of iteration report"
               "\t! -1.0 = CSV export format\n"
               "\t! 0.0 = brief problem report\t&\tsolution only\t\t\t&\tonly solution time\n"
               "\t! 1.0 = brief problem report\t&\tone line per major iteration\t& \tonly solution time\n"
               "\t! 2.0 = detailed problem report\t&\tone line per major iteration\t&\tbrief time report\n"
               "\t! 3.0 = detailed problem report\t&\tone line per minor iteration\t&\tdetailed time report" << std::endl;
        out << "\t" << "debug_level = " << m_values["debug_level"].getValue() << std::endl;

        out << "! Maximal # of iterations" <<  std::endl;
        out << "\t" << "iteration_limit = " << m_values["iteration_limit"].getValue() <<  std::endl;

        out << "! Time limit for a problem (sec)" <<  std::endl;
        out << "\t" << "time_limit = " << m_values["time_limit"].getValue() <<  std::endl;


        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " <<filename;
    }
}

void SimplexParameterHandler::initParameters()
{
    //Tolerances
    m_values["e_pivot"] = Parameter("e_pivot", DefaultParameters::E_PIVOT);
    m_values["e_harris"] = Parameter("e_harris", DefaultParameters::E_HARRIS);
    m_values["e_feasibility"] = Parameter("e_feasibility", DefaultParameters::E_FEASIBILITY);
    m_values["e_optimality"] = Parameter("e_optimality", DefaultParameters::E_OPTIMALITY);

    //Starting procedures
    m_values["presolve"] = Parameter("presolve", DefaultParameters::PRESOLVE);
    m_values["scaling"] = Parameter("scaling", DefaultParameters::SCALING);
    m_values["starting_basis"] = Parameter("starting_basis", DefaultParameters::STARTING_BASIS);

    //Basis factorization
    m_values["factorization_type"] = Parameter("factorization_type", DefaultParameters::FACTORIZATION_TYPE);
    m_values["nontriangular_method"] = Parameter("nontriangular_method", DefaultParameters::NONTRIANGULAR_METHOD);
    m_values["nontriangular_pivot_rule"] = Parameter("nontriangular_pivot_rule", DefaultParameters::NONTRIANGULAR_PIVOT_RULE);
    m_values["pivot_threshold"] = Parameter("pivot_threshold", DefaultParameters::PIVOT_THRESHOLD);
    m_values["reinversion_frequency"] = Parameter("reinversion_frequency", DefaultParameters::REINVERSION_FREQUENCY);

    //Pricing
    m_values["pricing_type"] = Parameter("pricing_type", DefaultParameters::PRICING_TYPE);

    //Global
    m_values["debug_level"] = Parameter("debug_level", DefaultParameters::DEBUG_LEVEL);
    m_values["iteration_limit"] = Parameter("iteration_limit", DefaultParameters::ITERATION_LIMIT);
    m_values["time_limit"] = Parameter("time_limit", DefaultParameters::TIME_LIMIT);

}
