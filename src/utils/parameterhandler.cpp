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
        std::ifstream in;
        in.open(filename);
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
    try {
        std::string line;
        std::vector<std::string> tokens;

        while (!in.eof()) {
            line = ignoreEmptyRows(in);
            if(in.eof()) {
                break;
            }

            tokens = tokenizer(line);
            if (tokens.size()==0){
                //Handling comment lines
                continue;
            }

            if (tokens.size()<3 && tokens[1] != "=") {
                throw ParameterException(std::string("Parameter error in parameter file: ").append(tokens[0]));
            }

            if (m_values.find(tokens[0])!=m_values.end()) {
                Parameter& parameter = m_values.at(tokens[0]);
                Entry::ENTRY_TYPE type = parameter.getEntryType();
                if(type == Entry::DOUBLE){
                    if(tokens.size() == 3){
                        parameter.setDoubleValue(atof(tokens[2].c_str()));
                    } else {
                        throw ParameterException(std::string("Parameter error in parameter file: ").append(tokens[0]));
                    }
                } else if (type == Entry::INTEGER){
                    if(tokens.size() == 3){
                        parameter.setIntegerValue(atoi(tokens[2].c_str()));
                    } else {
                        throw ParameterException(std::string("Parameter error in parameter file: ").append(tokens[0]));
                    }
                } else if (type == Entry::STRING){
                    std::string value;
                    for(unsigned int i=2; i<tokens.size(); i++){
                        value.append(tokens[i]);
                        if(i != tokens.size()-1){
                            value.append(" ");
                        }
                    }
                    parameter.setStringValue(value);
                } else if (type == Entry::BOOL){
                    if(tokens.size() == 3){
                        if(tokens[2].compare("true") == 0){
                            parameter.setBoolValue(true);
                        } else if(tokens[2].compare("false") == 0){
                            parameter.setBoolValue(false);
                        } else {
                            throw ParameterException(std::string("Parameter error in parameter file: ").append(tokens[0]));
                        }
                    } else {
                        throw ParameterException(std::string("Parameter error in parameter file: ").append(tokens[0]));
                    }
                }
            }
            else {
                throw std::string("Unknown Parameter in the Parameter File: ").append(tokens[0]);
            }
        }
    }
    catch(const std::string& str) {
        std::cerr << str << std::endl;
    }
}

std::vector<std::string> ParameterHandler::tokenizer(std::string& line) {
    std::vector<std::string> tokens;
    char *p_ch;
    p_ch = strtok((char*) line.c_str()," \t");
    while (p_ch!=0)
    {
        tokens.push_back(p_ch);
        p_ch = strtok(NULL, " \t");
    }

    for(unsigned int i=0;i < tokens.size(); i++){
        if(tokens[i].find("!") != std::string::npos){
            tokens.erase(tokens.begin()+i,tokens.end());
            break;
        }
    }
    return tokens;
}

std::string ParameterHandler::ignoreEmptyRows(std::ifstream& in) {
    std::string line;
    getline(in, line);
    while (!in.eof() && line.size()==0) {
        getline(in, line);
    }
    return line;
}
