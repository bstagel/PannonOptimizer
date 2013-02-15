/**
 * @file numerical.h
 * @author Mark Korondi <korondi@dcs.uni-pannon.hu>
 * @date 26/02/2010
 */

#ifndef _NUMERICAL_H_
#define _NUMERICAL_H_

#ifdef USE_MULTIPLE_PRECISION
#include <gmpxx.h>
#endif
#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>
#include "../debug.h"

#define infinity (std::numeric_limits<double>::infinity())

class Numerical
{
public:

    class Summarizer
    {
        double m_negpos[2];

        union Number
        {
            double m_num;
            unsigned long long int m_bits;
        } m_number;

    public:

        Summarizer()
        {
            m_negpos[0] = 0.0;
            m_negpos[1] = 0.0;
        }

        inline void add(const double & v)
        {
            m_number.m_num = v;
            *(m_negpos + ((m_number.m_bits & 0x8000000000000000LL) >> 63)) += v;
            /*if (v < 0.0) {
                m_negpos[0] += v;
            } else {
                m_negpos[1] += v;
            }*/
        }

        inline double getResult() const
        {
            return m_negpos[0] + m_negpos[1];
        }

        inline void clear()
        {
            m_negpos[0] = 0.0;
            m_negpos[1] = 0.0;
        }
        
        inline const double * getData() const {
            return m_negpos;
        }
    };


#ifdef USE_MULTIPLE_PRECISION

    // TODO: kell <primitiv> op <Double> operator is

    class Double
    {
        friend class Numerical;

        enum STATE
        {
            FINITE, PLUS_INFINITY, MINUS_INFINITY, NOT_A_NUMBER
        };
        mpq_t m_data;
        double m_doubleData;
        STATE m_state;
        static unsigned long long int sm_addCount;
        static unsigned long long int sm_subCount;
        static unsigned long long int sm_mulCount;
        static unsigned long long int sm_divCount;

        static void setNan(Double & v)
        {
            mpq_clear(v.m_data);
            mpq_init(v.m_data);
            v.m_state = NOT_A_NUMBER;
            v.m_doubleData = (0.0 / 0.0);
        }

        static void add(Double & result, const Double & v1, const Double & v2)
        {
            double d1 = mpq_get_d(v1.m_data);
            double d2 = mpq_get_d(v2.m_data);
            double d3 = v1.m_doubleData;
            double d4 = v2.m_doubleData;

            check(v1);
            check(v2);
            sm_addCount++;
            result.m_doubleData = v1.m_doubleData + v2.m_doubleData;
            if (v1.m_state == FINITE && v2.m_state == FINITE) {
                mpq_add(result.m_data, v1.m_data, v2.m_data);
            } else if ((v1.m_state == PLUS_INFINITY && v2.m_state == MINUS_INFINITY) ||
                    (v1.m_state == MINUS_INFINITY && v2.m_state == PLUS_INFINITY)) {
                // TODO: exception
                setNan(result);
            } else if ((v1.m_state == PLUS_INFINITY && v2.m_state != MINUS_INFINITY) ||
                    (v1.m_state != MINUS_INFINITY && v2.m_state == PLUS_INFINITY)) {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = PLUS_INFINITY;
            } else if ((v1.m_state == MINUS_INFINITY && v2.m_state != PLUS_INFINITY) ||
                    (v1.m_state != PLUS_INFINITY && v2.m_state == MINUS_INFINITY)) {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = MINUS_INFINITY;
            }
            check(result, d1, d2, d3, d4);
        }

        static void sub(Double & result, const Double & v1, const Double & v2)
        {
            sm_subCount++;
            result.m_doubleData = v1.m_doubleData - v2.m_doubleData;
            if (v1.m_state == FINITE && v2.m_state == FINITE) {
                mpq_sub(result.m_data, v1.m_data, v2.m_data);
            } else if ((v1.m_state == PLUS_INFINITY && v2.m_state == PLUS_INFINITY) ||
                    (v1.m_state == MINUS_INFINITY && v2.m_state == MINUS_INFINITY)) {
                // TODO: exception
                setNan(result);
            } else if ((v1.m_state == FINITE && v2.m_state == MINUS_INFINITY) ||
                    (v1.m_state == PLUS_INFINITY && v2.m_state == FINITE) ||
                    (v1.m_state == PLUS_INFINITY && v2.m_state == MINUS_INFINITY)) {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = PLUS_INFINITY;
            } else if ((v1.m_state == FINITE && v2.m_state == PLUS_INFINITY) ||
                    (v1.m_state == MINUS_INFINITY && v2.m_state == FINITE) ||
                    (v1.m_state == MINUS_INFINITY && v2.m_state == PLUS_INFINITY)) {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = MINUS_INFINITY;
            }
            check(result);
        }

