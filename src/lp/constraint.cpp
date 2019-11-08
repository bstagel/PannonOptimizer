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
 * @file constraint.cpp
 */

#include <lp/constraint.h>

Constraint Constraint::createConstraint(const char * name,
    Numerical::Double lowerBound,
    Numerical::Double upperBound)
{
    return Constraint(lowerBound, upperBound, name);
}

Constraint Constraint::createGreaterTypeConstraint(const char * name,
    Numerical::Double lowerBound)
{
    return Constraint(lowerBound, Numerical::Infinity, name);
}

Constraint Constraint::createLessTypeConstraint(const char * name,
    Numerical::Double upperBound)
{
    return Constraint(-Numerical::Infinity, upperBound, name);
}

Constraint Constraint::createRangeTypeConstraint(const char * name,
    Numerical::Double lowerBound,
    Numerical::Double upperBound)
{
    return Constraint(lowerBound, upperBound, name);
}

Constraint Constraint::createEqualityTypeConstraint(const char * name,
    Numerical::Double value)
{
    return Constraint(value, value, name);
}

Constraint Constraint::createNonBindingTypeConstraint(const char * name)
{
    return Constraint(-Numerical::Infinity, Numerical::Infinity, name);
}

std::ostream & operator<<(std::ostream & os, const Constraint & var)
{
    os << "Name: " << var.getName() << std::endl;
    os << "\tupper bound: " << var.m_upperBound << std::endl;
    os << "\tlower bound: " << var.m_lowerBound << std::endl;
    os << "\ttype: ";
    switch (var.m_type) {
        case Constraint::GREATER_OR_EQUAL:
            os << "greater or equal";
            break;
        case Constraint::LESS_OR_EQUAL:
            os << "less or equal";
            break;
        case Constraint::RANGE:
            os << "range";
            break;
        case Constraint::EQUALITY:
            os << "equality";
            break;
        case Constraint::NON_BINDING:
            os << "non-binding";
            break;
        default:
            os << "unknown type";
    }
    os << std::endl;
    return os;
}

void Constraint::check() const
{
    if (m_lowerBound == Numerical::Infinity) {
        std::ostringstream message;
        message << "Constraint " << getName() << " has invalid lower bound: " <<
            m_lowerBound;
        throw Constraint::InvalidLowerBoundException(*this, message.str());
    }
    if (m_upperBound == -Numerical::Infinity) {
        std::ostringstream message;
        message << "Constraint " << getName() << " has invalid upper bound: " <<
            m_upperBound;
        throw Constraint::InvalidUpperBoundException(*this, message.str());
    }
    if (m_upperBound < m_lowerBound) {
        std::ostringstream message;
        message << "Constraint " << getName() <<
            " has invalid bounds. The upper bound is less than the lower bound: " <<
            m_upperBound << " < " << m_lowerBound;
        throw Constraint::InvalidBoundsException(*this, message.str());
    }
}

Constraint::ConstraintException::ConstraintException(const Constraint & constraint,
    const std::string & message) : PanOptException(message)
{
    m_constraint = new Constraint(constraint);
}

Constraint::ConstraintException::~ConstraintException()
{
    delete m_constraint;
    m_constraint = nullptr;
}

const Constraint * Constraint::ConstraintException::getConstraint() const
{
    return m_constraint;
}

Constraint::InvalidUpperBoundException::InvalidUpperBoundException(const Constraint & constraint,
    const std::string & message) : Constraint::ConstraintException(constraint, message)
{
}

Constraint::InvalidLowerBoundException::InvalidLowerBoundException(const Constraint & constraint,
    const std::string & message) : Constraint::ConstraintException(constraint, message)
{
}

Constraint::InvalidBoundsException::InvalidBoundsException(const Constraint & constraint,
    const std::string & message) : Constraint::ConstraintException(constraint, message)
{
}
