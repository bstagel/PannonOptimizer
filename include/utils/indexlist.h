/**
 * @file linkedlist.h
 */


#ifndef LINKEDLIST_H
#define	LINKEDLIST_H

#include "globals.h"

#include <set>
#include <iostream>
using namespace std;

/**
 * The class stores double linked lists of indices.
 * Each linked list has a header. The next and previous element
 * of an empty linked list is the header. When the linked is not empty,
 * the next element of the last element is the header, and the previous
 * element of first element is the header also.
 * 
 */
class IndexList
{
    friend class IndexListTestSuite;

    /**
     * Stores an element of the linked list.
     */
    struct Element
    {
        /**
         * This variable represents the stored index.
         */
        unsigned int m_data;

        /**
         * Link to the next element in the linked list.
         */
        Element * m_next;

        /**
         * Link to the previous element in the linked list.
         */
        Element * m_previous;

        /**
         * When the element represnts a header, it is true.
         */
        bool m_isHeader;

        /**
         * The current element belongs to the m_partitionIndex'th partition.
         * When an index does not belongs to a partition, m_partitionIndex is
         * greater or equal than the number of partitions.
         */
        unsigned int m_partitionIndex;

        /**
         * A pointer to an optional object.
         */
        const void * m_pointer;
    };

public:

    /**
     * Constructor of IndexList. Initializes the linked lists
     * to empty. The count and partitions can be zero.
     * <hr>
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>count</th>
     *   <th>partitions</th>
     *   <th>complexity</th>
     *   <th>validity</th>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> zero </td>
     *   <td> O(1) </td>
     *   <td> valid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> zero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> nonzero </td>
     *   <td> O(count + partitions) </td>
     *   <td> valid </td>
     * </tr>
     * </table>
     * 
     * @param count Number of possible indices.
     * @param partitions Number of linked lists.
     */
    IndexList(unsigned int count = 0, unsigned int partitions = 0);

    /**
     * Copy constructor of IndexList.
     * <hr>
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>list.count</th>
     *   <th>list.partitions</th>
     *   <th>complexity</th>
     *   <th>validity</th>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> zero </td>
     *   <td> O(1) </td>
     *   <td> valid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> zero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> nonzero </td>
     *   <td> O(list.count + list.partitions) </td>
     *   <td> valid </td>
     * </tr>
     * </table>
     * 
     * @param list The original list.
     */
    IndexList(const IndexList & list);

    /**
     * Assignment operator of IndexList.
     * <hr>
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>list.count</th>
     *   <th>list.partitions</th>
     *   <th>complexity</th>
     *   <th>validity</th>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> zero </td>
     *   <td> O(1) </td>
     *   <td> valid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> zero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> nonzero </td>
     *   <td> O(list.count + list.partitions) </td>
     *   <td> valid </td>
     * </tr>
     * </table>
     * 
     * @param list The original list object.
     * @return Reference of the actual list object.
     */
    IndexList & operator=(const IndexList & list);

    /**
     * Destructor of IndexList.
     * <hr>
     * Complexity: O(1)
     */
    ~IndexList();

    /**
     * Returns with the number of linked lists.
     * <hr>
     * Complexity: O(1)
     * 
     * @return number of linked lists
     */
    inline unsigned int getPartitionCount() const
    {
        return m_partitionCount;
    }

    /**
     * Returns with the number of possible indices.
     * <hr>
     * Complexity: O(1)
     * @return number of possible indices
     */
    inline unsigned int getIndexCount() const
    {
        return m_count;
    }

    /**
     * Initializes the linked lists and the set of possible
     * indices. The possible indices are nonnegative values,
     * and less than count.
     * <hr>
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>count</th>
     *   <th>partitions</th>
     *   <th>complexity</th>
     *   <th>validity</th>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> zero </td>
     *   <td> O(1) </td>
     *   <td> valid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> zero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> - </td>
     *   <td> invalid </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> nonzero </td>
     *   <td> O(count + partitions) </td>
     *   <td> valid </td>
     * </tr>
     * </table>
     * 
     * @param count Number of possible indices.
     * @param partitions Number of linked lists.
     */
    void init(unsigned int count, unsigned int partitions);

