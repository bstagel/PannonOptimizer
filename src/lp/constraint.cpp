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

void Constraint::check() const
{
    if (m_lowerBound == infinity) {
        std::ostringstream message;
        message << "Constraint " << getName() << " has invalid lower bound: " <<
            m_lowerBound;
        throw Constraint::InvalidLowerBoundException(*this, message.str());
    }
    if (m_upperBound == -infinity) {
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
    const std::string & message) : Exception(message)
{
    m_constraint = new Constraint(constraint);
}

Constraint::ConstraintException::~ConstraintException()
{
    delete m_constraint;
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
