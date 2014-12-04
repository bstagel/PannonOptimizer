#include <utils/primitives.h>
#include <debug.h>

union Number {
    double m_num;
    unsigned long long int m_bits;
};

bool __prim_debug = false;

void doNothing_seriously_nothing() {
    // I've warned you...
}

Numerical::Double denseToDenseDotProductUnstable(const Numerical::Double * __restrict__ vec1,
                                                 const Numerical::Double * __restrict__ vec2,
                                                 size_t count) {
    Numerical::Double result1 = 0.0;
    Numerical::Double result2 = 0.0;
    unsigned int index;
    unsigned int one = count % 2;
    count -= one;
    for (index = 0; index < count; index += 2) {
        result1 += vec1[index] * vec2[index];
        result2 += vec1[index + 1] * vec2[index + 1];
    }
    if (one == 1) {
        result1 += vec1[count] * vec2[count];
    }
    return result1 + result2;
}

Numerical::Double denseToSparseDotProductUnstable(const Numerical::Double * __restrict__ dense,
                                                  const Numerical::Double * __restrict__ sparse,
                                                  const unsigned int * __restrict__ indices,
                                                  size_t count) {

    Numerical::Double result1 = 0.0;
    Numerical::Double result2 = 0.0;
    unsigned int index;
    //unsigned int one = count % 2;
    //count -= one;
    for (index = 0; index < count; index += 1) {
        result1 += dense[ indices[index] ] * sparse[index];
       // result2 += dense[ indices[index + 1] ] * sparse[index + 1];
    }
    /*if (one == 1) {
        result1 += dense[ indices[count] ] * sparse[count];
    }*/
    return result1 + result2;
}

Numerical::Double denseToDenseDotProductStable(const Numerical::Double * __restrict__ vec1,
                                               const Numerical::Double * __restrict__ vec2,
                                               size_t count,
                                               Numerical::Double * __restrict__ negPtr) {
#if DOUBLE_TYPE == DOUBLE_CLASSIC
    double negpos1[2] = {0.0, 0.0};
    double negpos2[2] = {0.0, 0.0};
    Number num1, num2;
    unsigned int index;
    unsigned int one = count % 2;
    count -= one;
    for (index = 0; index < count; index += 2) {
        num1.m_num = vec1[index] * vec2[index];
        num2.m_num = vec1[index + 1] * vec2[index + 1];
        *(negpos1 + (num1.m_bits >> 63)) += num1.m_num;
        *(negpos2 + (num2.m_bits >> 63)) += num2.m_num;
    }
    if (one == 1) {
        num1.m_num = vec1[count] * vec2[count];
        *(negpos1 + (num1.m_bits >> 63)) += num1.m_num;
    }
    *negPtr = negpos1[1] + negpos2[1];
    return negpos1[0] + negpos2[0];
#else
    unsigned int index;
    Numerical::Double neg = 0.0;
    Numerical::Double pos = 0.0;
    for (index = 0; index < count; index++) {
        Numerical::Double prod = vec1[index] * vec2[index];
        if (prod < 0.0) {
            neg += prod;
        } else {
            pos += prod;
        }
    }
    *negPtr = neg;
    return pos;
#endif
}

