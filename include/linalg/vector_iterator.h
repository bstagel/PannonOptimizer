/**
 * @file vector_iterator.h
 */


#ifndef VECTOR_ITERATOR_H
#define	VECTOR_ITERATOR_H

#include "globals.h"

#ifdef IN_VECTOR

#include <utils/numerical.h>

/**
 *
 */
class CommonIterator
{
public:

    /**
     *
     */
    CommonIterator() :
    m_start(0), m_end(0), m_data(0), m_index(0)
    {
    }

    /**
     *
     * @param start
     * @param end
     * @param data
     * @param index
     */
    CommonIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data,
            unsigned int * index) :
    m_start(start), m_end(end), m_data(data), m_index(index)
    {
    }

    /**
     *
     */
    virtual ~CommonIterator()
    {
    }

    /**
     *
     * @param original
     */
    CommonIterator(const CommonIterator & original) :
    m_start(original.m_start), m_end(original.m_end), m_data(original.m_data), m_index(original.m_index)
    {
    }

    /**
     *
     * @return
     */
    inline unsigned int getIndex() const
    {
        if (m_index)
            return *m_index;
        else
            return m_data - m_start;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator==(const CommonIterator & iterator) const
    {
        return m_data == iterator.m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator!=(const CommonIterator & iterator) const
    {
        return m_data != iterator.m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator<(const CommonIterator & iterator) const
    {
        return m_data < iterator.m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator>(const CommonIterator & iterator) const
    {
        return m_data > iterator.m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator<=(const CommonIterator & iterator) const
    {
        return m_data <= iterator.m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline bool operator>=(const CommonIterator & iterator) const
    {
        return m_data >= iterator.m_data;
    }
protected:
    /**
     *
     */
    Numerical::Double * m_start;
    /**
     *
     */
    Numerical::Double * m_end;
    /**
     *
     */
    Numerical::Double * m_data;
    /**
     *
     */
    unsigned int * m_index;

    /**
     *
     */
    inline void next()
    {
        m_data++;
        if (m_index)
            m_index++;
    }

    /**
     *
     */
    inline void prev()
    {
        m_data--;
        if (m_index)
            m_index--;
    }

    /**
     *
     */
    inline void nextToNonzero()
    {
        do {
            m_data++;
            if (m_index)
                m_index++;
        } while (m_data < m_end && *m_data == 0.0);
    }

    /**
     *
     */
    inline void prevToNonzero()
    {
        do {
            m_data--;
            if (m_index)
                m_index--;
            //        } while (m_data >= m_start && *m_data == 0.0);
        } while (m_data >= m_start && *m_data == 0.0);
    }

    /**
     *
     * @param iterator
     */
    inline void copy(const CommonIterator & iterator)
    {
        m_start = iterator.m_start;
        m_end = iterator.m_end;
        m_data = iterator.m_data;
        m_index = iterator.m_index;
    }

};
//public:

/**
 *
 */
class Iterator : public CommonIterator
{
public:

    /**
     *
     */
    Iterator() :
    CommonIterator()
    {
    }

    /**
     *
     * @param start
     * @param end
     * @param data
     * @param index
     */
    Iterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index) :
    CommonIterator(start, end, data, index)
    {
    }

    /**
     *
     */
    virtual ~Iterator()
    {
    }

    /**
     *
     * @param original
     */
    Iterator(const Iterator & original) :
    CommonIterator(original)
    {
    }

    /**
     *
     * @return
     */
    inline Numerical::Double & operator*() const
    {
        return *m_data;
    }

    /**
     *
     * @return
     */
    inline Numerical::Double * operator->() const
    {
        return m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline Iterator & operator=(const Iterator & iterator)
    {
        copy(iterator);
        return *this;
    }

    /**
     *
     * @return
     */
    inline Iterator & operator++()
    {
        next();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline Iterator & operator++(int)
    {
        next();
        return *this;
    }

    /**
     *
     * @return
     */
    inline Iterator & operator--()
    {
        prev();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline Iterator & operator--(int)
    {
        prev();
        return *this;
    }
};

/**
 *
 */
class ConstIterator : public CommonIterator
{
public:

    /**
     *
     */
    ConstIterator() :
    CommonIterator()
    {
    }

    /**
     *
     * @param start
     * @param end
     * @param data
     * @param index
     */
    ConstIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data,
            unsigned int * index) :
    CommonIterator(start, end, data, index)
    {
    }

    /**
     *
     */
    virtual ~ConstIterator()
    {
    }

    /**
     *
     * @param original
     */
    ConstIterator(const ConstIterator & original) :
    CommonIterator(original)
    {
    }

    /**
     *
     * @return
     */
    inline const Numerical::Double & operator*() const
    {
        return *m_data;
    }

    /**
     *
     * @return
     */
    inline const Numerical::Double * operator->() const
    {
        return m_data;
    }

    /**
     *
     * @return
     */
    inline ConstIterator & operator++()
    {
        next();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline ConstIterator & operator++(int)
    {
        next();
        return *this;
    }

    /**
     *
     * @return
     */
    inline ConstIterator & operator--()
    {
        prev();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline ConstIterator & operator--(int)
    {
        prev();
        return *this;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline ConstIterator & operator=(const ConstIterator & iterator)
    {
        copy(iterator);
        return *this;
    }

};

/**
 *
 */
class NonzeroIterator : public CommonIterator
{
public:

    /**
     *
     */
    NonzeroIterator() :
    CommonIterator()
    {
    }

    /**
     *
     * @param start
     * @param end
     * @param data
     * @param index
     * @param next
     */
    NonzeroIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data,
            unsigned int * index, bool next) :
    CommonIterator(start, end, data, index)
    {
        if (next) {
            //if (*start == 0.0) {
            if (*start == 0.0) {

                nextToNonzero();

            }
        }
    }

    /**
     *
     */
    virtual ~NonzeroIterator()
    {
    }

    /**
     *
     * @param original
     */
    NonzeroIterator(const Iterator & original) :
    CommonIterator(original)
    {
    }

    /**
     *
     * @return
     */
    inline Numerical::Double & operator*() const
    {
        return *m_data;
    }

    /**
     *
     * @return
     */
    inline Numerical::Double * operator->() const
    {
        return m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline NonzeroIterator & operator=(const NonzeroIterator & iterator)
    {
        copy(iterator);
        return *this;
    }

    /**
     *
     * @return
     */
    inline NonzeroIterator & operator++()
    {
        nextToNonzero();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline NonzeroIterator & operator++(int)
    {
        nextToNonzero();
        return *this;
    }

    /**
     *
     * @return
     */
    inline NonzeroIterator & operator--()
    {
        prevToNonzero();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline NonzeroIterator & operator--(int)
    {
        prevToNonzero();
        return *this;
    }

};

/**
 *
 */
class ConstNonzeroIterator : public CommonIterator
{
public:

    /**
     *
     */
    ConstNonzeroIterator() :
    CommonIterator()
    {
    }

    /**
     *
     * @param start
     * @param end
     * @param data
     * @param index
     * @param next
     */
    ConstNonzeroIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data,
            unsigned int * index, bool next) :
    CommonIterator(start, end, data, index)
    {
        //if (next && *start == 0.0)
        if (next && *start == 0.0)
            nextToNonzero();
    }

    /**
     *
     */
    virtual ~ConstNonzeroIterator()
    {
    }

    /**
     *
     * @param original
     */
    ConstNonzeroIterator(const Iterator & original) :
    CommonIterator(original)
    {
    }

    /**
     *
     * @return
     */
    inline const Numerical::Double & operator*() const
    {
        return *m_data;
    }

    /**
     *
     * @return
     */
    inline const Numerical::Double * operator->() const
    {
        return m_data;
    }

    /**
     *
     * @param iterator
     * @return
     */
    inline ConstNonzeroIterator & operator=(const ConstNonzeroIterator & iterator)
    {
        copy(iterator);
        return *this;
    }

    /**
     *
     * @return
     */
    inline ConstNonzeroIterator & operator++()
    {
        nextToNonzero();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline ConstNonzeroIterator & operator++(int)
    {
        nextToNonzero();
        return *this;
    }

    /**
     *
     * @return
     */
    inline ConstNonzeroIterator & operator--()
    {
        prevToNonzero();
        return *this;
    }

    /**
     *
     * @param
     * @return
     */
    inline ConstNonzeroIterator & operator--(int)
    {
        prevToNonzero();
        return *this;
    }
};

#endif /* IN_VECTOR */

#endif	/* VECTOR_ITERATOR_H */
