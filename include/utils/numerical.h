/**
 * @file numerical.h This file contains the API of the Numerical class.
 * @author Jozsef Smidla, tar
 */

#ifndef _NUMERICAL_H_
#define _NUMERICAL_H_

#include <globals.h>

#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>
#include <debug.h>

#define DOUBLE_CLASSIC 0
#define DOUBLE_HISTORY 1

#define DOUBLE_TYPE DOUBLE_CLASSIC

#if DOUBLE_TYPE == DOUBLE_HISTORY
#include <utils/doublehistory.h>
#endif

#if DOUBLE_TYPE == DOUBLE_CLASSIC
#define COPY_DOUBLES(dest, src, count) panOptMemcpy(dest, src, sizeof(Numerical::Double) * count);
#define CLEAR_DOUBLES(dest, count) panOptMemset(dest, 0, sizeof(Numerical::Double) * count);
//memcpy(dest, src, sizeof(Numerical::Double) * count);
#else
#define COPY_DOUBLES(dest, src, count) {unsigned int _count = (count); \
    for(unsigned int i = 0; i < _count; i++) { \
    dest[i] = src[i]; \
    } \
    }
#define CLEAR_DOUBLES(dest, count) {unsigned int _count = (count); \
    for(unsigned int i = 0; i < _count; i++) { \
    dest[i] = 0.0;
    } \
    }
#endif

/**
 * This class implements numerical functions focusing on high-precisition results.
 *
 * @class Numerical
 */
class Numerical
{
    friend class InitPanOpt;
private:
    static void _globalInit();

public:

    /**
     * This type describes an addition operation type.
     */
    enum ADD_TYPE {
        ADD_FAST,
        ADD_ABS,
        ADD_REL,
        ADD_ABS_REL
    };

    /**
     * This type describes the dot product type.
     */
    enum DOT_PRODUCT_TYPE {
        DOT_UNSTABLE,
        DOT_FAST,
        DOT_ABS,
        DOT_REL,
        DOT_ABS_REL
    };

#if DOUBLE_TYPE == DOUBLE_CLASSIC
    typedef double Double;
#endif

#if DOUBLE_TYPE == DOUBLE_HISTORY
    typedef DoubleHistory Double;
#endif

    /**
     * Helper enum to provide the sign bit of a double.
     */
    union Number
    {
        double m_num;
        unsigned long long int m_bits;
    };

    /**
     * This class implements a summarizer able to add elements with the same sign first.
     * With this technique there are less numerical errors when summing floating point numbers.
     *
     * @class Summarizer
     */
    class Summarizer
    {
        /**
         * The sum of elements with positive and negative sign separated.
         */
        Double m_negpos[2];

       /**
        * Helper type to separate positive and negative doubles quickly with bit operators.
        */
        Number m_number;
    public:

        /**
         * Constructor of the Summarizer class.
         * Initializes the sum to zero.
         *
         * @constructor
         */
        Summarizer()
        {
            m_negpos[0] = 0.0;
            m_negpos[1] = 0.0;
        }

        /**
         * Adds a value to the summarizer.
         * This detects the sign of the number and adds it to the corresponding sum.
         *
         * @param v The value to be added.
         */
        ALWAYS_INLINE void add(const Double & v)
        {
            m_number.m_num = v;
            *(m_negpos + (m_number.m_bits >> 63)) += v;
        }

        /**
         * Returns the result of the sums, with the addition of the negative and positive sum.
         *
         * @param abs Set this true to use absolute tolerance during the addition.
         * @param rel Set this true to use relative tolerance during the addition.
         * @return The result of the sum.
         */
        inline Double getResult(bool abs = true, bool rel = true) const
        {
            if(abs && rel){
                return stableAdd(m_negpos[0], m_negpos[1]);
            } else if(abs){
                return stableAddAbs(m_negpos[0], m_negpos[1]);
            } else if(rel){
                return stableAddRel(m_negpos[0], m_negpos[1]);
            } else {
                return m_negpos[0] + m_negpos[1];
            }
        }

        /**
         * Clears the summarizer and sets its sums to zero.
         */
        inline void clear()
        {
            m_negpos[0] = 0.0;
            m_negpos[1] = 0.0;
        }

    };

    /**
     * This class implements a summarizer able to add elements with the same magnitude.
     * With this technique there are less numerical errors when summing floating point numbers.
     *
     * @class BucketSummarizer
     */
    class BucketSummarizer {

        /**
         * The sum of elements with the same magnitude separated.
         */
        double * m_buckets;

        /**
         * The number of bits deciding the magnitude bucket of the number.
         */
        unsigned int m_shift;

        /**
         * The number of magnitude buckets.
         */
        unsigned int m_size;

    public:

