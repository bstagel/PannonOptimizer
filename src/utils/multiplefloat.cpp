#include <utils/multiplefloat.h>

#if DOUBLE_TYPE == DOUBLE_GMP

#define GMP_SETTINGS_FLOAT    0
#define GMP_SETTINGS_RATIONAL 1
#define GMP_SETTINGS GMP_SETTINGS_RATIONAL


#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
    #define M_NUMBER m_numberF
    GMP_TYPE_ENUM GMP_TYPE = GMP_FLOAT;
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
    #define M_NUMBER m_numberR
    GMP_TYPE_ENUM GMP_TYPE = GMP_RATIONAL;
#endif

bool ENABLE_MULTIPLE_PRECISION = true;

namespace std {
    std::string to_string(const MultipleFloat & mf) {
        return mf.getString();
    }
}

void MultipleFloat::check() const {
    /*if (std::isnan(m_infinitySymbol)) {
        std::cout << m_infinitySymbol << std::endl;
        int * ptr = nullptr;
        *ptr = 666;
    }
    if (m_infinitySymbol != 0.0 && !std::isinf(m_infinitySymbol)) {
        std::cout << m_infinitySymbol << std::endl;
        std::cin.get();
    }

    if (!std::isinf(m_infinitySymbol) && !std::isnan(m_infinitySymbol) && m_infinitySymbol != 0.0) {
        std::cout << "hiba: " << m_infinitySymbol << std::endl;
        std::cin.get();
        // int * ptr = nullptr;
        // *ptr = 666;
    }*/
}

MultipleFloat::MultipleFloat(): M_NUMBER(0.0), m_value(0.0), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {
    check();
}

MultipleFloat::MultipleFloat(float value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (std::isinf(value) || std::isnan(value)) {
            M_NUMBER = 0.0;
            m_infinitySymbol = value;
        } else {
            M_NUMBER = value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    } else {
        m_value = value;
    }
    check();
}

MultipleFloat::MultipleFloat(double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (std::isinf(value) || std::isnan(value)) {
            M_NUMBER = 0.0;
            m_infinitySymbol = value;
        } else {
            M_NUMBER = value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    } else {
        m_value = value;
    }
    check();
}

MultipleFloat::MultipleFloat(long double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (std::isinf(value) || std::isnan(value)) {
            M_NUMBER = 0.0;
            m_infinitySymbol = value;
        } else {
            M_NUMBER = (double)value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    } else {
        m_value = value;
    }
    check();
}