        static void mul(Double & result, const Double & v1, const Double & v2)
        {
            sm_mulCount++;
            result.m_doubleData = v1.m_doubleData * v2.m_doubleData;
            if (v1.m_state == FINITE && v2.m_state == FINITE) {
                mpq_mul(result.m_data, v1.m_data, v2.m_data);
            } else if (v1.m_state == FINITE || v2.m_state == FINITE) {
                int sign = v1.m_state == FINITE ? mpq_sgn(v1.m_data) : mpq_sgn(v2.m_data);
                if (sign == 0) {
                    setNan(result);
                    // TODO: exeption dobas, +- vegtelent nem szorozhatunk 0-val
                }
                mpq_clear(result.m_data);
                mpq_init(result.m_data);

                if (v2.m_state == PLUS_INFINITY || v1.m_state == PLUS_INFINITY) {
                    if (sign == 1) {
                        result.m_state = PLUS_INFINITY;
                    } else if (sign == -1) {
                        result.m_state = MINUS_INFINITY;
                    }
                } else if (v2.m_state == MINUS_INFINITY || v1.m_state == MINUS_INFINITY) {
                    if (sign == 1) {
                        result.m_state = MINUS_INFINITY;
                    } else if (sign == -1) {
                        result.m_state = PLUS_INFINITY;
                    }
                }
            } else if (v1.m_state == v2.m_state) {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = PLUS_INFINITY;
            } else {
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = MINUS_INFINITY;
            }
            check(result);
        }

        static void div(Double & result, const Double & v1, const Double & v2)
        {
            sm_divCount++;
            result.m_doubleData = v1.m_doubleData / v2.m_doubleData;
            if (v1.m_state == FINITE && v2.m_state == FINITE) {
                if (mpq_sgn(v2.m_data) == 0) {
                    if (mpq_sgn(v1.m_data) == 0) {
                        setNan(result);
                        // TODO: exception dobas
                    } else {
                        result.m_state = mpq_sgn(v1.m_data) == 1 ? PLUS_INFINITY : MINUS_INFINITY;
                        mpq_clear(result.m_data);
                        mpq_init(result.m_data);
                    }
                } else {
                    mpq_div(result.m_data, v1.m_data, v2.m_data);
                }
            } else if (v1.m_state != FINITE && v2.m_state != FINITE) {
                setNan(result);
                // TODO: exception dobas
            } else if (v1.m_state != FINITE) { // vegtelent osztok vegessel
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                result.m_state = v1.m_state;

            } else { // vegest osztok vegtelennel                
                int sign = mpq_sgn(v1.m_data);
                sign /= v2.m_state == PLUS_INFINITY ? 1 : -1;
                mpq_clear(result.m_data);
                mpq_init(result.m_data);
                mpq_set_d(result.m_data, sign == 1 ? 0.0 : -0.0);
            }
            check(result);
        }

        static int compare(const Double & v1, const Double & v2)
        {
            check(v1);
            check(v2);
            if (v1.m_state == FINITE && v2.m_state == FINITE) {
                return mpq_cmp(v1.m_data, v2.m_data);
            } else if (v1.m_state == v2.m_state) {
                return 0;
            } else if (v1.m_state == MINUS_INFINITY && v2.m_state == PLUS_INFINITY) {
                return -1;
            } else if (v1.m_state == PLUS_INFINITY && v2.m_state == MINUS_INFINITY) {
                return 1;
            } else if (v1.m_state == FINITE) {
                if (v2.m_state == PLUS_INFINITY) {
                    return -1;
                } else {
                    return 1;
                }
            } else {
                if (v1.m_state == PLUS_INFINITY) {
                    return 1;
                } else {
                    return -1;
                }
            }
        }

    public:

        static void check(const Double & v, double d1 = 0.0, double d2 = 0.0, double d3 = 0.0, double d4 = 0.0)
        {

        }

        inline Double()
        {
            m_state = FINITE;
            mpq_init(m_data);
            m_doubleData = 0;
            check(*this);
        }

