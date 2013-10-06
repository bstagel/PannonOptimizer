/**
 * @file parameter.h
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <globals.h>
#include <string>
#include <ostream>

class Parameter {
public:
    Parameter(const std::string& name="", double value=0) :
        m_name(name), m_value(value){

    }

    const std::string& getName() const { return m_name; }
    double getValue() const { return m_value; }
    void setValue(double const value) { m_value = value; }

    friend std::ostream& operator<<(std::ostream& os, const Parameter& param){
        os << param.m_name << " = " << param.m_value << std::endl;
        return os;
    }

private:
    std::string m_name;
    double m_value;
};

#endif /* PARAMETER_H_ */
