/**
 * @file parameterhandler.h
 */

#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <globals.h>

#include <map>
#include <vector>
#include <utils/parameter.h>
#include <utils/entry.h>

class ParameterHandler{
    friend class LinalgParameterHandlerTestSuite;
    friend class SimplexParameterHandlerTestSuite;

public:
    virtual ~ParameterHandler();

    inline bool hasParameter(const std::string& name) const {return m_values.count(name)>0;}
    inline void createParameter(const std::string& name, Entry::ENTRY_TYPE type){
        m_values.insert(std::pair<std::string, Parameter>(name, Parameter(name,type)));
    }

    inline Entry::ENTRY_TYPE getParameterType(const std::string& name) const {
        return m_values.at(name).getEntryType();}
    inline const std::string & getStringParameterValue(const std::string& name) const {
        return *(m_values.at(name).getEntry().m_string);}
    inline const int & getIntegerParameterValue(const std::string& name) const {
        return m_values.at(name).getEntry().m_integer;}
    inline const double & getDoubleParameterValue(const std::string& name) const {
        return m_values.at(name).getEntry().m_double;}
    inline const bool & getBoolParameterValue(const std::string& name) const {
        return m_values.at(name).getEntry().m_bool;}

    inline void setParameterValue(const std::string name, const double value){
        m_values.at(name).setDoubleValue(value);
    }

    inline void setParameterValue(const std::string name, const int value){
        m_values.at(name).setIntegerValue(value);
    }

    inline void setParameterValue(const std::string name, const std::string value){
        m_values.at(name).setStringValue(value);
    }

    inline void setParameterValue(const std::string name, const bool value){
        m_values.at(name).setBoolValue(value);
    }

    virtual void readParameterFile(const char * filename);
    const std::string writeParameter(std::string name);
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
