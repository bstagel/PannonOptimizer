/**
 * @file parameter.h
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include <ostream>

class Parameter {
public:
    Parameter(const std::string& name="", double value=0, int group=0) :
        m_name(name), m_value(value), m_group(group) {

    }

    const std::string& getName() const { return m_name; }
    double getValue() const { return m_value; }
    int getGroup() const { return m_group; }
    void setValue(double const value) { m_value = value; }

    friend std::ostream& operator<<(std::ostream& os, const Parameter& param){
        os << "Group " <<param.m_group << ": " << param.m_name << " = " << param.m_value << std::endl;
        return os;
    }

private:
    std::string m_name;
	double m_value;
    int m_group;
};

#endif /* PARAMETER_H_ */