    /**
     * Inserts an index to the linked list given by argument partitionIndex.
     * Value most not be any of linked list.
     * <hr>
     * Complexity: O(1)
     * 
     * @param partitionIndex The index will be inserted to this linked list.
     * @param index This index will be inserted.
     */
    inline void insert(unsigned int partitionIndex, unsigned int value, const void * pointer = 0)
    {
        Element * forward = m_heads[partitionIndex].m_next;
        m_heads[partitionIndex].m_next = m_dataArray + value;
        m_dataArray[value].m_next = forward;
        m_dataArray[value].m_partitionIndex = partitionIndex;
        m_dataArray[value].m_pointer = pointer;
        forward->m_previous = m_dataArray + value;
        m_dataArray[value].m_previous = m_heads + partitionIndex;
    }

    /**
     * Sets the pointer of the value'th element.
     *
     * @param value
     * @param pointer
     */
    inline void setPointer(unsigned int value, const void * pointer) {
        m_dataArray[value].m_pointer = pointer;
    }

    /**
     *
     * @param value
     * @return
     */
    inline const void * getPointer(unsigned int value) const {
        return m_dataArray[value].m_pointer;
    }

    /**
     * Removes the index from the linked lists.
     * <hr>
     * Complexity: O(1)
     * 
     * @param index This index will be removed.
     */
    inline void remove(unsigned int value)
    {
        Element & element = m_dataArray[value];
        element.m_partitionIndex = m_partitionCount;
        element.m_previous->m_next = element.m_next;
        element.m_next->m_previous = element.m_previous;
    }

    /**
     *
     * @param value
     * @param targetPartition
     */
    inline void move(unsigned int value, unsigned int targetPartition) {
        Element & element = m_dataArray[value];
        element.m_partitionIndex = targetPartition;

        Element * forward = m_heads[targetPartition].m_next;
        m_heads[targetPartition].m_next = &element;
        element.m_next = forward;
        forward->m_previous = &element;
        element.m_previous = m_heads + targetPartition;
    }

    /**
     * Returns the number of partition, where the index can
     * be found.
     *
     * @param index The function returns with the partition index of this value.
     * @return The partition index.
     *
     * @see m_partitionIndex
     */
    inline unsigned int where(unsigned int value) const {
        return m_dataArray[value].m_partitionIndex;
    }

    /**
     * Iterator class for listing elements of a linked list.
     */
    class Iterator
    {
        /**
         * Pointer to an element of a linked list.
         */
        Element * m_actual;

        std::set<Element*> m_borders;
    public:

        /**
         * Default constructor of class Iterator.
         * <hr>
         * Complexity: O(1)
         */
        inline Iterator()
        {
            m_actual = 0;
        }

        /**
         * Constructor of class Iterator. Sets the pointer to an element
         * of a linked list.
         * <hr>
         * Complexity: O(1)
         * 
         * @param actual The value of pointer of actual element in linked list.
         */
        inline Iterator(Element * actual, const std::set<Element*> & borders)
        {
            m_actual = actual;
            m_borders = borders;
        }

        /**
         * When the iterator refers to the header of linked list, 
         * this functions returns the index of linked list, otherwise
         * returns the index of actual element in linked list.
         * <hr>
         * Complecity: O(1)
         *
         * @return Index of linked list or actual element.
         */
        inline unsigned int getData() const
        {
            return m_actual->m_data;
        }

        /**
         * Returns with the stored pointer of the current element.
         *
         * @return The stored pointer.
         */
        inline const void * getPointer() const {
            return m_actual->m_pointer;
        }

        /**
         * Returns with the partition index of the current element.
         *
         * @return Partition index of the current element.
         */
        inline unsigned int getPartitionIndex() const {
            return m_actual->m_partitionIndex;
        }

