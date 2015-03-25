#ifndef MULTIPLEFLOAT_H
#define MULTIPLEFLOAT_H

#include <gmpxx.h>
#include <iostream>
#include <limits>
#include <cmath>

#define DEFAULT_DEBUG_MODE false

class MultipleFloat {
public:
    MultipleFloat(): m_number(0.0), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) {

    }

    //template<class NUMBER_TYPE>
    //MultipleFloat(const NUMBER_TYPE & value): m_number(value) {}

    MultipleFloat(float value) {
        if (std::isinf(value) || std::isnan(value)) {
            m_number = 0.0;
            m_infinitySymbol = value;
        } else {
            m_number = value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    }

    MultipleFloat(double value) {
        if (std::isinf(value) || std::isnan(value)) {
            m_number = 0.0;
            m_infinitySymbol = value;
        } else {
            m_number = value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    }

    MultipleFloat(long double value) {
        if (std::isinf(value) || std::isnan(value)) {
            m_number = 0.0;
            m_infinitySymbol = value;
        } else {
            m_number = (double)value;
            m_infinitySymbol = 0;
        }
        m_debug = DEFAULT_DEBUG_MODE;
    }

    MultipleFloat(short value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(unsigned short value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(int value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(unsigned int value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(long value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(unsigned long value): m_number(value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(long long value): m_number((long)value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    MultipleFloat(unsigned long long value): m_number((unsigned long)value), m_infinitySymbol(0.0), m_debug(DEFAULT_DEBUG_MODE) { }

    operator double() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_d();
    }

    operator float() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_d();
    }

    operator long() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_si();
    }

    operator unsigned long() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_ui();
    }

    operator int() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_si();
    }

    operator unsigned int() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_ui();
    }

    operator char() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_si();
    }

    operator unsigned char() const {
        if (m_infinitySymbol != 0.0) {
            return m_infinitySymbol;
        }
        return m_number.get_ui();
    }

    bool operator>(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number > number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol > number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol > number.m_number.get_d();
        } else {
            return m_number.get_d() > number.m_infinitySymbol;
        }
    }

    bool operator>=(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number >= number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol >= number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol >= number.m_number.get_d();
        } else {
            return m_number.get_d() >= number.m_infinitySymbol;
        }
    }

    bool operator<(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number < number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol < number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol < number.m_number.get_d();
        } else {
            return m_number.get_d() < number.m_infinitySymbol;
        }
    }

    bool operator<=(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number <= number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol <= number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol <= number.m_number.get_d();
        } else {
            return m_number.get_d() <= number.m_infinitySymbol;
        }
    }

    bool operator==(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number == number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol == number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol == number.m_number.get_d();
        } else {
            return m_number.get_d() == number.m_infinitySymbol;
        }
    }

    bool operator!=(const MultipleFloat & number) const {
        if (m_infinitySymbol == 0.0 && number.m_infinitySymbol == 0.0) {
            return m_number != number.m_number;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol != 0.0) {
            return m_infinitySymbol != number.m_infinitySymbol;
        } else if (m_infinitySymbol != 0.0 && number.m_infinitySymbol == 0.0) {
            return m_infinitySymbol != number.m_number.get_d();
        } else {
            return m_number.get_d() != number.m_infinitySymbol;
        }
    }

    friend bool operator < (float value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (long double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (unsigned char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (unsigned short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (unsigned int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (unsigned long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }

    friend bool operator < (unsigned long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v < number;
    }


    friend bool operator < (const MultipleFloat & number, float value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, double value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, long double value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, char value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, unsigned char value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, short value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, unsigned short value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, int value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, unsigned int value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, long value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, unsigned long value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, long long value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator < (const MultipleFloat & number, unsigned long long value) {
        MultipleFloat v = value;
        return number < v;
    }

    friend bool operator > (float value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (long double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (unsigned char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (unsigned short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (unsigned int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (unsigned long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (unsigned long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v > number;
    }

    friend bool operator > (const MultipleFloat & number, float value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, double value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, long double value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, char value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, unsigned char value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, short value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, unsigned short value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, int value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, unsigned int value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, long value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, unsigned long value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, long long value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator > (const MultipleFloat & number, unsigned long long value) {
        MultipleFloat v = value;
        return number > v;
    }

    friend bool operator >= (const float & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const double & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const long double & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const char & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const unsigned char & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const short & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const unsigned short & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const int & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const unsigned int & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const unsigned long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const long long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const unsigned long long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v >= number;
    }

    friend bool operator >= (const MultipleFloat & number, const float & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const double & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const long double & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const char & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const unsigned char & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const short & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const unsigned short & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const int & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const unsigned int & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const long & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const unsigned long & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const long long & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator >= (const MultipleFloat & number, const unsigned long long & value) {
        MultipleFloat v = value;
        return number >= v;
    }

    friend bool operator <= (const MultipleFloat & number, const float & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const double & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const long double & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const char & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const unsigned char & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const short & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const unsigned short & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const int & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const unsigned int & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const long & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const unsigned long & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const long long & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const MultipleFloat & number, const unsigned long long & value) {
        MultipleFloat v = value;
        return number <= v;
    }

    friend bool operator <= (const float & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const double & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const long double & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const char & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const unsigned char & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const short & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const unsigned short & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const int & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const unsigned int & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const unsigned long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const long long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator <= (const unsigned long long & value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v <= number;
    }

    friend bool operator != (float value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (long double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (unsigned char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (unsigned short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (unsigned int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (unsigned long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (unsigned long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v != number;
    }

    friend bool operator != (const MultipleFloat & number, float value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, double value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, long double value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, char value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, unsigned char value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, short value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, unsigned short value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, int value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, unsigned int value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, long value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, unsigned long value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, long long value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator != (const MultipleFloat & number, unsigned long long value) {
        MultipleFloat v = value;
        return number != v;
    }

    friend bool operator == (float value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (long double value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (unsigned char value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (unsigned short value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (unsigned int value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (unsigned long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (unsigned long long value, const MultipleFloat & number) {
        MultipleFloat v = value;
        return v == number;
    }

    friend bool operator == (const MultipleFloat & number, float value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, double value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, long double value) {
        MultipleFloat v = value;
        return number == v;
    }


    friend bool operator == (const MultipleFloat & number, char value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, unsigned char value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, short value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, unsigned short value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, int value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, unsigned int value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, long value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, unsigned long value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, long long value) {
        MultipleFloat v = value;
        return number == v;
    }

    friend bool operator == (const MultipleFloat & number, unsigned long long value) {
        MultipleFloat v = value;
        return number == v;
    }

    MultipleFloat & operator=(const MultipleFloat & number) {
        if (this == &number) {
            return *this;
        }
        m_number = number.m_number;
        m_infinitySymbol = number.m_infinitySymbol;
        // do not rewrite m_debug!
        return *this;
    }

    MultipleFloat & operator+=(const MultipleFloat & number) {
        m_number += number.m_number;
        m_infinitySymbol += number.m_infinitySymbol;
        return *this;
    }

    MultipleFloat & operator-=(const MultipleFloat & number) {
        m_number -= number.m_number;
        m_infinitySymbol -= number.m_infinitySymbol;
        return *this;
    }

    MultipleFloat & operator*=(const MultipleFloat & number) {
        m_number *= number.m_number;
        m_infinitySymbol *= number.m_infinitySymbol;
        return *this;
    }

    MultipleFloat & operator/=(const MultipleFloat & number) {
        m_number /= number.m_number;
        if (number.m_infinitySymbol == 0.0) {
            m_infinitySymbol /= number.m_number.get_d();
        } else {
            m_infinitySymbol /= number.m_infinitySymbol;
        }
        return *this;
    }

    MultipleFloat operator + (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number + number.m_number;
        result.m_infinitySymbol = m_infinitySymbol + number.m_infinitySymbol;
        result.m_debug = m_debug;
        return result;
    }

    MultipleFloat operator - (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number - number.m_number;
        result.m_infinitySymbol = m_infinitySymbol - number.m_infinitySymbol;
        result.m_debug = m_debug;
        return result;
    }

    MultipleFloat operator * (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number * number.m_number;
        result.m_infinitySymbol = m_infinitySymbol * number.m_infinitySymbol;
        result.m_debug = m_debug;
        return result;
    }

    MultipleFloat operator / (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number / number.m_number;
        if (number.m_infinitySymbol == 0.0) {
            result.m_infinitySymbol = m_infinitySymbol / number.m_number.get_d();
        } else {
            result.m_infinitySymbol = m_infinitySymbol / number.m_infinitySymbol;
        }
        result.m_debug = m_debug;
        return result;
    }

    MultipleFloat operator - () const {
        MultipleFloat result = *this;
        result.m_number = -m_number;
        if (result.m_infinitySymbol != 0.0) {
            result.m_infinitySymbol = -m_infinitySymbol;
        }
        result.m_debug = m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = value * number.m_number.get_d();
        } else {
            result.m_number = value * number.m_number;
            result.m_infinitySymbol = value * number.m_infinitySymbol;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = number.m_number.get_d() * value;
        } else {
            result.m_number = number.m_number * value;
            result.m_infinitySymbol = number.m_infinitySymbol * value;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = value / number.m_number.get_d();
        } else {
            result.m_number = value / number.m_number;
            if (number.m_infinitySymbol != 0.0) {
                result.m_infinitySymbol = value / number.m_infinitySymbol;
            } else {
                result.m_infinitySymbol = value / number.m_number.get_d();
            }
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = number.m_number.get_d() / value;
        } else {
            result.m_number = number.m_number / value;
            result.m_infinitySymbol = number.m_infinitySymbol / value;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = value + number.m_number.get_d();
        } else {
            result.m_number = value + number.m_number;
            result.m_infinitySymbol = number.m_infinitySymbol;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = number.m_number.get_d() + value;
        } else {
            result.m_number = number.m_number + value;
            result.m_infinitySymbol = number.m_infinitySymbol;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = value - number.m_number.get_d();
        } else {
            result.m_number = value - number.m_number;
            result.m_infinitySymbol = - number.m_infinitySymbol;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        if (std::isinf(value) || std::isnan(value)) {
            result.m_number = 0.0;
            result.m_infinitySymbol = number.m_number.get_d() - value;
        } else {
            result.m_number = number.m_number - value;
            result.m_infinitySymbol = number.m_infinitySymbol;
        }
        result.m_debug = number.m_debug;
        return result;
    }

    static MultipleFloat abs(const MultipleFloat & number) {
        MultipleFloat result = number;
        result.m_number = ::abs(number.m_number);
        result.m_infinitySymbol = ::abs(number.m_infinitySymbol);
        return result;
    }

    static MultipleFloat sqrt(const MultipleFloat & number) {
        MultipleFloat result = number;
        result.m_number = ::sqrt(number.m_number);
        result.m_infinitySymbol = ::sqrt(number.m_infinitySymbol);
        return result;
    }

    static MultipleFloat log10(const MultipleFloat & number) {
        MultipleFloat result = number;
        result.m_number = ::log10(number.m_number.get_d());
        result.m_infinitySymbol = ::log10(number.m_infinitySymbol);
        return result;
    }

    static MultipleFloat round(const MultipleFloat & number) {
        MultipleFloat result = number;
        int sign = sgn(number.m_number);
        result.m_number = ::abs(number.m_number);
        if (result.m_number - trunc(result.m_number) < 0.5) {
            result.m_number = trunc(result.m_number);
        } else {
            result.m_number = trunc(result.m_number) + 1.0;
        }
        result.m_number = result.m_number.get_si() * (sign == 1 ? -1 : 1);
        return result;
    }

    friend std::ostream & operator<<(std::ostream & os, const MultipleFloat & number) {
        //os << "(" << number.m_number << "; " << number.m_infinitySymbol << ")";
        if (number.m_infinitySymbol == 0.0) {
            os << number.m_number;
        } else {
            os << number.m_infinitySymbol;
        }
        return os;
    }

    std::string getString() const {
        mp_exp_t exp;
        if (m_infinitySymbol == 0.0) {
            return m_number.get_str(exp);
        } else {
            return std::to_string(m_infinitySymbol);
        }
    }

    static MultipleFloat pow(const MultipleFloat & x, const MultipleFloat & y) {
        MultipleFloat result;
        result = pow((double)x, (double)y);
        return result;
    }

    void setDebugMode(bool flag) {
        m_debug = flag;
    }

private:

    mpf_class m_number;
    double m_infinitySymbol;
    bool m_debug;

};

namespace std {
    std::string to_string(const MultipleFloat & mf);
}

/*namespace std {
template<>
  struct is_floating_point<MultipleFloat>
  //: public integral_constant<bool, (__is_floating_point_helper<typename
    //                remove_cv<_Tp>::type>::value)>
  {
      static constexpr bool value = true;
      typedef integral_constant<bool, value> type;
      constexpr operator bool() { return value; }
  };

  inline string
  to_string(MultipleFloat __val)
  {
      return __val.getString();
  }
}*/

#endif // MULTIPLEFLOAT_H
