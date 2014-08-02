#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <globals.h>
#include <utils/numerical.h>

extern Numerical::Double denseToDenseDotProductUnstable(const Numerical::Double * __restrict__ vec1,
                                                        const Numerical::Double * __restrict__ vec2,
                                                        size_t count);

#endif // PRIMITIVES_H
