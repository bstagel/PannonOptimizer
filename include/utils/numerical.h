/**
 * @file numerical.h
 */

#ifndef _NUMERICAL_H_
#define _NUMERICAL_H_

#include <globals.h>

#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>
#include <debug.h>
#include <utils/doublehistory.h>

//#include <qd/qd_real.h>

#define DOUBLE_CLASSIC 0
#define DOUBLE_HISTORY 1
#define DOUBLE_QD      2

#define DOUBLE_TYPE DOUBLE_CLASSIC

#if DOUBLE_TYPE == DOUBLE_CLASSIC
#define COPY_DOUBLES(dest, src, count) memcpy(dest, src, sizeof(Numerical::Double) * count);
#else
#define COPY_DOUBLES(dest, src, count) {unsigned int _count = (count); \
    for(unsigned int i = 0; i < _count; i++) { \
    dest[i] = src[i]; \
    } \
    }
#endif

class Numerical
{
public:

    enum ADD_TYPE {
        ADD_FAST,
        ADD_ABS,
        ADD_REL,
        ADD_ABS_REL
    };

    //    class QD_Summarizer
    //    {
    //        qd_real m_negpos[2];

    //        union Number
    //        {
    //            double m_num;
    //            unsigned long long int m_bits;
    //        } m_number;

    //    public:

    //        QD_Summarizer()
    //        {
    //            m_negpos[0] = 0.0;
    //            m_negpos[1] = 0.0;
    //        }

    //        inline void add(const double & v)
    //        {
    //            m_number.m_num = v;
    //            *(m_negpos + ((m_number.m_bits & 0x8000000000000000LL) >> 63)) += v;
    //            /*if (v < 0.0) {
    //                m_negpos[0] += v;
    //            } else {
    //                m_negpos[1] += v;
    //            }*/
    //        }

    //        inline double getResult() const
    //        {
    //            return stableAdd(m_negpos[0], m_negpos[1]);
    //        }

    //        inline void clear()
    //        {
    //            m_negpos[0] = 0.0;
    //            m_negpos[1] = 0.0;
    //        }

    ////        inline const double * getData() const {
    ////            return m_negpos;
    ////        }
    //    };

#if DOUBLE_TYPE == DOUBLE_CLASSIC
    typedef double Double;
#endif

#if DOUBLE_TYPE == DOUBLE_HISTORY
    typedef DoubleHistory Double;
#endif

    class Summarizer
    {
        Double m_negpos[2];

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

        ALWAYS_INLINE void add(const Double & v)
        {
            m_number.m_num = v;
            *(m_negpos + (m_number.m_bits >> 63)) += v;
            /*if (v < 0.0) {
                m_negpos[0] += v;
            } else {
                m_negpos[1] += v;
            }*/
        }

        inline Double getResult() const
        {
            return stableAdd(m_negpos[0], m_negpos[1]);
        }

        inline void clear()
        {
            m_negpos[0] = 0.0;
            m_negpos[1] = 0.0;
        }
        
        //        inline const double * getData() const {
        //            return m_negpos;
        //        }
    };

    class BucketSummarizer {

        double * m_buckets;


        unsigned int m_shift;

        unsigned int m_size;

    public:

        BucketSummarizer(int level) {
            m_shift = 11 - level;
            m_size = 1 << m_shift;
            m_buckets = alloc<double, 32>(m_size);
            m_shift = 53 + level;
        }

        ~BucketSummarizer() {
            release(m_buckets);
        }

        ALWAYS_INLINE void add(double value) {
            const unsigned int index = ((*(unsigned long long int*)(&value)) ) >> (m_shift);
            m_buckets[ index ] += value;
        }


        ALWAYS_INLINE double getResult() {
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
                return stableAdd(pos1 + pos2, neg1 + neg2);
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

            return stableAdd(pos1 + pos2, neg1 + neg2);
        }
    };

    ALWAYS_INLINE static Double fabs(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::fabs(val);
#else
        return ::fabs(val);
#endif
    }

    ALWAYS_INLINE static Double sqrt(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::sqrt(val);
#else
        return ::sqrt(val);
#endif
    }

    ALWAYS_INLINE static Double round(Double val)
    {
#if DOUBLE_TYPE == DOUBLE_HISTORY
        return DoubleHistory::round(val);
#else
        return ::round(val);
#endif
    }

    ALWAYS_INLINE static bool equal(Double value1,Double value2,Double tolerance){
        if( fabs(value1-value2) <= tolerance ){
            return true;
        }
        return false;
    }

    ALWAYS_INLINE static bool lessthan(Double value1,Double value2,Double tolerance){
        if( value1 + tolerance < value2 ){
            return true;
        }
        return false;
    }

    ALWAYS_INLINE static bool lessOrEqual(Double value1,Double value2,Double tolerance){
        return ( lessthan(value1,value2,tolerance) || equal(value1,value2,tolerance) );
    }

