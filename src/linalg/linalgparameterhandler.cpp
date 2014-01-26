/**
 * @file linalgparameterhandler.cpp
 */
#include <linalg/linalgparameterhandler.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>

#include <cstdio>

THREAD_STATIC_DEF const char * LinalgParameterHandler::sm_defaultFilename = "linalg.PAR";

LinalgParameterHandler::LinalgParameterHandler()
{
    m_filename = sm_defaultFilename;
}

ParameterHandler& LinalgParameterHandler::getInstance()
{
    try{
        static LinalgParameterHandler s_instance;
        THREAD_STATIC_DECL bool s_init = false;
        if(!s_init) {
            s_instance.initParameters();
            s_instance.readParameterFile(sm_defaultFilename);
            s_init = true;
        }
        return s_instance;
    } catch (const ParameterException & exception) {
        LPERROR( "Linalg ParameterException: " <<exception.getMessage() );
        exit(-1);
    }
}



void LinalgParameterHandler::writeParameterFile()
{
    try {
        std::ofstream out;
        out.open(m_filename, std::ios::out | std::ios::binary);

        if (!out.is_open()) throw -1;

        out << "!!! Linear Algebraic parameter file for the Pannon Optimizer !!!" << std::endl;
        out << "!\tIf this file is present, the values of the given parameters can be changed." << std::endl;

        out << std::scientific << std::endl;
        out << "!!! Tolerances !!!" << std::endl << std::endl;
        out << "! Absolute tolerance" << std::endl;
        out << "\t" << "e_absolute = " << writeParameter("e_absolute") << std::endl;
        out << "! Relative tolerance for additive operations" << std::endl;
        out << "\t" << "e_relative = " << writeParameter("e_relative") << std::endl << std::endl;

        out << std::fixed << std::endl;
        out << "! Nonzero ratio used as a bound between sparse and dense vector forms" << std::endl;
        out << "\t" << "sparsity_ratio = " << writeParameter("sparsity_ratio") << std::endl;
        out << "! The number of extra spaces reserved in vectors for new elements" << std::endl;
        out << "\t" << "elbowroom = " << writeParameter("elbowroom") << std::endl;

        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " << m_filename << std::endl;
    }
}

void LinalgParameterHandler::initParameters()
{
    //Tolerances
    createParameter("e_absolute", Entry::DOUBLE);
    setParameterValue("e_absolute", DefaultParameters::E_ABSOLUTE);
    createParameter("e_relative", Entry::DOUBLE);
    setParameterValue("e_relative", DefaultParameters::E_RELATIVE);

    //Global
    createParameter("sparsity_ratio", Entry::DOUBLE);
    setParameterValue("sparsity_ratio", DefaultParameters::SPARSITY_RATIO);
    createParameter("elbowroom", Entry::INTEGER);
    setParameterValue("elbowroom", DefaultParameters::ELBOWROOM);

}