        inline Double(const Double & orig)
        {
            m_state = orig.m_state;
            mpq_init(m_data);
            mpq_set(m_data, orig.m_data);
            m_doubleData = orig.m_doubleData;
            check(*this);
        }

        inline Double(double orig)
        {
            m_doubleData = orig;
            if (orig == infinity) {
                mpq_init(m_data);
                m_state = PLUS_INFINITY;
                check(*this);
                return;
            } else if (orig == -infinity) {
                mpq_init(m_data);
                m_state = MINUS_INFINITY;
                check(*this);
                return;
            }
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_d(m_data, orig);
            check(*this);
        }

        inline Double(unsigned long long int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_ui(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double(signed long long int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_si(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double(unsigned long int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_ui(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double(signed long int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_si(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double(unsigned int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_ui(m_data, (unsigned long int) orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double(signed int orig)
        {
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_si(m_data, (signed long int) orig, 1);
            m_doubleData = orig;
            check(*this);
        }

        inline Double & operator=(const Double & orig)
        {
            check(orig);
            if (&orig == this) {
                //LPERROR("ezek azonosak!");
                return *this;
            }
            m_doubleData = orig.m_doubleData;
            mpq_clear(m_data);

            m_state = orig.m_state;
            if (m_state == FINITE) {
                mpq_init(m_data);
                mpq_set(m_data, orig.m_data);
            } else {
                mpq_init(m_data);
            }
            static int counter = 0;
            counter++;
            check(*this);
            return *this;
        }

        inline Double & operator=(double orig)
        {
            mpq_clear(m_data);
            m_doubleData = orig;
            if (orig == infinity) {
                mpq_init(m_data);
                m_state = PLUS_INFINITY;
                check(*this);
                return *this;
            } else if (orig == -infinity) {
                mpq_init(m_data);
                m_state = MINUS_INFINITY;
                check(*this);
                return *this;
            }
            m_state = FINITE;
            mpq_init(m_data);
            mpq_set_d(m_data, orig);
            check(*this);
            return *this;
        }

        inline Double & operator=(int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_si(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline Double & operator=(long int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_si(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline Double & operator=(unsigned int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_ui(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline Double & operator=(unsigned long int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_ui(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline Double & operator=(unsigned long long int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_ui(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline Double & operator=(long long int orig)
        {
            m_state = FINITE;
            mpq_clear(m_data);
            mpq_init(m_data);
            mpq_set_si(m_data, orig, 1);
            m_doubleData = orig;
            check(*this);
            return *this;
        }

        inline ~Double()
        {
            m_doubleData = 0;
            mpq_clear(m_data);
        }

        operator unsigned int () const
        {
            // TODO: exception dobas
            return (unsigned int) mpq_get_d(m_data);
        }

        operator int () const
        {
            // TODO: exception dobas
            return (int) mpq_get_d(m_data);
        }

        operator double () const
        {
            switch (m_state) {
                case FINITE:
                    return mpq_get_d(m_data);
                case PLUS_INFINITY:
                    return infinity;
                case MINUS_INFINITY:
                    return -infinity;
                case NOT_A_NUMBER:
                    return (0.0 / 0.0);
            }
            return 0.0;
        }

        static unsigned long long int getAddCount()
        {
            return sm_addCount;
        }

        static unsigned long long int getSubCount()
        {
            return sm_subCount;
        }

        static unsigned long long int getMulCount()
        {
            return sm_mulCount;
        }

        static unsigned long long int getDivCount()
        {
            return sm_divCount;
        }

        inline Double operator+(const Double & val) const
        {
            Double result;
            add(result, *this, val);
            return result;
        }

        inline Double operator+(double val) const
        {
            return *this +Double(val);
        }

        inline Double operator+(int val) const
        {
            return *this +Double(val);
        }

        inline Double operator+(unsigned int val) const
        {
            return *this +Double(val);
        }

        inline Double operator+(long val) const
        {
            return *this +Double(val);
        }

        inline Double operator+(unsigned long val) const
        {
            return *this +Double(val);
        }

        friend inline Double operator+(double v1, const Double & v2)
        {
            return v2 + Double(v1);
        }

        inline Double & operator+=(const Double & val)
        {
            add(*this, *this, val);
            return *this;
        }

        inline Double & operator+=(double val)
        {
            *this += Double(val);
            return *this;
        }

        inline Double operator-(const Double & val) const
        {
            Double result;
            sub(result, *this, val);
            return result;
        }

        inline Double operator-(double val) const
        {
            return *this -Double(val);
        }

        inline Double operator-(int val) const
        {
            return *this -Double(val);
        }

        inline Double operator-(unsigned int val) const
        {
            return *this -Double(val);
        }

        inline Double operator-(long val) const
        {
            return *this -Double(val);
        }

        inline Double operator-(unsigned long val) const
        {
            return *this -Double(val);
        }

        friend inline Double operator-(double v1, const Double & v2)
        {
            return v2 - Double(v1);
        }

        friend inline Double operator-(int v1, const Double & v2)
        {
            return v2 - Double(v1);
        }

        friend inline Double operator-(long v1, const Double & v2)
        {
            return v2 - Double(v1);
        }

        friend inline Double operator-(unsigned int v1, const Double & v2)
        {
            return v2 - Double(v1);
        }

        friend inline Double operator-(unsigned long v1, const Double & v2)
        {
            return v2 - Double(v1);
        }

        inline Double & operator-=(const Double & val)
        {
            sub(*this, *this, val);
            return *this;
        }

        inline Double & operator-=(double val)
        {
            *this -= Double(val);
            return *this;
        }

        inline Double operator*(const Double & val) const
        {
            Double result;
            mul(result, *this, val);
            return result;
        }

        inline Double operator*(double val) const
        {
            return *this * Double(val);
        }

        inline Double operator*(int val) const
        {
            return *this * Double(val);
        }

        inline Double operator*(long val) const
        {
            return *this * Double(val);
        }

        inline Double operator*(unsigned int val) const
        {
            return *this * Double(val);
        }

        inline Double operator*(unsigned long val) const
        {
            return *this * Double(val);
        }

        friend inline Double operator*(double v1, const Double & v2)
        {
            return v2 * Double(v1);
        }

        friend inline Double operator*(int v1, const Double & v2)
        {
            return v2 * Double(v1);
        }

        friend inline Double operator*(long v1, const Double & v2)
        {
            return v2 * Double(v1);
        }

        friend inline Double operator*(unsigned int v1, const Double & v2)
        {
            return v2 * Double(v1);
        }

        friend inline Double operator*(unsigned long v1, const Double & v2)
        {
            return v2 * Double(v1);
        }

        inline Double & operator*=(const Double & val)
        {
            mul(*this, *this, val);
            return *this;
        }

        inline Double & operator*=(double val)
        {
            *this *= (Double(val));
            return *this;
        }

        inline Double operator/(const Double & val) const
        {
            Double result;
            div(result, *this, val);
            return result;
        }

        inline Double operator/(double val) const
        {
            return *this / Double(val);
        }

        inline Double operator/(int val) const
        {
            return *this / Double(val);
        }

        inline Double operator/(unsigned int val) const
        {
            return *this / Double(val);
        }

        inline Double operator/(long val) const
        {
            return *this / Double(val);
        }

        inline Double operator/(unsigned long val) const
        {
            return *this / Double(val);
        }

        friend inline Double operator/(double v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(int v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(long v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(long long v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(unsigned int v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(unsigned long v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        friend inline Double operator/(unsigned long long v1, const Double & v2)
        {
            return v2 / Double(v1);
        }

        inline Double & operator/=(const Double & val)
        {
            div(*this, *this, val);
            return *this;
        }

        inline Double & operator/=(double val)
        {
            div(*this, *this, Double(val));
            return *this;
        }

        inline Double operator-() const
        {
            check(*this);
            Double result = *this;
            result.m_doubleData = -m_doubleData;
            mpq_neg(result.m_data, result.m_data);
            if (result.m_state == PLUS_INFINITY) {
                result.m_state = MINUS_INFINITY;
            } else if (result.m_state == MINUS_INFINITY) {
                result.m_state = PLUS_INFINITY;
            }
            check(*this);
            check(result);
            return result;
        }

        inline bool operator==(const Double & val) const
        {
            return compare(*this, val) == 0;
        }

        inline bool operator==(double val) const
        {
            return *this == Double(val);
        }

        inline bool operator==(long int val) const
        {
            return *this == Double(val);
        }

        inline bool operator==(unsigned long int val) const
        {
            return *this == Double(val);
        }

        inline bool operator==(int val) const
        {
            return *this == Double(val);
        }

        inline bool operator==(unsigned int val) const
        {
            return *this == Double(val);
        }

        inline bool operator!=(const Double & val) const
        {
            return compare(*this, val) != 0;
        }

        inline bool operator!=(double val) const
        {
            return *this != Double(val);
        }

        inline bool operator!=(long int val) const
        {
            return *this != Double(val);
        }

        inline bool operator!=(unsigned long int val) const
        {
            return *this != Double(val);
        }

        inline bool operator!=(int val) const
        {
            return *this != Double(val);
        }

        inline bool operator!=(unsigned int val) const
        {
            return *this != Double(val);
        }

        inline bool operator>(const Double & val) const
        {
            return compare(*this, val) > 0;
        }

        inline bool operator>(double val) const
        {
            return *this > Double(val);
        }

        inline bool operator>(long int val) const
        {
            return *this > Double(val);
        }

        inline bool operator>(unsigned long int val) const
        {
            return *this > Double(val);
        }

        inline bool operator>(int val) const
        {
            return *this > Double(val);
        }

        inline bool operator>(unsigned int val) const
        {
            return *this > Double(val);
        }

        inline bool operator<(const Double & val) const
        {
            return compare(*this, val) < 0;
        }

        inline bool operator<(double val) const
        {
            return *this < Double(val);
        }

        inline bool operator<(long int val) const
        {
            return *this < Double(val);
        }

        inline bool operator<(unsigned long int val) const
        {
            return *this < Double(val);
        }

        inline bool operator<(int val) const
        {
            return *this < Double(val);
        }

        inline bool operator<(unsigned int val) const
        {
            return *this < Double(val);
        }

        inline bool operator>=(const Double & val) const
        {
            return compare(*this, val) >= 0;
        }

        inline bool operator>=(double val) const
        {
            return *this >= Double(val);
        }

        inline bool operator>=(long int val) const
        {
            return *this >= Double(val);
        }

        inline bool operator>=(unsigned long int val) const
        {
            return *this >= Double(val);
        }

        inline bool operator>=(int val) const
        {
            return *this >= Double(val);
        }

        inline bool operator>=(unsigned int val) const
        {
            return *this >= Double(val);
        }

        inline bool operator<=(const Double & val) const
        {
            return compare(*this, val) <= 0;
        }

        inline bool operator<=(double val) const
        {
            return *this <= Double(val);
        }

        inline bool operator<=(long int val) const
        {
            return *this <= Double(val);
        }

        inline bool operator<=(unsigned long int val) const
        {
            return *this <= Double(val);
        }

        inline bool operator<=(int val) const
        {
            return *this <= Double(val);
        }

        inline bool operator<=(unsigned int val) const
        {
            return *this <= Double(val);
        }

        friend std::ostream & operator<<(std::ostream & stream, const Double & val)
        {
            stream.precision(20);
            switch (val.m_state) {
                case Double::FINITE:
                    stream << mpq_get_d(val.m_data);
                    break;
                case Double::PLUS_INFINITY:
                    stream << infinity;
                    break;
                case Double::MINUS_INFINITY:
                    stream << -infinity;
                    break;
                case Double::NOT_A_NUMBER:
                    stream << (0.0 / 0.0);
                    break;
            }

            return stream;
        }
    };

    inline static Double round(Double val)
    {

        Double v = ::round(mpq_get_d(val.m_data));
        Double::check(v);
        return v;
    }

    inline static Double fabs(Double val)
    {

        if (val.m_state != Double::FINITE) {
            Double result(0.0);
            result.m_state = Double::PLUS_INFINITY;
            result.m_doubleData = ::fabs(val.m_doubleData);
            Double::check(result);
            return result;
        }
        Double result;
        mpq_abs(result.m_data, val.m_data);
        result.m_doubleData = ::fabs(val.m_doubleData);
        Double::check(result);
        return result;
    }

    inline static Double sqrt(Double val)
    {
        Double v = ::sqrt(mpq_get_d(val.m_data));
        Double::check(v);
        return v;
    }
#else
    typedef double Double;

    inline static Double fabs(Double val)
    {
        return ::fabs(val);
    }

    inline static Double sqrt(Double val)
    {
        return ::sqrt(val);
    }

    inline static Double round(double val)
    {
        return ::round(val);
    }

#endif    

    /**
     * epsilon representation for dealing with numerical errors when rounding
     */
    static const Double AbsoluteTolerance;
    static const Double RelativeTolerance;

    /**
     * Absolute value of tolerance in feasibility checking
     */
    static const Double feasibilityTolerance;

    /**
     * Absolute value of tolerance in optimality checking
     */
    static const Double optimalityTolerance;

    /**
     * Does fuzzy comparison for checking equity on two Double variables
     *
     * @param a first argument
     * @param b second argument
     * @return true if a and b equals by the meaning their difference is less than the AbsoluteTolerance
     * @author Mark Korondi <korondi@dcs.uni-pannon.hu>
     */
    static inline bool equals(const Double & a, const Double & b)
    {
#ifdef USE_MULTIPLE_PRECISION
        return a == b;
#else
        //        return fabs(a - b) < AbsoluteTolerance;
        register const Double aabs = Numerical::fabs(a);
        register const Double babs = Numerical::fabs(b);
        //Checking infinity
        register const Numerical::Double bound = (aabs > babs ? aabs : babs) * RelativeTolerance;
        if (bound == infinity) {
            return false;
        }
        //Equality is necessary here to handle the case if both of them are equal to zero!
        return Numerical::fabs(a - b) <= (bound > AbsoluteTolerance ? bound : AbsoluteTolerance);
#endif
    }

    /**
     * Does histerezis comparison for checking whether the first argument is
     * greater than the second.
     *
     * @param a first argument, assumed being greater than b
     * @param b second argument, assumed being less than a
     * @return true, if a>b, otherwise false
     */
    static inline bool greater(const Double & a, const Double & b)
    {
#ifdef USE_MULTIPLE_PRECISION
        return a > b;
#else
        return a - b > Numerical::AbsoluteTolerance;
#endif
    }

    /**
     * Does histerezis comparison for checking wether the first argument is
     * less than the second.
     *
     * @param a first argument, assumed being less than b
     * @param b second argument, assumed being greater than a
     * @return true, if a<b, otherwise false
     */
    static inline bool less(const Double & a, const Double & b)
    {
#ifdef USE_MULTIPLE_PRECISION
        return b > a;
#else
        return b - a > Numerical::AbsoluteTolerance;
#endif        
    }

    /**
     *
     * @param a
     * @return
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    static inline bool isZero(const Double & a)
    {
        //return Numerical::fabs(a) < Numerical::epsilon;
#ifdef USE_MULTIPLE_PRECISION
        return a == 0.0 || a == -0.0;
#else
        return a < 0.0 ? a > -Numerical::AbsoluteTolerance : a < Numerical::AbsoluteTolerance;
#endif        

    }

    struct Float32Bits
    {
        unsigned int m_sign : 1;
        unsigned int m_exponent : 8;
        unsigned int m_mantissa : 23;
    };

    struct Float64Bits
    {
        unsigned long long int m_mantissa : 52;
        unsigned long long int m_exponent : 11;
        unsigned long long int m_sign : 1;
    };

    union Float32
    {
        float m_value;
        Float32Bits m_bits;
    };

    union Float64
    {
        double m_value;
        Float64Bits m_bits;
    };

    /**
     *
     * @param value1
     * @param value2
     * @return
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    static inline bool rigidEqualsFloat64(const double & value1, const double & value2)
    {
        Float64 val1;
        val1.m_value = value1;
        Float64 val2;
        val2.m_value = value2;
        return val1.m_bits.m_sign == val2.m_bits.m_sign &&
                val1.m_bits.m_exponent == val2.m_bits.m_exponent &&
                val1.m_bits.m_mantissa == val2.m_bits.m_mantissa;
    }

    /**
     * Numerical stable add operation. Ensures that when the first operand
     * is negative of second one, the result will be zero.
     *
     * @param value1
     * @param value2
     * @return
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    static inline Double stableAdd(register const Double & value1, register const Double & value2)
    {
        register const Double value1abs = Numerical::fabs(value1);
        register const Double value2abs = Numerical::fabs(value2);
        register const Double result = value1 + value2;
        if ((value1abs > value2abs ? value1abs : value2abs) * RelativeTolerance > Numerical::fabs(result)) {
            return 0.0;
        }
        return result;
    }

    /**
     * Numerical stable subtraction operation. Ensures that when operands are equal,
     * the result will be zero.
     *
     * @param value1
     * @param value2
     * @return
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    static inline Double stableSub(register const Double & value1, register const Double & value2)
    {
        register const Double value1abs = Numerical::fabs(value1);
        register const Double value2abs = Numerical::fabs(value2);
        register const Double result = value1 - value2;
        if ((value1abs > value2abs ? value1abs : value2abs) * Numerical::RelativeTolerance > Numerical::fabs(result)) {
            return 0.0;
        }
        return result;
    }


};

#endif /* NUMERICAL_H_ */