        /**
         * Moves the iterator to the next element. When the iterator
         * refers to the last element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         */
        inline void next()
        {
            if (m_actual) {
                m_actual = m_actual->m_next;
                if (m_actual->m_isHeader == true) {
                    std::set<Element*>::iterator iter = m_borders.find(m_actual);
                    if (iter != m_borders.end()) {
                        iter++; // next border
                        if (iter != m_borders.end()) {
                            m_actual = (*iter)->m_next;
                            bool end = !m_actual->m_isHeader;
                            while (end == false) {
                                iter = m_borders.find(m_actual);
                                if (iter != m_borders.end()) {
                                    iter++;
                                    if (iter != m_borders.end()) {
                                        m_actual = (*iter)->m_next;
                                        end = !m_actual->m_isHeader;
                                    } else {
                                        end = true;
                                    }
                                } else {
                                    end = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        /**
         * Moves the iterator to the previous element. When the iterator
         * refers to the first element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         */
        inline void previous()
        {
            if (m_actual) {
                m_actual = m_actual->m_previous;
            }
        }

        /**
         * Moves the iterator to the next element. When the iterator
         * refers to the last element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         * 
         * @return Reference to the iterator object.
         */
        inline Iterator & operator++()
        {
            next();
            return *this;
        }

        /**
         * Moves the iterator to the next element. When the iterator
         * refers to the last element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         * 
         * @param not used
         * @return Reference to the iterator object.
         */
        inline Iterator & operator++(int)
        {
            next();
            return *this;
        }

        /**
         * Moves the iterator to the previous element. When the iterator
         * refers to the first element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         * 
         * @return Reference to the iterator object.
         */
        inline Iterator & operator--()
        {
            previous();
            return *this;
        }

        /**
         * Moves the iterator to the previous element. When the iterator
         * refers to the first element, the iterator steps to the header.
         * <hr>
         * Complexity: O(1)
         * 
         * @param not used
         * @return Reference to the iterator object.
         */
        inline Iterator & operator--(int)
        {
            previous();
            return *this;
        }

        /**
         * Returns true when the iter and current Iterator refer to the same
         * list element.
         * <hr>
         * Complexity: O(1)
         * 
         * @param iter
         * @return True, when the 2 iterators refer to the same list element.
         */
        inline bool operator==(const Iterator & iter)
        {
            return m_actual == iter.m_actual;
        }

        /**
         * Returns true when the iter and current Iterator refer to different
         * list elements.
         * <hr>
         * Complexity: O(1)
         * 
         * @param iter
         * @return True, when the 2 iterators refer to different list element.
         */
        inline bool operator!=(const Iterator & iter)
        {
            return m_actual != iter.m_actual;
        }

    };

    void getIterators(Iterator * begin, Iterator * end, unsigned int partitionIndex,
        unsigned int partitions = 1) const
    {
        unsigned int lastPartitionIndex = partitionIndex + partitions - 1;
        Element * beginHead = m_heads + partitionIndex;
        Element * endHead = m_heads + lastPartitionIndex;
        std::set<Element*> borders;
        *end = Iterator(endHead, borders);

        unsigned int index = partitionIndex;
        for (; index <= lastPartitionIndex; index++) {
            borders.insert(m_heads + index);
        }
        *begin = Iterator(beginHead->m_next, borders);

    }

    
    /**
     * Returns an iterator. When the indexth linked list contains indices,
     * the iterator refers to the first element of the list, otherwise it
     * refers to the list head.
     * <hr>
     * Complexity: O(1)
     * 
     * @param index The index of a linked list.
     * @return Iterator to the first element or header.
     */
    /*Iterator begin(unsigned int index) const
    {
        Element * head = m_heads + index;
        return Iterator(); //(head->m_next, 0, 0);
    }*/

    /**
     * Returns an iterator refers to the head of the indexth linked list.
     * <hr>
     * Complexity: O(1)
     * 
     * @param index The index of a linked list.
     * @return Iterator to the header.
     */
    /*Iterator end(unsigned int index) const
    {
        Element * head = m_heads + index;
        return Iterator(); //(head, 0, 0);
    }*/

    inline void clearPartition(unsigned int index)
    {
        Element * element = m_heads + index;
        element->m_next = element;
        element->m_previous = element;
    }

private:

    /**
     * Number of linked lists.
     */
    unsigned int m_partitionCount;

    /**
     * Pointer to the array of header pointers. The size of array
     * is m_partitionCount.
     */
    Element * m_heads;

    /**
     * Number of possible indices.
     */
    unsigned int m_count;

    /**
     * Pointer to the array of indices. The size of array is
     * m_count.
     */
    Element * m_dataArray;

    /**
     * Releases the arrays, and sets to zero each variable.
     * <hr>
     * Complexity: O(1)
     */
    void clear();

    /**
     * Copies the list.
     * <hr>
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>list.m_count</th>
     *   <th>complexity</th>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> O(list.m_count + list.m_partitions) </td>
     * </tr>
     * </table>
     * 
     * @param list
     */
    void copy(const IndexList & list);

};

std::ostream & operator << (std::ostream & os, const IndexList & list);

#endif	/* LINKEDLIST_H */
