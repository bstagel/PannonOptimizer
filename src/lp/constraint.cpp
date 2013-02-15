#include <lp/constraint.h>

Constraint Constraint::createGreaterTypeConstraint(const char * name,
    Numerical::Double lowerBound)
{
    return Constraint(lowerBound, infinity, name);
}

Constraint Constraint::createLessTypeConstraint(const char * name,
    Numerical::Double upperBound)
{
    return Constraint(-infinity, upperBound, name);
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
    return Constraint(-infinity, infinity, name);
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
    }
    os << std::endl;
    return os;
}