        /**
         * Constructor of the BucketSummarizer class.
         * The number of magnitude buckets can be specified by the level.
         *
         * @param level The level of the bucket summarizer. Higher level means fewer buckets.
         */
        BucketSummarizer(int level) {
            m_shift = 11 - level;
            m_size = 1 << m_shift;
            m_buckets = alloc<double, 32>(m_size);
            memset(m_buckets, 0, sizeof(double) * m_size);
            m_shift = 53 + level;
        }

        /**
         * Destructor of the BucketSummarizer class.
         *
         * @destructor
         */
        ~BucketSummarizer() {
            release(m_buckets);
        }

        /**
         * Adds a value to the summarizer.
         * This detects the magnitude of the number and adds it to the corresponding bucket.
         *
         * @param value The value to be added.
         */
        ALWAYS_INLINE void add(double value) {
            /**
             * This type allows the fast detection of double magnitude by bit operators.
             */
            union Bits {
                double m_value;
                unsigned long long int m_bits;
            } bits;
            bits.m_value = value;

            //const unsigned int index = ((*(unsigned long long int*)(&value)) ) >> (m_shift);
            const unsigned int index = bits.m_bits >> m_shift;
            m_buckets[ index ] += value;
        }

        /**
         * Returns the result of the sums, with the addition of sums with the same magnitude.
         *
         * @param abs Set this true to use absolute tolerance during the addition.
         * @param rel Set this true to use relative tolerance during the addition.
         * @return The result of the sum.
         */
        ALWAYS_INLINE double getResult(bool abs = true, bool rel = true) {
            double pos1 = 0;
            double pos2 = 0;
            double neg1 = 0;
            double neg2 = 0;
            unsigned int i;

            if (m_size == 2) {
                pos1 = m_buckets[0];
                neg1 = m_buckets[1];
                m_buckets[0] = 0;
                m_buckets[1] = 0;
            } else {
                for (i = 0; i < m_size / 2; i += 2) {
                    pos1 += m_buckets[i];
                    pos2 += m_buckets[i + 1];
                    m_buckets[i] = 0;
                    m_buckets[i + 1] = 0;
                }

                for (; i < m_size; i += 2) {
                    neg1 += m_buckets[i];
                    neg2 += m_buckets[i + 1];
                    m_buckets[i] = 0;
                    m_buckets[i + 1] = 0;
                }
            }

            if(abs && rel){
                return stableAdd(pos1 + pos2, neg1 + neg2);
            } else if(abs){
                return stableAddAbs(pos1 + pos2, neg1 + neg2);
            } else if(rel){
                return stableAddRel(pos1 + pos2, neg1 + neg2);
            } else {
                return (pos1 + pos2) + (neg1 + neg2);
            }
        }
    };

    /**
     * Returns the absolute value of the given number.
     *
     * @param val The given number.
     * @return The absolute value of val.
     */
    ALWAYS_INLINE static Double fabs(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::fabs(val);
#else
        return ::fabs(val);
#endif
    }

    /**
     * Returns the square root of the given number.
     *
     * @param val The given number.
     * @return The square root of val.
     */
    ALWAYS_INLINE static Double sqrt(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::sqrt(val);
#else
        return ::sqrt(val);
#endif
    }

    /**
     * Returns the rounded value of the given number.
     *
     * @param val The given number.
     * @return The rounded value of val.
     */
    ALWAYS_INLINE static Double round(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::round(val);
#else
        return ::round(val);
#endif
    }

    /**
     * Determines with tolerance whether two values are equal or not.
     *
     * @param value1 The first value.
     * @param value2 The second value.
     * @param tolerance The value of the absolute tolerance.
     * @return True if the difference of the two values are less than the tolerance.
     */
    ALWAYS_INLINE static bool equal(Double value1,Double value2,Double tolerance){
        if( fabs(value1-value2) <= tolerance ){
            return true;
        }
        return false;
    }

    /**
     * Determines with tolerance whether one value is less than another.
     *
     * @param value1 The first value.
     * @param value2 The second value.
     * @param tolerance The value of the absolute tolerance.
     * @return True if value1 is less than value2 by at least the tolerance.
     */
    ALWAYS_INLINE static bool lessthan(Double value1,Double value2,Double tolerance){
        if( value1 + tolerance < value2 ){
            return true;
        }
        return false;
    }

    /**
     * Constant value limit representing infinity.
     */
    static const Double Infinity;

    //static const Double & AbsoluteTolerance;
    //static const Double & RelativeTolerance;

    /**
     * This is a reference of numerical parameter "AbsoluteTolerance".
     * See LinalgParameterHandler for details.
     */
    static Double AbsoluteTolerance;

    /**
     * This is a reference of numerical parameter "RelativeTolerance".
     * See LinalgParameterHandler for details.
     */
    static Double RelativeTolerance;

