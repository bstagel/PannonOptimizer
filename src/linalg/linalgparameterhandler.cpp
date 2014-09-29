/**
 * @file linalgparameterhandler.cpp
 */

#include <linalg/linalgparameterhandler.h>
#include <linalg/linalgparametercomments.h>

#include <fstream>
#include <iostream>

#include <defaultparameters.h>
#include <initpanopt.h>

#include <cstdio>

#include <utils/parameterfilebuilder.h>

thread_local const char * LinalgParameterHandler::sm_defaultFilename = "linalg.PAR";

LinalgParameterHandler * LinalgParameterHandler::sm_instance;

LinalgParameterHandler::LinalgParameterHandler()
{
    m_filename = sm_defaultFilename;
}

ParameterHandler& LinalgParameterHandler::getInstance()
{
    try{
        return *sm_instance;
    } catch (const ParameterException & exception) {
        LPERROR( "Linalg ParameterException: " <<exception.getMessage() );
        exit(-1);
    }
}

void LinalgParameterHandler::_globalInit() {
    sm_instance = new LinalgParameterHandler;
    sm_instance->initParameters();
    sm_instance->readParameterFile(sm_defaultFilename);
}

void LinalgParameterHandler::_globalRelease()
{
    delete sm_instance;
    sm_instance = nullptr;
}


void LinalgParameterHandler::writeParameterFile()
{

    try {
        std::ofstream out;
        out.open(m_filename, std::ios::out | std::ios::binary);

        if (!out.is_open()) throw -1;

        ParameterFileBuilder root;

        for (auto & iter: m_valueNames) {
            root.addNewParameter(m_values[iter]);
        }
        root.addComment("", LINALG_PARAMETERFILE_FIRST_COMMENT);

        root.writeToStream(out);
        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " << m_filename;
    }

}

void LinalgParameterHandler::initParameters()
{
    //Tolerances
    createParameter("e_absolute", Entry::DOUBLE, E_ABSOLUTE_COMMENT);
    setParameterValue("e_absolute", DefaultParameters::E_ABSOLUTE);
    createParameter("e_relative", Entry::DOUBLE, E_RELATIVE_COMMENT);
    setParameterValue("e_relative", DefaultParameters::E_RELATIVE);

    //Global
    createParameter("sparsity_ratio", Entry::DOUBLE, SPARSITY_RATIO_COMMENT);
    setParameterValue("sparsity_ratio", DefaultParameters::SPARSITY_RATIO);
    createParameter("elbowroom", Entry::INTEGER, ELBOWROOM_COMMENT);
    setParameterValue("elbowroom", DefaultParameters::ELBOWROOM);

}
