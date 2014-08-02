#include <utils/primitives.h>

Numerical::Double denseToDenseDotProductUnstable(const Numerical::Double * __restrict__ vec1,
                                                 const Numerical::Double * __restrict__ vec2,
                                                 size_t count) {
    double result = 0.0;
    unsigned int index;
    for (index = 0; index < count; index++) {
        result += vec1[index] * vec2[index];
    }
    return result;
}