    /**
     * Does fuzzy comparison for checking equality on two Double variables
     *
     * @param a first argument
     * @param b second argument
     * @return true if a and b equals by the meaning their difference is less than the AbsoluteTolerance
     */
    static inline bool equals(const Double & a, const Double & b)
    {
        const Double aabs = Numerical::fabs(a);
        const Double babs = Numerical::fabs(b);
        //Checking infinity
        const Numerical::Double bound = (aabs > babs ? aabs : babs) * RelativeTolerance;
        if (bound == Infinity) {
            return false;
        }
        //Equality is necessary here to handle the case if both of them are equal to zero!
        return Numerical::fabs(a - b) <= (bound > AbsoluteTolerance ? bound : AbsoluteTolerance);
    }

    /**
     * Does histerezis comparison for checking whether the first argument is less than the second.
     *
     * @param a first argument, assumed being less than b
     * @param b second argument, assumed being greater than a
     * @return true, if a<b, otherwise false
     */
    static ALWAYS_INLINE bool less(const Double & a, const Double & b)
    {
        return b - a > Numerical::AbsoluteTolerance;
    }

    /**
     * Does fuzzy comparsion to check whether a value is zero.
     *
     * @param a The given value to be checked.
     * @return True if a is zero.
     */
    static ALWAYS_INLINE bool isZero(const Double & a)
    {
        return a < 0.0 ? a > -Numerical::AbsoluteTolerance : a < Numerical::AbsoluteTolerance;
    }

    /**
     * This type describes the custom representation of a 64 bit floating point number.
     * This sign, exponent and mantissa are separated.
     */
    struct Float64Bits
    {
        unsigned long long int m_mantissa : 52;
        unsigned long long int m_exponent : 11;
        unsigned long long int m_sign : 1;
    };

    /**
     * This type allows to reach the sign, exponent and mantissa of a double individually.
     */
    union Float64
    {
        double m_value;
        Float64Bits m_bits;
    };

    /**
     * Returns the rigid equality of two doubles.
     *
     * @param value1 The first value.
     * @param value2 The second value.
     * @return True if the two values are equal.
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
     * Numerical stable add operation.
     * Ensures that when the first operand is negative of second one, the result will be zero.
     * This function uses absolute and relative tolerances.
     *
     * @param value1 The first value.
     * @param value2 The second value to be added.
     * @return The sum of the two values.
     */
    static ALWAYS_INLINE Double stableAdd(const Double & value1, const Double & value2)
    {
        //        if(value1 * value2 > 0){
        //            return value1 + value2;
        //        }
        //        *((unsigned long long int*)&num) = (0x7FFFFFFFFFFFFFFFULL & *((unsigned long long int*)(&num)));
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        const Double result = value1 + value2;
        if ((value1abs + value2abs) * RelativeTolerance > Numerical::fabs(result)) {
            return 0.0;
        }

        else if(Numerical::fabs(result)<AbsoluteTolerance){
            return 0.0;
        }
        return result;
    }

    /**
     * Numerical stable add operation.
     * Ensures that when the first operand is negative of second one, the result will be zero.
     * This function uses only relative tolerance.
     *
     * @param value1 The first value.
     * @param value2 The second value to be added.
     * @return The sum of the two values.
     */
    static ALWAYS_INLINE Double stableAddRel(const Double & value1, const Double & value2)
    {
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        const Double result = value1 + value2;
        if ((value1abs + value2abs) * RelativeTolerance >= Numerical::fabs(result)) {
            return 0.0;
        }

        return result;
    }


    /**
     * Numerical stable add operation.
     * Ensures that when the first operand is negative of second one, the result will be zero.
     * This function uses only absolute tolerance.
     *
     * @param value1 The first value.
     * @param value2 The second value to be added.
     * @return The sum of the two values.
     */
    static ALWAYS_INLINE Double stableAddAbs(const Double & value1, const Double & value2)
    {
        const Double result = value1 + value2;
        if(Numerical::fabs(result)<AbsoluteTolerance) {
            return 0.0;
        }

        return result;
    }

    /**
     * Numerical stable add operation.
     * Ensures that when the first operand is negative of second one, the result will be zero.
     * This function uses absolute and relative tolerances, and stores the result in value1.
     *
     * @param value1 The first value. The result will be stored in this variable also.
     * @param value2 The second value to be added.
     */
    static inline void stableAddTo(Double & value1, const Double & value2)
    {
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        value1 += value2;
        if ((value1abs > value2abs ? value1abs : value2abs) * RelativeTolerance >= Numerical::fabs(value1)) {
            value1 = 0.0;
        }
        else if(Numerical::fabs(value1)<AbsoluteTolerance){
            value1 = 0.0;
        }
    }

};

/**
 * Constant double defining zero.
 */
extern const Numerical::Double ZERO;

/**
 * Constant double defining invalid value.
 */
extern const Numerical::Double INVALID;

#endif /* NUMERICAL_H_ */
