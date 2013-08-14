/**
 * @file vector_iterator.h
 */

#ifndef VECTOR_ITERATOR_H
#define	VECTOR_ITERATOR_H

#include "globals.h"

#ifdef IN_VECTOR

#include <utils/numerical.h>

class CommonIterator
{
public:

    CommonIterator() :
        m_start(0), m_end(0), m_data(0), m_index(0) {}

    CommonIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index) :
        m_start(start), m_end(end), m_data(data), m_index(index) {}

    inline unsigned int getIndex() const
    {
        if (m_index)
            return *m_index;
        else
            return m_data - m_start;
    }

    inline bool operator==(const CommonIterator & iterator) const
    {
        return m_data == iterator.m_data;
    }

    inline bool operator!=(const CommonIterator & iterator) const
    {
        return m_data != iterator.m_data;
    }

    inline bool operator<(const CommonIterator & iterator) const
    {
        return m_data < iterator.m_data;
    }

    inline bool operator>(const CommonIterator & iterator) const
    {
        return m_data > iterator.m_data;
    }

    inline bool operator<=(const CommonIterator & iterator) const
    {
        return m_data <= iterator.m_data;
    }

    inline bool operator>=(const CommonIterator & iterator) const
    {
        return m_data >= iterator.m_data;
    }

    inline const Numerical::Double & operator*() const
    {
        return *m_data;
    }

//    inline Numerical::Double * operator->() const
//    {
//        return m_data;
//    }
protected:
    Numerical::Double * m_start;
    Numerical::Double * m_end;
    Numerical::Double * m_data;
    unsigned int * m_index;

    inline void next()
    {
        m_data++;
        if (m_index)
            m_index++;
    }

    inline void prev()
    {
        m_data--;
        if (m_index)
            m_index--;
    }

    inline void nextToNonzero()
    {
        do {
            m_data++;
            if (m_index)
                m_index++;
        } while (m_data < m_end && *m_data == 0.0);
    }

    inline void prevToNonzero()
    {
        do {
            m_data--;
            if (m_index)
                m_index--;
        } while (m_data >= m_start && *m_data == 0.0);
    }
};


class Iterator : public CommonIterator
{
public:
    Iterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index) :
        CommonIterator(start, end, data, index) {}

    inline Iterator & operator++()
    {
        next();
        return *this;
    }

    inline Iterator & operator++(int)
    {
        next();
        return *this;
    }

    inline Iterator & operator--()
    {
        prev();
        return *this;
    }

    inline Iterator & operator--(int)
    {
        prev();
        return *this;
    }
};

class NonzeroIterator : public CommonIterator
{
public:
    NonzeroIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index, bool next) :
        CommonIterator(start, end, data, index)
    {
        if (next && *start == 0.0)
            nextToNonzero();
    }

    inline NonzeroIterator & operator++()
    {
        nextToNonzero();
        return *this;
    }

    inline NonzeroIterator & operator++(int)
    {
        nextToNonzero();
        return *this;
    }

    inline NonzeroIterator & operator--()
    {
        prevToNonzero();
        return *this;
    }

    inline NonzeroIterator & operator--(int)
    {
        prevToNonzero();
        return *this;
    }
};

#endif /* IN_VECTOR */

#endif	/* VECTOR_ITERATOR_H */
