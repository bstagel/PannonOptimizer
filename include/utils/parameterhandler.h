/**
 * @file parameterhandler.h
 */

#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <globals.h>

#include <map>
#include <vector>
#include <utils/parameter.h>

class ParameterHandler{
    friend class LinalgParameterHandlerTestSuite;
    friend class SimplexParameterHandlerTestSuite;

public:
    virtual ~ParameterHandler();

    inline bool hasParameter(const std::string& name) const {return m_values.count(name)>0;}
    inline double getParameterValue(const std::string& name) const {return m_values.at(name).getValue();}
    inline void setParameterValue(const std::string name, const double value){m_values[name] = Parameter(name,value);}

    virtual void readParameterFile(const char * filename);
    virtual void writeParameterFile() = 0;
    virtual void initParameters() = 0;

    inline std::string getFileName() const {return m_filename;}
    inline void setFileName(std::string filename) {m_filename = filename;}

protected:

    std::string m_filename;

    std::map<std::string, Parameter> m_values;

    void loadValuesFromFile(std::ifstream& in);

    std::vector<std::string> tokenizer(std::string& line);
    std::string ignoreEmptyRows(std::ifstream& in);

};

#endif // PARAMETERHANDLER_H
