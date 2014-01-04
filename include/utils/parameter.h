/**
 * @file parameter.h
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <globals.h>
#include <string>
#include <ostream>

#include <utils/exceptions.h>
#include <utils/entry.h>

class Parameter {
public:
    Parameter(const std::string& name, const Entry::ENTRY_TYPE type) :
        m_name(name), m_type(type) {
        if(m_type == Entry::STRING){
            m_entry.m_string = new std::string();
        }
    }

    ~Parameter() {
        if(m_type == Entry::STRING){
            delete m_entry.m_string;
        }
    }

    const std::string& getName() const { return m_name; }
    const Entry & getEntry() const { return m_entry; }
    Entry::ENTRY_TYPE getEntryType() const { return m_type; }
    void setStringValue(const std::string value) throw (ParameterException) {
        if(m_type != Entry::STRING){
            throw ParameterException("Invalid parameter value");
        }
        *(m_entry.m_string) = value;
    }
    void setIntegerValue(const int value) throw (ParameterException) {
        if(m_type != Entry::INTEGER){
            throw ParameterException("Invalid parameter value");
        }
        m_entry.m_integer = value;
    }
    void setDoubleValue(const double value) throw (ParameterException) {
        if(m_type != Entry::DOUBLE){
            throw ParameterException("Invalid parameter value");
        }
        m_entry.m_double = value;
    }
    void setBoolValue(const bool value) throw (ParameterException) {
        if(m_type != Entry::BOOL){
            throw ParameterException("Invalid parameter value");
        }
        m_entry.m_bool = value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Parameter& param){
        os << param.m_name << " = ";
        if(param.m_type == Entry::STRING){
            os << param.m_entry.m_string << std::endl;
        } else if (param.m_type == Entry::INTEGER){
            os << param.m_entry.m_integer << std::endl;
        } else if (param.m_type == Entry::DOUBLE){
            os << param.m_entry.m_double << std::endl;
        } else if (param.m_type == Entry::BOOL){
            os << param.m_entry.m_bool << std::endl;
        }
        return os;
    }

private:
    std::string m_name;
    Entry m_entry;
    Entry::ENTRY_TYPE m_type;
};

#endif /* PARAMETER_H_ */
