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

#ifndef MACROS_H
#define MACROS_H

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define STATIC_ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define __UNUSED(x) (void)x;

#define INVALID_POSITION INT_MIN
#define SPARSE 1
#define DENSE 0

#define CONCAT(a, b) a ## b

#define STL_FOREACH(_CONTAINER_TYPE, _CONTAINER, _ITERATOR) \
    _CONTAINER_TYPE::const_iterator _ITERATOR = _CONTAINER.begin(); \
    _CONTAINER_TYPE::const_iterator CONCAT(_ITERATOR, End) = _CONTAINER.end(); \
    for (; _ITERATOR != CONCAT(_ITERATOR, End); _ITERATOR++)

#define STL_FOREACH_MODIFY(_CONTAINER_TYPE, _CONTAINER, _ITERATOR) \
    _CONTAINER_TYPE::iterator _ITERATOR = _CONTAINER.begin(); \
    _CONTAINER_TYPE::iterator CONCAT(_ITERATOR, End) = _CONTAINER.end(); \
    for (; _ITERATOR != CONCAT(_ITERATOR, End); _ITERATOR++)


#endif // MACROS_H
