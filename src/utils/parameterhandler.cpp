/**
 * @file parameterhandler.cpp
 */

#include "utils/parameterhandler.h"
#include <defaultparameters.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "debug.h"

/*static const char* groupNames[] = {"Tolerances",
  "Preprocessing",
  "Starting procedures",
  "Basis factorization",
  "Pricing",
  "Ratio test",
  "Global"};
*/

ParameterHandler* ParameterHandler::m_instance = 0;

//ParameterHandler::ParameterHandler(const std::string &fileName){
//    initParameters();

//    try {
//        std::ifstream in(fileName.c_str());
//        if (!in.is_open()){
//            throw -1;
//        }
//        loadValuesFromFile(in);
//        in.close();
//    }
//    catch(int) {
//        std::cerr << "Parameter file not found : " << fileName << std::endl;
//    }
//}

ParameterHandler::~ParameterHandler()
{
    m_instance = 0;
}

void ParameterHandler::loadValuesFromFile(std::ifstream &in)
{
    try {
        std::string line;
        std::vector<std::string> tokens;

        while (!in.eof()) {
            line = ignoreEmptyRows(in);
            if(in.eof()) {
                break;
            }
            tokens = tokenizer(line);

            if (tokens.size()!=3 && tokens[1] != "=") {
                throw std::string("Parameter error ... ");
            }

            double val = strtod(tokens[2].c_str(), NULL);

            if (m_values.find(tokens[0])!=m_values.end()) {
                m_values[tokens[0]].setValue(val);
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
    while (!in.eof() && (line.size()==0 || line[0]=='!')) {
        getline(in, line);
    }
    return line;
}
