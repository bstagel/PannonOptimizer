/**
 * @file vector_iterator.h This file contains the API of the Iterator classes.
 * @author smidla
 */

#ifndef VECTOR_ITERATOR_H
#define	VECTOR_ITERATOR_H

#include <globals.h>

#ifdef IN_VECTOR

#include <utils/numerical.h>

/**
 * This class describes the basic functions needed by every Iterator classes.
 *
 * @class CommonIterator
 */
class CommonIterator
{
public:

    /**
     * Default constructor of the CommonIterator class.
     *
     * @constructor
     */
    CommonIterator() :
        m_start(0), m_end(0), m_data(0), m_index(0) {}

    /**
     * Constructor of the CommonIterator class able to set its properties individually.
     *
     * @constructor
     * @param start The first element of the iteration.
     * @param end The last element of the iteration.
     * @param data The currently pointed element of the iteration.
     * @param index The currently pointed index of the iteration.
     */
    CommonIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index) :
        m_start(start), m_end(end), m_data(data), m_index(index) {}

    /**
     * @return The index of the currently pointed element.
     */
    inline unsigned int getIndex() const
    {
        if (m_index)
            return *m_index;
        else
            return m_data - m_start;
    }

    /**
     * Returns true if the two CommonIterators point to the same element.
     *
     * @param iterator Reference of another CommonIterator.
     * @return True if the two CommonIterators point to the same element.
     */
    inline bool operator==(const CommonIterator & iterator) const
    {
        return m_data == iterator.m_data;
    }

    /**
     * Returns false if the two CommonIterators point to the same element.
     *
     * @param iterator Reference of another CommonIterator.
     * @return False if the two CommonIterators point to the same element.
     */
    inline bool operator!=(const CommonIterator & iterator) const
    {
        return m_data != iterator.m_data;
    }

    /**
     * Returns true if the other CommonIterator points to an element with smaller index.
     *
     * @param iterator Reference of another CommonIterator.
     * @return True if the other CommonIterator points to an element with
     * smaller index.
     */
    inline bool operator<(const CommonIterator & iterator) const
    {
        return m_data < iterator.m_data;
    }

    /**
     * Returns true if the other CommonIterator points to an element with greater index.
     *
     * @param iterator Reference of another CommonIterator.
     * @return True if the other CommonIterator points to an element with
     * greater index.
     */
    inline bool operator>(const CommonIterator & iterator) const
    {
        return m_data > iterator.m_data;
    }

    /**
     * Returns false if the other CommonIterator points to an element with greater index.
     *
     * @param iterator Reference of another CommonIterator.
     * @return True if the other CommonIterator points to an element with
     * smaller or equal index.
     */
    inline bool operator<=(const CommonIterator & iterator) const
    {
        return m_data <= iterator.m_data;
    }

    /**
     * Returns false if the other CommonIterator points to an element with smaller index.
     *
     * @param iterator Reference of another CommonIterator.
     * @return True if the other CommonIterator points to an element with
     * bigger or equal index.
     */
    inline bool operator>=(const CommonIterator & iterator) const
    {
        return m_data >= iterator.m_data;
    }

    /**
     * Returns a pointer to the currently pointed element.
     *
     * @return Pointer to the currently pointed element.
     */
    inline const Numerical::Double & operator*() const
    {
        return *m_data;
    }

//    inline Numerical::Double * operator->() const
//    {
//        return m_data;
//    }
protected:
    /**
     * Pointer to the first element of the iteration.
     */
    Numerical::Double * m_start;

    /**
     * Pointer to the last element of the iteration.
     */
    Numerical::Double * m_end;

    /**
     * Pointer to the currently selected element.
     */
    Numerical::Double * m_data;

    /**
     * Pointer to the currently selected index.
     * In the case of dense vectors, this is a null pointer.
     */
    unsigned int * m_index;

    /**
     * Jumps to the next element.
     */
    inline void next()
    {
        m_data++;
        if (m_index)
            m_index++;
    }

    /**
     * Selects the previous element.
     */
    inline void prev()
    {
        m_data--;
        if (m_index)
            m_index--;
    }

    /**
     * Jumps to the next nonzero element.
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
     * Jumps to the previous nonzero element.
     */
    inline void prevToNonzero()
    {
        do {
            m_data--;
            if (m_index)
                m_index--;
        } while (m_data >= m_start && *m_data == 0.0);
    }
};

/**
 * This class describes a classic iterator for the Vector class.
 *
 * @class Iterator
 */
class Iterator : public CommonIterator
{
public:
    /**
     * Constructor of the Iterator class able to set its properties individually.
     *
     * @constructor
     * @param start The first element of the iteration.
     * @param end The last element of the iteration.
     * @param data The currently pointed element of the iteration.
     * @param index The currently pointed index of the iteration.
     */
    Iterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index) :
        CommonIterator(start, end, data, index) {}

    /**
     * Jumps to the next element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline Iterator & operator++()
    {
        next();
        return *this;
    }

    /**
     * Jumps to the next element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline Iterator & operator++(int)
    {
        next();
        return *this;
    }

    /**
     * Jumps to the previous element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline Iterator & operator--()
    {
        prev();
        return *this;
    }

    /**
     * Jumps to the previous element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline Iterator & operator--(int)
    {
        prev();
        return *this;
    }
};

/**
 * This class describes a nonzero iterator for the Vector class.
 * A nonzero iterator can only point to nonzero elements of a vector.
 *
 * @class NonzeroIterator
 */
class NonzeroIterator : public CommonIterator
{
public:

    /**
     * Constructor of the NonzeroIterator class able to set its properties individually.
     * Automatically jumps to the first nonzero element.
     *
     * @constructor
     * @param start The first element of the iteration.
     * @param end The last element of the iteration.
     * @param data The currently pointed element of the iteration.
     * @param index The currently pointed index of the iteration.
     */
    NonzeroIterator(Numerical::Double * start, Numerical::Double * end, Numerical::Double * data, unsigned int * index, bool next) :
        CommonIterator(start, end, data, index)
    {
        if (next && *start == 0.0)
            nextToNonzero();
    }

    /**
     * Jumps to the next nonzero element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline NonzeroIterator & operator++()
    {
        nextToNonzero();
        return *this;
    }

    /**
     * Jumps to the next nonzero element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline NonzeroIterator & operator++(int)
    {
        nextToNonzero();
        return *this;
    }

    /**
     * Jumps to the previous nonzero element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline NonzeroIterator & operator--()
    {
        prevToNonzero();
        return *this;
    }

    /**
     * Jumps to the previous nonzero element of the vector.
     *
     * @return Reference to the Iterator.
     */
    inline NonzeroIterator & operator--(int)
    {
        prevToNonzero();
        return *this;
    }
};

#endif /* IN_VECTOR */

#endif	/* VECTOR_ITERATOR_H */
