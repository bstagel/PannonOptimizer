/**
 * @file numerical.h
 */

#ifndef _NUMERICAL_H_
#define _NUMERICAL_H_

#include "globals.h"

#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>
#include "../debug.h"

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

    static const Double Infinity;

    /**
     * epsilon representation for dealing with numerical errors when rounding
     */
    static const Double AbsoluteTolerance;
    static const Double RelativeTolerance;

//    /**
//     * Absolute value of tolerance in feasibility checking
//     */
//    static const Double FeasibilityTolerance;

//    /**
//     * Absolute value of tolerance in optimality checking
//     */
//    static const Double OptimalityTolerance;

    /**
     * Does fuzzy comparison for checking equity on two Double variables
     *
     * @param a first argument
     * @param b second argument
     * @return true if a and b equals by the meaning their difference is less than the AbsoluteTolerance
     */
    static inline bool equals(const Double & a, const Double & b)
    {
        //        return fabs(a - b) < AbsoluteTolerance;
        register const Double aabs = Numerical::fabs(a);
        register const Double babs = Numerical::fabs(b);
        //Checking infinity
        register const Numerical::Double bound = (aabs > babs ? aabs : babs) * RelativeTolerance;
        if (bound == Infinity) {
            return false;
        }
        //Equality is necessary here to handle the case if both of them are equal to zero!
        return Numerical::fabs(a - b) <= (bound > AbsoluteTolerance ? bound : AbsoluteTolerance);
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
        return a - b > Numerical::AbsoluteTolerance;
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
        return b - a > Numerical::AbsoluteTolerance;
    }

    static inline bool isZero(const Double & a)
    {
        //return Numerical::fabs(a) < Numerical::epsilon;
        return a < 0.0 ? a > -Numerical::AbsoluteTolerance : a < Numerical::AbsoluteTolerance;
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