    static const Double Infinity;

    /**
     * epsilon representation for dealing with numerical errors when rounding
     */
    static const Double AbsoluteTolerance;
    static const Double RelativeTolerance;

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
     * Does histerezis comparison for checking whether the first argument is
     * greater than the second.
     *
     * @param a first argument, assumed being greater than b
     * @param b second argument, assumed being less than a
     * @return true, if a>b, otherwise false
     */
    static ALWAYS_INLINE bool greater(const Double & a, const Double & b)
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
    static ALWAYS_INLINE bool less(const Double & a, const Double & b)
    {
        return b - a > Numerical::AbsoluteTolerance;
    }

    static ALWAYS_INLINE bool isZero(const Double & a)
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
    static ALWAYS_INLINE Double stableAdd(const Double & value1, const Double & value2)
    {
        //        if(value1 * value2 > 0){
        //            return value1 + value2;
        //        }
        //        *((unsigned long long int*)&num) = (0x7FFFFFFFFFFFFFFFULL & *((unsigned long long int*)(&num)));
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        const Double result = value1 + value2;
        if ((value1abs + value2abs) * RelativeTolerance >= Numerical::fabs(result)) {
            //        if ((value1abs > value2abs ? value1abs : value2abs) * RelativeTolerance >= Numerical::fabs(result)) {
            return 0.0;
        }

        else if(Numerical::fabs(result)<AbsoluteTolerance){
            //            LPWARNING("stableAdd RESULT IS BAAAD: "<<std::scientific<<result);
            return 0.0;
        }
        //        LPINFO("result: "<<result);
        return result;
    }


    /**
     * Numerical stable add operation. Ensures that when the first operand
     * is negative of second one, the result will be zero.
     *
     * @param value1
     * @param value2
     * @return
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
     * Numerical stable add operation. Ensures that when the first operand
     * is negative of second one, the result will be zero.
     *
     * @param value1
     * @param value2
     * @return
     */
    static ALWAYS_INLINE Double stableAddAbs(const Double & value1, const Double & value2)
    {
        const Double result = value1 + value2;
        if(Numerical::fabs(result)<AbsoluteTolerance) {
            return 0.0;
        }

        return result;
    }
    //    static inline Double stableAdd(const qd_real & value1, const qd_real & value2)
    //    {
    //        const qd_real value1abs = value1.is_positive()?value1:-value1;
    //        const qd_real value2abs = value2.is_positive()?value2:-value2;
    //        const qd_real result = value1 + value2;
    //        const qd_real resultabs = result.is_positive()?result:-result;
    ////        LPINFO("result_before: "<<to_double(result));
    //        if ((value1abs > value2abs ? value1abs : value2abs) * RelativeTolerance >= resultabs) {
    //            return 0.0;
    //        }
    //        else if(resultabs<AbsoluteTolerance){
    //////            LPWARNING("stableAdd RESULT IS BAAAD: "<<std::scientific<<result);
    //            return 0.0;
    //        }
    ////        LPINFO("result: "<<result);
    ////        LPINFO("to_double(result): "<<to_double(result));
    //        return to_double(result);
    //    }
    /**
     * Numerical stable add operation. Ensures that when the first operand
     * is negative of second one, the result will be zero.
     *
     * @param value1
     * @param value2
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
            //            LPWARNING("stableAddTo RESULT IS BAAAD: "<<std::scientific<<value1);
            value1 = 0.0;
        }
    }

    /**
     * Numerical stable subtraction operation. Ensures that when operands are equal,
     * the result will be zero.
     *
     * @param value1
     * @param value2
     * @return
     */
    static inline Double stableSub(const Double & value1, const Double & value2)
    {
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        const Double result = value1 - value2;
        if ((value1abs > value2abs ? value1abs : value2abs) * Numerical::RelativeTolerance >= Numerical::fabs(result)) {
            return 0.0;
        }
        else if(Numerical::fabs(result)<AbsoluteTolerance){
            //            LPWARNING("stableSub RESULT IS BAAAD: "<<std::scientific<<result);
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
    static inline void stableSubFrom(Double & value1, const Double & value2)
    {
        const Double value1abs = Numerical::fabs(value1);
        const Double value2abs = Numerical::fabs(value2);
        value1 -= value2;
        if ((value1abs > value2abs ? value1abs : value2abs) * Numerical::RelativeTolerance >= Numerical::fabs(value1)) {
            value1 = 0.0;
        }
        else if(Numerical::fabs(value1)<AbsoluteTolerance){
            //            LPWARNING("stableAddTo RESULT IS BAAAD: "<<std::scientific<<value1);
            value1 = 0.0;
        }
    }
};

extern const Numerical::Double ZERO;
extern const Numerical::Double INVALID;

#endif /* NUMERICAL_H_ */
