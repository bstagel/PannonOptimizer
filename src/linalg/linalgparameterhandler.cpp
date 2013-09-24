#include "linalg/linalgparameterhandler.h"

#include <fstream>
#include <iostream>

#include "defaultparameters.h"

static const std::string filename = "linalg.PAR";

LinalgParameterHandler::LinalgParameterHandler()
{

}

ParameterHandler& LinalgParameterHandler::getInstance()
{
    static LinalgParameterHandler s_instance;
    static bool s_init = false;
    if(!s_init) {
        s_instance.initParameters();
        //s_instance.readParameterFile(filename);
        s_init = true;
    }
    return s_instance;
}

void LinalgParameterHandler::writeParameterFile()
{try {
        std::ofstream out(filename.data());
        if (!out.is_open()) throw -1;

        out << "!!! Linear Algebraic parameter file for the Pannon Optimizer !!!" << std::endl;
        out << "!\tIf this file is present, the values of the given parameters can be changed." << std::endl;

        out << std::scientific << std::endl;
        out << "!!! Tolerances !!!" << std::endl << std::endl;
        out << "! Absolute tolerance" << std::endl;
        out << "\t" << "e_absolute = " << m_values["e_absolute"].getValue() << std::endl;
        out << "! Relative tolerance for additive operations" << std::endl;
        out << "\t" << "e_relative = " << m_values["e_relative"].getValue() << std::endl << std::endl;

        out << std::fixed << std::endl;
        out << "! Nonzero ratio used as a bound between sparse and dense vector forms" << std::endl;
        out << "\t" << "sparsity_ratio = " << m_values["sparsity_ratio"].getValue() << std::endl;
        out << "! The number of extra spaces reserved in vectors for new elements" << std::endl;
        out << "\t" << "elbowroom = " << m_values["elbowroom"].getValue() << std::endl;

        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " <<filename;
    }
}

void LinalgParameterHandler::initParameters()
{
    //Tolerances
    m_values["e_absolute"] = Parameter("e_absolute", DefaultParameters::E_ABSOLUTE);
    m_values["e_relative"] = Parameter("e_relative", DefaultParameters::E_RELATIVE);

    //Global
    m_values["sparsity_ratio"] = Parameter("sparsity_ratio", DefaultParameters::SPARSITY_RATIO);
    m_values["elbowroom"] = Parameter("elbowroom", DefaultParameters::SPARSITY_RATIO);

}
