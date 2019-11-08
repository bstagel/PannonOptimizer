//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file variable.cpp
 */

#include <lp/variable.h>
#include <sstream>

Variable Variable::createVariable(const char * name,
    Numerical::Double lowerBound,
    Numerical::Double upperBound)
{
    return Variable(lowerBound, upperBound, name);
}

Variable Variable::createPlusTypeVariable(const char * name,
    Numerical::Double lowerBound)
{
    return Variable(lowerBound, Numerical::Infinity, name);
}

Variable Variable::createMinusTypeVariable(const char * name,
    Numerical::Double upperBound)
{
    return Variable(-Numerical::Infinity, upperBound, name);
}

Variable Variable::createBoundedTypeVariable(const char * name,
    Numerical::Double lowerBound,
    Numerical::Double upperBound)
{
    return Variable(lowerBound, upperBound, name);
}

Variable Variable::createFixedTypeVariable(const char * name,
    Numerical::Double value)
{
    return Variable(value, value, name);
}

Variable Variable::createFreeTypeVariable(const char * name)
{
    return Variable(-Numerical::Infinity, Numerical::Infinity, name);
}

std::ostream & operator<<(std::ostream & os, const Variable & var)
{
    os << "Name: " << var.getName() << std::endl;
    os << "\tupper bound: " << var.m_upperBound << std::endl;
    os << "\tlower bound: " << var.m_lowerBound << std::endl;
    os << "\ttype: ";
    switch (var.m_type) {
        case Variable::FIXED:
            os << "fixed";
            break;
        case Variable::PLUS:
            os << "plus";
            break;
        case Variable::MINUS:
            os << "minus";
            break;
        case Variable::BOUNDED:
            os << "bounded";
            break;
        case Variable::FREE:
            os << "free";
            break;
        default:
            os << "unknown type";
    }
    os << std::endl;
    return os;
}

void Variable::check() const
{
    if (m_lowerBound == Numerical::Infinity) {
        std::ostringstream message;
        message << "Variable " << getName() << " has invalid lower bound: " <<
            m_lowerBound;
        throw Variable::InvalidLowerBoundException(*this, message.str());
    }
    if (m_upperBound == -Numerical::Infinity) {
        std::ostringstream message;
        message << "Variable " << getName() << " has invalid upper bound: " <<
            m_upperBound;
        throw Variable::InvalidUpperBoundException(*this, message.str());
    }
    if (m_upperBound < m_lowerBound) {
        std::ostringstream message;
        message << "Variable " << getName() <<
            " has invalid bounds. The upper bound is less than the lower bound: " <<
            m_upperBound << " < " << m_lowerBound;
        throw Variable::InvalidBoundsException(*this, message.str());
    }
}

Variable::VariableException::VariableException(const Variable & variable,
    const std::string & message) : PanOptException(message)
{
    m_variable = new Variable(variable);
}

Variable::VariableException::~VariableException()
{
    delete m_variable;
    m_variable = nullptr;
}

const Variable * Variable::VariableException::getVariable() const
{
    return m_variable;
}

Variable::InvalidUpperBoundException::InvalidUpperBoundException(const Variable & variable,
    const std::string & message) : Variable::VariableException(variable, message)
{
}

Variable::InvalidLowerBoundException::InvalidLowerBoundException(const Variable & variable,
    const std::string & message) : Variable::VariableException(variable, message)
{
}

Variable::InvalidBoundsException::InvalidBoundsException(const Variable & variable,
    const std::string & message) : Variable::VariableException(variable, message)
{
}