MultipleFloat::MultipleFloat(short value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(unsigned short value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(int value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(unsigned int value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(long value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(unsigned long value): M_NUMBER(value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(long long value): M_NUMBER((long)value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

MultipleFloat::MultipleFloat(unsigned long long value): M_NUMBER((unsigned long)value), m_value(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {check(); }

bool operator <(const MultipleFloat &number, unsigned long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator >(const MultipleFloat &number, unsigned long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >=(const MultipleFloat &number, const unsigned long long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator <=(const unsigned long long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator !=(const MultipleFloat &number, unsigned long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator ==(const MultipleFloat &number, unsigned long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

MultipleFloat &MultipleFloat::operator=(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (this == &number) {
            return *this;
        }
        M_NUMBER = number.M_NUMBER;
        m_infinitySymbol = number.m_infinitySymbol;
        // do not rewrite m_debug!
    } else {
        m_value = number.m_value;
    }
    check();
    return *this;

}

MultipleFloat &MultipleFloat::operator+=(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        *this = *this + number;
        //M_NUMBER += number.M_NUMBER;
        //m_infinitySymbol += number.m_infinitySymbol;
    } else {
        m_value += number.m_value;
    }
    check();
    return *this;
}

MultipleFloat &MultipleFloat::operator-=(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        *this = *this - number;
        //M_NUMBER -= number.M_NUMBER;
        //m_infinitySymbol -= number.m_infinitySymbol;
    } else {
        m_value -= number.m_value;
    }
    check();
    return *this;
}

MultipleFloat &MultipleFloat::operator*=(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        *this = *this * number;
        //M_NUMBER *= number.M_NUMBER;
        //m_infinitySymbol *= number.m_infinitySymbol;
    } else {
        m_value *= number.m_value;
    }
    check();
    return *this;
}

MultipleFloat &MultipleFloat::operator/=(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        *this = *this / number;
        /*M_NUMBER /= number.M_NUMBER;
        if (number.m_infinitySymbol == 0.0) {
            m_infinitySymbol /= number.M_NUMBER.get_d();
        } else {
            m_infinitySymbol /= number.m_infinitySymbol;
        }*/
    } else {
        m_value /= number.m_value;
    }
    check();
    return *this;
}

MultipleFloat MultipleFloat::abs(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result = number;
        if (result.M_NUMBER < 0.0) {
            result.M_NUMBER = -result.M_NUMBER;
        }
        result.m_infinitySymbol = ::abs(number.m_infinitySymbol);
        result.check();
        return result;
    } else {
        MultipleFloat result(::fabs(number.m_value));
        return result;
    }
}

MultipleFloat MultipleFloat::sqrt(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result = number;
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        result.M_NUMBER = ::sqrt(number.M_NUMBER);
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        result.M_NUMBER = ::sqrt(number.M_NUMBER.get_d());
#endif
        result.m_infinitySymbol = ::sqrt(number.m_infinitySymbol);
        result.check();
        return result;
    } else {
        MultipleFloat result(::sqrt(number.m_value));
        return result;
    }
}

MultipleFloat MultipleFloat::log10(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result = number;
        result.M_NUMBER = ::log10(number.M_NUMBER.get_d());
        result.m_infinitySymbol = ::log10(number.m_infinitySymbol);
        result.check();
        return result;
    } else {
        MultipleFloat result(::log10(number.m_value));
        return result;
    }
}

MultipleFloat MultipleFloat::round(const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result = number;
        int sign = sgn(number.M_NUMBER);
        result.M_NUMBER = ::abs(number.M_NUMBER);
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        if (result.M_NUMBER - trunc(result.M_NUMBER) < 0.5) {
            result.M_NUMBER = trunc(result.M_NUMBER);
        } else {
            result.M_NUMBER = trunc(result.M_NUMBER) + 1.0;
        }
        result.M_NUMBER = result.M_NUMBER.get_si() * (sign == 1 ? -1 : 1);
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        if (result.M_NUMBER - trunc(result.M_NUMBER.get_d()) < 0.5) {
            result.M_NUMBER = trunc(result.M_NUMBER.get_d());
        } else {
            result.M_NUMBER = trunc(result.M_NUMBER.get_d()) + 1.0;
        }
        result.M_NUMBER = result.M_NUMBER.get_num().get_si() * (sign == 1 ? -1 : 1);
#endif
        result.check();
        return result;
    } else {
        MultipleFloat result(::round(number.m_value));
        return result;
    }
}

std::ostream & operator<<(std::ostream &os, const MultipleFloat &number) {
    //os << "(" << number.M_NUMBER << "; " << number.m_infinitySymbol << ")";
    if (ENABLE_MULTIPLE_PRECISION) {
        if (number.m_infinitySymbol == 0.0) {
            os << number.M_NUMBER.get_d();
        } else {
            os << number.m_infinitySymbol;
        }
    } else {
        os << number.m_value;
    }
    return os;
}

std::string MultipleFloat::getString() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        mp_exp_t exp;
        if (m_infinitySymbol == 0.0) {
            return M_NUMBER.get_str(exp);
        } else {
            return std::to_string(m_infinitySymbol);
        }
    } else {
        return std::to_string(m_value);
    }
}

MultipleFloat MultipleFloat::pow(const MultipleFloat &x, const MultipleFloat &y) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result;
        result = pow(x.M_NUMBER.get_d(), y.M_NUMBER.get_d());
        result.check();
        return result;
    } else {
        MultipleFloat result( pow(x.m_value, y.m_value) );
        return result;
    }
}

double MultipleFloat::getDouble() const {
    if (m_infinitySymbol == 0.0) {
        return M_NUMBER.get_d();
    }
    return m_infinitySymbol;
}

void MultipleFloat::setDebugMode(bool flag) {
    m_debug = flag;
}

MultipleFloat MultipleFloat::operator -() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result = *this;
        result.M_NUMBER = -M_NUMBER;
        if (result.m_infinitySymbol != 0.0) {
            result.m_infinitySymbol = -m_infinitySymbol;
        }
        result.m_debug = m_debug;
        result.check();
        return result;
    } else {
        MultipleFloat result(-m_value);
        return result;
    }

}

