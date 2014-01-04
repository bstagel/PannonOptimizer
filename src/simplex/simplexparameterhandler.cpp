/**
 * @file simplexparameterhandler.cpp
 */

#include <simplex/simplexparameterhandler.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>

THREAD_STATIC_DEF const char * SimplexParameterHandler::sm_defaultFilename = "simplex.PAR";

SimplexParameterHandler::SimplexParameterHandler()
{
    m_filename = sm_defaultFilename;
}

ParameterHandler& SimplexParameterHandler::getInstance()
{
    try{
    static SimplexParameterHandler s_instance;
    THREAD_STATIC_DECL bool s_init = false;
    if(!s_init) {
        s_instance.initParameters();
        s_instance.readParameterFile(sm_defaultFilename);
        s_init = true;
    }
    return s_instance;
    } catch (const ParameterException & exception) {
        LPERROR( "Simplex ParameterException: " <<exception.getMessage() );
        exit(-1);
}
}

void SimplexParameterHandler::writeParameterFile(){
    try {
        std::ofstream out(m_filename);
        if (!out.is_open()) throw -1;

        out << "!!! Simplex parameter file for the Pannon Optimizer !!!" << std::endl;
        out << "!\tIf this file is present, the values of the given parameters can be changed." << std::endl;

        out << std::scientific << std::endl;
        out << "!!! Tolerances !!!" << std::endl << std::endl;

        out << "! Pivot tolerance for SSX iterations" << std::endl;
        out << "\t" << "e_pivot = " <<writeParameter("e_pivot") << std::endl;

        out << "! Feasibility tolerance" << std::endl;
        out << "\t" << "e_feasibility = " << writeParameter("e_feasibility") << std::endl;

        out << "! Optimality tolerance" << std::endl;
        out << "\t" << "e_optimality = " << writeParameter("e_optimality") << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Starting procedures !!!" << std::endl << std::endl;

        out << "! Presolve" << std::endl;
        out << "\t" << "presolve = " << writeParameter("presolve") << std::endl;

        out << "! Scaling" << std::endl;
        out << "\t" << "scaling = " << writeParameter("scaling") << std::endl;

        out << "! Starting procedures" << std::endl;
        out << "! 0 = Lower logical\n"
               "! 1 = Upper logical\n"
               "! 2 = Mixed logical" << std::endl;
        out << "\t" << "starting_basis = " << writeParameter("starting_basis") << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Basis factorization !!!" << std::endl << std::endl;

        out << "! Type of basis factorization\n"
               "! 0 = PFI" << std::endl;
        out << "\t" << "factorization_type = " << writeParameter("factorization_type") << std::endl << std::endl;

        out << "\t! PFI specific parameters" << std::endl;
        out << "\t! The method used to process the non-triangular kernel\n"
               "\t! 0 = Simple search for potential pivot positions\n"
               "\t! 1 = Create a block triangular form and pivot down the diagonal\n"
               "\t! 2 = Create a block triangular form and order the blocks by column counts before pivoting"<< std::endl;
        out << "\t\t" << "nontriangular_method = " << writeParameter("nontriangular_method") <<  std::endl;

        out << "\t! Pivot rules for the non-triangular part\n"
               "\t! 0 = Simple pivot tolerance is used\n"
               "\t! 1 = Threshold pivoting" <<  std::endl;
        out << "\t\t" << "nontriangular_pivot_rule = " << writeParameter("nontriangular_pivot_rule") <<  std::endl;

        out << "\t! Frequency of reinversions in # of iterations" <<  std::endl;
        out << "\t\t" << "reinversion_frequency = " << writeParameter("reinversion_frequency") <<  std::endl;

        out << "\t ! Constant value used for threshold pivoting" <<  std::endl;
        out << "\t\t" << "pivot_threshold = " << writeParameter("pivot_threshold") <<  std::endl;


        out << std::fixed << std::endl;
        out << "!!! Pricing !!!" <<  std::endl <<  std::endl;

        out << "! Pricing type\n"
               "! 0 = Dantzig" << std::endl;
        out << "\t" << "pricing_type = " << writeParameter("pricing_type") << std::endl;


        out << std::fixed << std::endl;
        out << "!!! Ratiotest !!!" <<  std::endl <<  std::endl;

        out << "! Use the piecewise linear concave function in primal phase I. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_primal_phaseI_function = " << writeParameter("nonlinear_primal_phaseI_function") << std::endl;


        out << "! Use the piecewise linear concave function in dual phase I. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_dual_phaseI_function = " << writeParameter("nonlinear_dual_phaseI_function") << std::endl;

        out << "! Use the piecewise linear concave function in dual phase II. \n"
               "\t! 0 = Traditional one step method \n"
               "\t! 1 = Piecewise linear function \n"
               "\t! 2 = Piecewise linear function with numerical threshold \n"
               "\t" << "nonlinear_dual_phaseII_function = " << writeParameter("nonlinear_dual_phaseII_function") << std::endl;

        out << "! Numerical Threshold report in Dual Phase I and II \n"
               "\t! 0 = no report \n"
               "\t! 1 = report threshold active & sum of steps \n"
               "\t! 2 = report threshold step by step"<< std::endl;
        out << "\t" << "threshold_report_level = " << writeParameter("threshold_report_level")
            << std::endl << std::endl;

        out << "! EXPAND procedure in Dual Phase I \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Active \n"
               "\t" << "expand_dual_phaseI = " << writeParameter("expand_dual_phaseI")
                << std::endl << std::endl;
        out << "! EXPAND procedure in Dual Phase II \n"
               "\t! 0 = Inactive \n"
               "\t! 1 = Active \n"
               "\t" << "expand_dual_phaseII = " << writeParameter("expand_dual_phaseII")
                << std::endl << std::endl;

        out << std::fixed << std::endl;
        out << "!!! Global !!!" <<  std::endl <<  std::endl;
        out << "! Level of iteration report\n"
               "! 0 = brief problem report\t&\tsolution only\t\t\t&\tonly solution time\n"
               "! 1 = brief problem report\t&\tone line per major iteration\t& \tonly solution time\n"
               "! 2 = detailed problem report\t&\tone line per major iteration\t&\tbrief time report\n"
               "! 3 = detailed problem report\t&\tone line per minor iteration\t&\tdetailed time report" << std::endl;
        out << "\t" << "debug_level = " << writeParameter("debug_level") << std::endl;

        out << "! Maximal # of iterations" <<  std::endl;
        out << "\t" << "iteration_limit = " << writeParameter("iteration_limit") <<  std::endl;

        out << "! Time limit for a problem (sec)" <<  std::endl;
        out << "\t" << "time_limit = " << writeParameter("time_limit") <<  std::endl;


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
    createParameter("scaling", Entry::BOOL);
    setParameterValue("scaling", DefaultParameters::SCALING);
    createParameter("starting_basis", Entry::INTEGER);
    setParameterValue("starting_basis", DefaultParameters::STARTING_BASIS);

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

    //Ratiotest
    createParameter("nonlinear_primal_phaseI_function", Entry::INTEGER);
    setParameterValue("nonlinear_primal_phaseI_function", DefaultParameters::NONLINEAR_PRIMAL_PHASEI_FUNCTION);
    createParameter("nonlinear_dual_phaseI_function", Entry::INTEGER);
    setParameterValue("nonlinear_dual_phaseI_function", DefaultParameters::NONLINEAR_DUAL_PHASEI_FUNCTION);
    createParameter("nonlinear_dual_phaseII_function", Entry::INTEGER);
    setParameterValue("nonlinear_dual_phaseII_function", DefaultParameters::NONLINEAR_DUAL_PHASEII_FUNCTION);

    createParameter("threshold_report_level", Entry::INTEGER);
    setParameterValue("threshold_report_level", DefaultParameters::THRESHOLD_REPORT_LEVEL);
    createParameter("expand_dual_phaseI", Entry::INTEGER);
    setParameterValue("expand_dual_phaseI", DefaultParameters::EXPAND_DUAL_PHASEI);
    createParameter("expand_dual_phaseII", Entry::INTEGER);
    setParameterValue("expand_dual_phaseII", DefaultParameters::EXPAND_DUAL_PHASEII);

    //Global
    createParameter("debug_level", Entry::INTEGER);
    setParameterValue("debug_level", DefaultParameters::DEBUG_LEVEL);
    createParameter("iteration_limit", Entry::INTEGER);
    setParameterValue("iteration_limit", DefaultParameters::ITERATION_LIMIT);
    createParameter("time_limit", Entry::DOUBLE);
    setParameterValue("time_limit", DefaultParameters::TIME_LIMIT);

}