Numerical::Double denseToSparseDotProductStable(const Numerical::Double * __restrict__ dense,
                                                const Numerical::Double * __restrict__ sparse,
                                                const unsigned int * __restrict__ indices,
                                                size_t count,
                                                Numerical::Double * __restrict__ negPtr) {
#if DOUBLE_TYPE == DOUBLE_CLASSIC
    double negpos1[2] = {0.0, 0.0};
    double negpos2[2] = {0.0, 0.0};
    Number num1, num2;
    unsigned int index;
    unsigned int one = count % 2;

    /*if (__prim_debug) {
        LPINFO("SIZE = " << count);
    }*/

    count -= one;
    for (index = 0; index < count; index += 2) {
        /*if (__prim_debug) {
            LPINFO(index << " - " << index + 1);
        }*/
        num1.m_num = dense[ indices[index] ] * sparse[index];
        num2.m_num = dense[ indices[index + 1] ] * sparse[index + 1];
        /*if (__prim_debug) {
            LPINFO(dense[ indices[index] ] << " * " << sparse[index]);
            LPINFO(dense[ indices[index + 1] ] << " * " << sparse[index + 1]);
        }*/
        *(negpos1 + (num1.m_bits >> 63)) += num1.m_num;
        *(negpos1 + (num2.m_bits >> 63)) += num2.m_num;
    }

    if (one == 1) {
    /*    if (__prim_debug == true) {
            LPINFO(count);
        }*/
        num1.m_num = dense[ indices[count] ] * sparse[count];
        *(negpos1 + (num1.m_bits >> 63)) += num1.m_num;
    }

    /*if (__prim_debug) {
        LPERROR(negpos1[1] << " + " << negpos2[1] << " = " << negpos1[1] + negpos2[1]);
        LPERROR(negpos1[0] << " + " << negpos2[0] << " = " << negpos1[0] + negpos2[0]);
    }
    if (__prim_debug) {
        std::cin.get();
    }*/
    *negPtr = negpos1[1] + negpos2[1];
    return negpos1[0] + negpos2[0];
#else
    unsigned int index;
    Numerical::Double neg = 0.0;
    Numerical::Double pos = 0.0;
    for (index = 0; index < count; index++) {
        Numerical::Double prod = dense[ indices[index] ] * sparse[index];
        if (prod < 0.0) {
            neg += prod;
        } else {
            pos += prod;
        }
    }
    *negPtr = neg;
    return pos;
#endif
}

Numerical::Double indexedDenseToIndexedDenseDotProductUnstable(const Numerical::Double * __restrict__ dense1,
                                                               const Numerical::Double * __restrict__ dense2,
                                                               const unsigned int * __restrict__ indices,
                                                               size_t nonZeroCount) {
    Numerical::Double sum1 = 0.0;
    Numerical::Double sum2 = 0.0;
    unsigned int one = nonZeroCount % 2;
    nonZeroCount -= one;

    unsigned int nonZeroIndex = 0;
    for (nonZeroIndex = 0; nonZeroIndex < nonZeroCount; nonZeroIndex += 2) {
        const unsigned int index1 = indices[nonZeroIndex];
        sum1 += dense1[index1] * dense2[index1];
        const unsigned int index2 = indices[nonZeroIndex + 1];
        sum2 += dense1[index2] * dense2[index2];
    }
    if (one == 1) {
        const unsigned int index = indices[nonZeroCount];
        sum1 += dense1[index] * dense2[index];
    }
    return sum1 + sum2;
}

Numerical::Double indexedDenseToIndexedDenseDotProductStable(const Numerical::Double * __restrict__ dense1,
                                                                    const Numerical::Double * __restrict__ dense2,
                                                                    const unsigned int * __restrict__ indices,
                                                                    size_t nonZeroCount,
                                                                    Numerical::Double * __restrict__ negPtr) {
#if DOUBLE_TYPE == DOUBLE_CLASSIC
    double negpos[2] = {0.0, 0.0};
    Number num1, num2;
    unsigned int nonZeroIndex;
    unsigned int one = nonZeroCount % 2;

    nonZeroCount -= one;
    for (nonZeroIndex = 0; nonZeroIndex < nonZeroCount; nonZeroIndex += 2) {
        const unsigned int index1 = indices[nonZeroIndex];
        num1.m_num = dense1[ index1 ] * dense2[index1];
        const unsigned int index2 = indices[nonZeroIndex + 1];
        num2.m_num = dense1[ index2 ] * dense2[index2];
        *(negpos + (num1.m_bits >> 63)) += num1.m_num;
        *(negpos + (num2.m_bits >> 63)) += num2.m_num;
    }

    if (one == 1) {
        const unsigned int index = indices[nonZeroCount];
        num1.m_num = dense1[index] * dense2[index];
        *(negpos + (num1.m_bits >> 63)) += num1.m_num;
    }

    *negPtr = negpos[1];
    return negpos[0];
#else
    unsigned int nonZeroIndex;
    Numerical::Double neg = 0.0;
    Numerical::Double pos = 0.0;
    for (nonZeroIndex = 0; nonZeroIndex < nonZeroCount; nonZeroIndex++) {
        const unsigned int index = indices[nonZeroIndex];
        Numerical::Double prod = dense1[ index ] * dense2[ index ];
        if (prod < 0.0) {
            neg += prod;
        } else {
            pos += prod;
        }
    }
    *negPtr = neg;
    return pos;
#endif
}