MultipleFloat MultipleFloat::operator /(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result;
        bool normal1 = m_infinitySymbol == 0.0;
        bool normal2 = number.m_infinitySymbol == 0.0;
        if (normal1) {
            if (normal2) {
                if (number.M_NUMBER == 0.0) {
                    result.M_NUMBER = 0.0;
                    result.m_infinitySymbol = M_NUMBER.get_d() / number.M_NUMBER.get_d();
                } else {
                    result.M_NUMBER = M_NUMBER / number.M_NUMBER;
                    result.m_infinitySymbol = 0.0;
                }
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = M_NUMBER.get_d() / number.m_infinitySymbol;
            }
        } else {
            if (normal2) {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol / number.M_NUMBER.get_d();
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol / number.m_infinitySymbol;
            }
        }
        result.m_debug = m_debug;
        result.check();
        return result;
    } else {
        MultipleFloat result(m_value / number.m_value);
        result.check();
        return result;
    }

}

MultipleFloat MultipleFloat::operator *(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result;
        bool normal1 = m_infinitySymbol == 0.0;
        bool normal2 = number.m_infinitySymbol == 0.0;
        if (normal1) {
            if (normal2) {
                result.M_NUMBER = M_NUMBER * number.M_NUMBER;
                result.m_infinitySymbol = 0.0;
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = M_NUMBER.get_d() * number.m_infinitySymbol;
            }
        } else {
            if (normal2) {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol * number.M_NUMBER.get_d();
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol * number.m_infinitySymbol;
            }
        }
        result.m_debug = m_debug;
        if (std::isnan(result.m_infinitySymbol)) {
            std::cout << m_infinitySymbol << " " << number.m_infinitySymbol << std::endl;
        }
        result.check();
        return result;
    } else {
        MultipleFloat result(m_value * number.m_value);
        result.check();
        return result;
    }

}

MultipleFloat MultipleFloat::operator -(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result;
        bool normal1 = m_infinitySymbol == 0.0;
        bool normal2 = number.m_infinitySymbol == 0.0;
        if (normal1) {
            if (normal2) {
                result.M_NUMBER = M_NUMBER - number.M_NUMBER;
                result.m_infinitySymbol = 0.0;
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = M_NUMBER.get_d() - number.m_infinitySymbol;
            }
        } else {
            if (normal2) {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol - number.M_NUMBER.get_d();
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol - number.m_infinitySymbol;
            }
        }
        result.m_debug = m_debug;
        result.check();
        return result;
    } else {
        MultipleFloat result(m_value - number.m_value);
        result.check();
        return result;
    }

}

MultipleFloat MultipleFloat::operator +(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat result;

        bool normal1 = m_infinitySymbol == 0.0;
        bool normal2 = number.m_infinitySymbol == 0.0;
        if (normal1) {
            if (normal2) {
                result.M_NUMBER = M_NUMBER + number.M_NUMBER;
                result.m_infinitySymbol = 0.0;
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = M_NUMBER.get_d() + number.m_infinitySymbol;
            }
        } else {
            if (normal2) {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol + number.M_NUMBER.get_d();
            } else {
                result.M_NUMBER = 0.0;
                result.m_infinitySymbol = m_infinitySymbol + number.m_infinitySymbol;
            }
        }
        result.m_debug = m_debug;
        result.check();
        return result;
    } else {
        MultipleFloat result(m_value + number.m_value);
        result.check();
        return result;
    }


}

bool operator ==(const MultipleFloat &number, long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, unsigned long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, unsigned int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, unsigned short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, unsigned char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, long double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(const MultipleFloat &number, float value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number == v;
    } else {
        return number.m_value == value;
    }
}

