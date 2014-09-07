/**
 * @file parameterhandler.cpp
 */

#include <utils/parameterhandler.h>
#include <defaultparameters.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <debug.h>

#include <cstdio>

ParameterHandler::~ParameterHandler()
{

}

void ParameterHandler::readParameterFile(const char * filename){
    try {
        std::ifstream in(filename, std::ifstream::in);
        //in.open(filename);
        if (!in.is_open()){
            writeParameterFile();
            in.open(filename);
            if (!in.is_open()){
                throw -1;
            }
        }
        loadValuesFromFile(in);
        in.close();
    }
    catch(int) {
        std::cerr << "Parameter file error : " << filename << std::endl;
    }
}

const std::string ParameterHandler::writeParameter(std::string name){
    Parameter& parameter = m_values.at(name);
    if(parameter.getEntryType() == Entry::DOUBLE) {
        std::ostringstream strs;
        strs << parameter.getEntry().m_double;
        return strs.str();
    } else if(parameter.getEntryType() == Entry::INTEGER){
        return std::to_string(parameter.getEntry().m_integer);
    } else if(parameter.getEntryType() == Entry::STRING){
        return *(parameter.getEntry().m_string);
    } else if(parameter.getEntryType() == Entry::BOOL){
        return parameter.getEntry().m_bool?std::string("true"):std::string("false");
    } else {
        throw ParameterException("Invalid parameter type");
    }
}

void ParameterHandler::loadValuesFromFile(std::ifstream &in){
    NodeFile file;

    try {
        file.loadFromStream(in);
    } catch (const SyntaxErrorException & ex ) {
        ex.show();
        exit(1);
    }
    const NodeFile::Node & root = file.getDocumentNode();

    processNode(root);

    m_nodeFile = file;
}

void ParameterHandler::processNode(const NodeFile::Node & node,
                                   const std::string & name)
{
    std::set<std::string> valueNames = node.getValueNames();
    auto valueIter = valueNames.begin();
    auto valueIterEnd = valueNames.end();
    for (; valueIter != valueIterEnd; valueIter++) {
        std::string parameterName = name;
        if (parameterName.length() > 0) {
            parameterName = parameterName + ".";
        }
        parameterName = parameterName + *valueIter;
        //LPINFO(parameterName);
        std::string value = node.getValue(*valueIter);
        try {
            Parameter& parameter = m_values.at( parameterName );
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
        } catch (const std::out_of_range & ex) {

        }
    }


    std::set<std::string> nodeNames = node.getNodeNames();
    auto nodeIter = nodeNames.begin();
    auto nodeIterEnd = nodeNames.end();

    for (; nodeIter != nodeIterEnd; nodeIter++) {
        std::vector<NodeFile::Node>::const_iterator childIter;
        std::vector<NodeFile::Node>::const_iterator childIterEnd;
        node.getNodes(*nodeIter, &childIter, &childIterEnd);
        for (; childIter != childIterEnd; childIter++) {
            if (name.length() > 0) {
                processNode(*childIter, name + "." + *nodeIter);
            } else {
                processNode(*childIter, *nodeIter);
            }
        }
    }
}
