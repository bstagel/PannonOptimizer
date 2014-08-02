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
