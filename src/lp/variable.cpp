#include <lp/variable.h>

Variable Variable::createPlusTypeVariable(const char * name,
    Numerical::Double value,
    Numerical::Double lowerBound)
{
    return Variable(lowerBound, infinity, value, name);
}

Variable Variable::createMinusTypeVariable(const char * name,
    Numerical::Double value,
    Numerical::Double upperBound)
{
    return Variable(-infinity, upperBound, value, name);
}

Variable Variable::createBoundedTypeVariable(const char * name,
    Numerical::Double value,
    Numerical::Double lowerBound,
    Numerical::Double upperBound)
{
    return Variable(lowerBound, upperBound, value, name);
}

Variable Variable::createFixedTypeVariable(const char * name,
    Numerical::Double value)
{
    return Variable(value, value, value, name);
}

Variable Variable::createFreeTypeVariable(const char * name,
    Numerical::Double value)
{
    return Variable(-infinity, infinity, value, name);
}

std::ostream & operator<<(std::ostream & os, const Variable & var)
{
    os << "Name: " << var.getName() << std::endl;
    os << "\tvalue : " << var.m_value << std::endl;
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
    }
    os << std::endl;
    return os;
}