bool operator ==(unsigned long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(unsigned long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(unsigned int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(unsigned short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(unsigned char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(long double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator ==(float value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v == number;
    } else {
        return value == number.m_value;
    }
}

bool operator !=(const MultipleFloat &number, long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, unsigned long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, unsigned int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, unsigned short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, unsigned char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, long double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(const MultipleFloat &number, float value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number != v;
    } else {
        return number.m_value != value;
    }
}

bool operator !=(unsigned long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(unsigned long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(unsigned int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(unsigned short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(unsigned char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(long double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator !=(float value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v != number;
    } else {
        return value != number.m_value;
    }
}

bool operator <=(const long long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const unsigned long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const unsigned int &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const int &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const unsigned short &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const short &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const unsigned char &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const char &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const long double &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const double &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const float &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v <= number;
    } else {
        return value <= number.m_value;
    }
}

bool operator <=(const MultipleFloat &number, const unsigned long long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const long long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const unsigned long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const unsigned int &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const int &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const unsigned short &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const short &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const unsigned char &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const char &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const long double &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const double &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator <=(const MultipleFloat &number, const float &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number <= v;
    } else {
        return number.m_value <= value;
    }
}

bool operator >=(const MultipleFloat &number, const long long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const unsigned long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const long &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const unsigned int &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const int &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const unsigned short &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const short &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const unsigned char &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const char &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const long double &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const double &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const MultipleFloat &number, const float &value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number >= v;
    } else {
        return number.m_value >= value;
    }
}

bool operator >=(const unsigned long long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const long long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const unsigned long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const long &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const unsigned int &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const int &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const unsigned short &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const short &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const unsigned char &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const char &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const long double &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const double &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >=(const float &value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v >= number;
    } else {
        return value >= number.m_value;
    }
}

bool operator >(const MultipleFloat &number, long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, unsigned long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, unsigned int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, unsigned short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, unsigned char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, long double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(const MultipleFloat &number, float value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number > v;
    } else {
        return number.m_value > value;
    }
}

bool operator >(unsigned long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(unsigned long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(unsigned int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(unsigned short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(unsigned char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(long double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator >(float value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v > number;
    } else {
        return value > number.m_value;
    }
}

bool operator <(const MultipleFloat &number, long long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, unsigned long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, long value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, unsigned int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, int value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, unsigned short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, short value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, unsigned char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, char value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, long double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, double value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(const MultipleFloat &number, float value) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return number < v;
    } else {
        return number.m_value < value;
    }
}

bool operator <(unsigned long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(long long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(unsigned long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(long value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(unsigned int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(int value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(unsigned short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(short value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(unsigned char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(char value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(long double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(double value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool operator <(float value, const MultipleFloat &number) {
    if (ENABLE_MULTIPLE_PRECISION) {
        MultipleFloat v = value;
        return v < number;
    } else {
        return value < number.m_value;
    }
}

bool MultipleFloat::operator!=(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER != number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol != number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol != number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() != number.m_infinitySymbol;
        }
    } else {
        return m_value != number.m_value;
    }
}

bool MultipleFloat::operator==(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER == number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol == number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol == number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() == number.m_infinitySymbol;
        }
    } else {
        return m_value == number.m_value;
    }
}

bool MultipleFloat::operator<=(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER <= number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol <= number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol <= number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() <= number.m_infinitySymbol;
        }
    } else {
        return m_value <= number.m_value;
    }
}

bool MultipleFloat::operator<(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER < number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol < number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol < number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() < number.m_infinitySymbol;
        }
    } else {
        return m_value < number.m_value;
    }
}

bool MultipleFloat::operator>=(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER >= number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol >= number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol >= number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() >= number.m_infinitySymbol;
        }
    } else {
        return m_value >= number.m_value;
    }
}

bool MultipleFloat::operator>(const MultipleFloat &number) const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return M_NUMBER > number.M_NUMBER;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol > number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol > number.M_NUMBER.get_d();
        } else {
            return M_NUMBER.get_d() > number.m_infinitySymbol;
        }
    } else {
        return m_value > number.m_value;
    }
}

MultipleFloat::operator unsigned char() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_ui();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif
    } else {
        return m_value;
    }
}

MultipleFloat::operator char() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_si();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif
    } else {
        return m_value;
    }
}

MultipleFloat::operator unsigned int() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_ui();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif
    } else {
        return m_value;
    }
}

MultipleFloat::operator int() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_si();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif
    } else {
        return m_value;
    }
}

MultipleFloat::operator unsigned long() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_ui();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif
    } else {
        return m_value;
    }
}

MultipleFloat::operator long() const {
    if (ENABLE_MULTIPLE_PRECISION) {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
#if GMP_SETTINGS == GMP_SETTINGS_FLOAT
        return M_NUMBER.get_si();
#endif
#if GMP_SETTINGS == GMP_SETTINGS_RATIONAL
        return M_NUMBER.get_d();
#endif

    } else {
        return m_value;
    }
}

#endif //#if DOUBLE_TYPE == DOUBLE_GMP
