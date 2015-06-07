//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <globals.h>
#include <utils/numerical.h>

extern bool __prim_debug;

extern void doNothing_seriously_nothing();

extern Numerical::Double denseToDenseDotProductUnstable(const Numerical::Double * __restrict__ vec1,
                                                        const Numerical::Double * __restrict__ vec2,
                                                        size_t count);

extern Numerical::Double denseToSparseDotProductUnstable(const Numerical::Double * __restrict__ dense,
                                                         const Numerical::Double * __restrict__ sparse,
                                                         const unsigned int * __restrict__ indices,
                                                         size_t count);

extern Numerical::Double denseToDenseDotProductStable(const Numerical::Double * __restrict__ vec1,
                                                      const Numerical::Double * __restrict__ vec2,
                                                      size_t count,
                                                      Numerical::Double * __restrict__ negPtr);

extern Numerical::Double denseToSparseDotProductStable(const Numerical::Double * __restrict__ dense,
                                                       const Numerical::Double * __restrict__ sparse,
                                                       const unsigned int * __restrict__ indices,
                                                       size_t count,
                                                       Numerical::Double * __restrict__ negPtr);

extern Numerical::Double indexedDenseToIndexedDenseDotProductUnstable(const Numerical::Double * __restrict__ dense1,
                                                                      const Numerical::Double * __restrict__ dense2,
                                                                      const unsigned int * __restrict__ indices,
                                                                      size_t nonZeroCount);

extern Numerical::Double indexedDenseToIndexedDenseDotProductStable(const Numerical::Double * __restrict__ dense1,
                                                                    const Numerical::Double * __restrict__ dense2,
                                                                    const unsigned int * __restrict__ indices,
                                                                    size_t nonZeroCount,
                                                                    Numerical::Double * __restrict__ negPtr);

extern void denseToDenseAddAbsRel(const Numerical::Double * __restrict__ a,
                                  const Numerical::Double * __restrict__ b,
                                  Numerical::Double * __restrict__ c,
                                  size_t count,
                                  Numerical::Double lambda,
                                  Numerical::Double absTolerance,
                                  Numerical::Double relTolerance);

extern void denseToDenseAddAbs(const Numerical::Double * __restrict__ a,
                               const Numerical::Double * __restrict__ b,
                               Numerical::Double * __restrict__ c,
                               size_t count,
                               Numerical::Double lambda,
                               Numerical::Double absTolerance);

extern void denseToDenseAdd(const Numerical::Double * __restrict__ a,
                            const Numerical::Double * __restrict__ b,
                            Numerical::Double * __restrict__ c,
                            size_t count,
                            Numerical::Double lambda);

#endif // PRIMITIVES_H
