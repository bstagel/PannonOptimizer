/**
 * @file linalgparameterhandler.cpp
 */
#include <linalg/linalgparameterhandler.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>
#include <initpanopt.h>

#include <cstdio>

THREAD_STATIC_DEF const char * LinalgParameterHandler::sm_defaultFilename = "linalg.PAR";

LinalgParameterHandler * LinalgParameterHandler::sm_instance;

LinalgParameterHandler::LinalgParameterHandler()
{
    m_filename = sm_defaultFilename;
}

ParameterHandler& LinalgParameterHandler::getInstance()
{
   // std::cout << "INIT LINALG" << std::endl;
    try{
        return *sm_instance;
        /*static LinalgParameterHandler * s_instance = 0;
        if(!s_instance) {

            InitPanOpt::getInstance().init();

            s_instance = new LinalgParameterHandler;
            s_instance->initParameters();
            s_instance->readParameterFile(sm_defaultFilename);
        }
        return *s_instance;*/
    } catch (const ParameterException & exception) {
        LPERROR( "Linalg ParameterException: " <<exception.getMessage() );
        exit(-1);
    }
}

void LinalgParameterHandler::init() {
    sm_instance = new LinalgParameterHandler;
    sm_instance->initParameters();
    sm_instance->readParameterFile(sm_defaultFilename);
}


void LinalgParameterHandler::writeParameterFile()
{
    try {
        std::ofstream out;
        out.open(m_filename, std::ios::out | std::ios::binary);

        if (!out.is_open()) throw -1;

        out << "!!! Linear Algebraic parameter file for the Pannon Optimizer !!! \n";
        out << "!\tIf this file is present, the values of the given parameters can be changed. \n";

        out << std::scientific << "\n";

        out << "!!! Tolerances !!! \n\n";
        out << "! Absolute tolerance \n"
               "\t" << "e_absolute = " << writeParameter("e_absolute") << "\n";
        out << "! Relative tolerance for additive operations \n"
               "\t" << "e_relative = " << writeParameter("e_relative") << "\n";

        out << std::fixed << "\n";

        out << "! Nonzero ratio used as a bound between sparse and dense vector forms \n"
               "\t" << "sparsity_ratio = " << writeParameter("sparsity_ratio") << "\n";
        out << "! The number of extra spaces reserved in vectors for new elements \n"
               "\t" << "elbowroom = " << writeParameter("elbowroom") << "\n";

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
