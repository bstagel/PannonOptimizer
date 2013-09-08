/**
 * @file parameterhandler.h
 */

#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <map>
#include <vector>

#include "utils/parameter.h"

class ParameterHandler{
public:
    virtual ParameterHandler* getInstance() = 0;
    virtual ~ParameterHandler();

    static double getParameterValue(const std::string& name);
    static void setParameterValue(std::string name, double value);

    virtual void writeParameterFile() = 0;

protected:
    std::map<std::string, Parameter> m_values;
    static ParameterHandler* m_instance;

//    ParameterHandler(const std::string& fileName);
    virtual void initParameters() = 0;
    void loadValuesFromFile(std::ifstream& in);

    std::vector<std::string> tokenizer(std::string& line);
    std::string ignoreEmptyRows(std::ifstream& in);

};

#endif // PARAMETERHANDLER_H
