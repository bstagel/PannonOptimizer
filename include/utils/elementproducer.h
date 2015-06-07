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

/**
 * @file elementproducer.h This file contains the API of the ElementProducer class.
 * @author Jozsef Smidla
 */


#ifndef ELEMENTPRODUCER_H
#define	ELEMENTPRODUCER_H

#include <globals.h>

/**
 * The class implements an accelerated linked list.
 * The elements of the linked lists are collected into groups which contain N elements.
 * One group is stored as an array, so inserting new element into the list is faster,
 * and this structure utilises the CPU cache.
 *
 * @class ElementProducer
 */
template <class T, int N>
class ElementProducer
{

    /**
     * This structure stores one list element group in an array.
     */
    struct ElementList
    {
        /**
         * The index of the group.
         */
        int m_index;

        /**
         * This array stores N piece of data.
         */
        T m_elements[N];

        /**
         * Pointer to the next group.
         */
        ElementList * m_next;
    };

    /**
     * Pointer to the first group.
     */
    ElementList * m_head;

    /**
     * Pointer to the actual group.
     */
    ElementList * m_actual;
public:

    /**
     * This class helps to visit every elements in the list.
     *
     * @class Iterator
     */
    class Iterator
    {
        /**
         * Pointer to the current group.
         */
        ElementList * m_actualList;

        /**
         * Pointer to the current element.
         */
        T * m_actualElement;

        /**
         * Pointer to the lass element in the current group.
         */
        T * m_lastElement;
    public:

        /**
         * Default constructor, initializes every members to zero.
         *
         * @constructor
         */
        inline Iterator():
            m_actualList(0),
            m_actualElement(0),
            m_lastElement(0)
        {

        }

        /**
         * Helper constructor, it is called by some functions of class List.
         *
         * @constructor
         * @param list Pointer to a list group.
         * @param element Pointer to an element in the group.
         * @param last Pointer the the last element in the group.
         */
        inline Iterator(ElementList * list, T * element, T * last)
        {
            m_actualList = list;
            m_actualElement = element;
            m_lastElement = last;
        }

        /**
         * Returns with the current element.
         *
         * @return Reference to the current element.
         */
        inline T & operator*() const
        {
            return *m_actualElement;
        }

        /**
         * Returns with the current element's pointer.
         *
         * @return Pointer to the current element.
         */
        inline T * operator->() const
        {
            return m_actualElement;
        }

        /**
         * Steps to the next element in the list.
         * When the function reaches the end of the current group, it steps
         * to the beginning of the next group. If the next group does not exists,
         * the m_actualElement will be zero.
         *
         * @return Refernece to the modified iterator.
         */
        inline Iterator & operator++()
        {
            next();
            return *this;
        }

        /**
         * Steps to the next element in the list.
         * When the function reaches the end of the current group, it steps
         * to the beginning of the next group. If the next group does not exists,
         * the m_actualElement will be zero.
         */
        inline void next() {
            m_actualElement++;
            if (m_actualElement >= m_lastElement) {
                m_actualList = m_actualList->m_next;
                if (m_actualList != 0) {
                    m_actualElement = m_actualList->m_elements;
                    m_lastElement = m_actualElement + m_actualList->m_index;
                } else {
                    m_actualElement = 0;
                }
            }
        }

        /**
         * Returns with true, if the two iterators point to same element.
         *
         * @param iterator Reference to the other iterator.
         * @return True, if the iterators point to the same element, otherwise returs
         * with false.
         */
        inline bool operator==(const Iterator & iterator) const
        {
            return m_actualElement == iterator.m_actualElement;
        }

        /**
         * Returns with true, if the two iterators point to different elements.
         *
         * @param iterator Reference to the other iterator.
         * @return False, if the iterators point to the same element, otherwise returs
         * with true.
         */
        inline bool operator!=(const Iterator & iterator) const
        {
            return m_actualElement != iterator.m_actualElement;
        }

    };

    /**
     * Default constructor, it initializes an empty list.
     *
     * @constructor
     */
    inline ElementProducer()
    {
        m_head = m_actual = 0;
    }

    /**
     * Copy constructor.
     *
     * @constructor
     * @param orig The original list.
     */
    // TODO: implement it!
    inline ElementProducer(const ElementProducer & orig)
    {

    }

    /**
     * Assignment operator.
     *
     * @param orig The original list.
     * @return Refernce to the current object.
     */
    // TODO: implement it!
    inline ElementProducer & operator=(const ElementProducer & orig) {

    }

    /**
     * Destructor of the class.
     *
     * @destructor
     */
    inline ~ElementProducer()
    {
        ElementList * temp = m_head;
        ElementList * next;
        while (temp) {
            next = temp->m_next;
            delete temp;
            temp = next;
        }
    }

    /**
     * Returns with a new T type element.
     *
     * @return Pointer of the new element.
     */
    inline T * getNewElement()
    {
        if (m_head == 0) {
            m_head = m_actual = new ElementList;
            m_head->m_index = 1;
            m_head->m_next = 0;
            return m_head->m_elements;
        } else {
            if (m_actual->m_index >= N) {
                m_actual->m_next = new ElementList;
                m_actual = m_actual->m_next;
                m_actual->m_index = 0;
                m_actual->m_next = 0;
            }
            T * res = m_actual->m_elements + m_actual->m_index;
            m_actual->m_index++;
            return res;
        }
        return 0;
    }

    /**
     * Returns with an iterator which points to the first element in the list.
     *
     * @return Iterator of the first element.
     */
    inline Iterator begin() const
    {
        if (m_head == 0)
            return end();
        return Iterator(m_head, m_head->m_elements, m_head->m_elements + m_head->m_index);
    }

    /**
     * Returns with an iterator which represents the end of the list.
     *
     * @return Iterator to the end of the list.
     */
    inline Iterator end() const
    {
        return Iterator(0, 0, 0);
    }

};

#endif	/* ELEMENTPRODUCER_H */

