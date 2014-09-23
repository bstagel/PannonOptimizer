/**
 * @file list.h This file contains the API of the List class.
 * @author Jozsef Smidla
 */


#ifndef LIST_H
#define	LIST_H

#include <globals.h>

#include <utils/elementproducer.h>
#include <cstring>

#define LIST_NODE_COUNT 10

/**
 * This class implements the controller object of the ElementProducer.
 *
 * @see ElementProducer
 *
 * @class List
 */
template <class T>
class List
{
public:
    class Iterator;

private:

    /**
     * This class represents a node of the list.
     */
    class Node
    {
        friend class List;
        friend class List::Iterator;

        /**
         * The data of the node.
         */
        T m_data;

        /**
         * Pointer to the next node in the list.
         */
        Node * m_next;

        /**
         * Pointer to the previous node in the list.
         */
        Node * m_previous;
    public:

        /**
         * Constructor of the Node class.
         *
         * @constructor
         */
        Node()
        {
            //memset(this, 0, sizeof(*this));
            m_next = 0;
            m_previous = 0;
        }
    };

    /**
     * Pointer to the header Node in the list.
     */
    Node * m_head;

    /**
     * Pointer to the last Node in the list.
     */
    Node * m_last;

    /**
     * The number of elements in the list.
     */
    unsigned int m_size;

    /**
     * The element producer of the list.
     * This is an accelerated linked list which utilizes the CPU cache.
     *
     * @see ElementProducer
     */
    ElementProducer<Node, LIST_NODE_COUNT> m_elementProducer;

public:

    /**
     * Default constructor of the List class.
     *
     * @constructor
     */
    inline List()
    {
        m_head = 0;
        m_last = 0;
        m_size = 0;
    }

    /**
     * Copy constructor of the List class.
     *
     * @param orig The other List object to be copied.
     *
     * @constructor
     */
    inline List(const List & orig)
    {
        m_head = 0;
        m_last = 0;
        m_size = 0;
        Node * temp = orig.m_head;
        while (temp) {
            pushBack(temp->m_data);
            temp = temp->m_next;
        }
    }

    /**
     * Destructor of the List class.
     *
     * @destructor
     */
    inline ~List()
    {

    }

    /**
     * Adds a new element at the end of the list.
     *
     * @param data The new element to be added.
     */
    inline void pushBack(T data)
    {
        Node * newNode = m_elementProducer.getNewElement();
        newNode->m_data = data;
        if (m_head == 0) {
            m_last = m_head = newNode;
        } else {
            m_last->m_next = newNode;
            newNode->m_previous = m_last;
            m_last = newNode;
        }
        m_size++;
    }

    /**
     * Returns the number of elements in the list.
     *
     * @return The number of elements in the list.
     */
    inline unsigned int size() const
    {
        return m_size;
    }

    /**
     * Iterator for the List class, helps to visit each element individually.
     * This uses the iterator of the ElementProducer class.
     *
     * @class Iterator
     */
    class Iterator
    {
        /**
         * The element producer iterator to be used.
         */
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator m_iterator;
    public:

        /**
         * Default constructor of the Iterator class.
         *
         * @constructor
         */
        inline Iterator()
        {
        }

        /**
         * Copy constructor of the Iterator class.
         *
         * @constructor
         * @param iterator The other element producer iterator to be copied.
         */
        inline Iterator(typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator & iterator)
        {
            m_iterator = iterator;
        }

        /**
         * Returns the data of the currently pointed element.
         *
         * @return The data of the currently pointed element.
         */
        inline T & getData() const
        {
            return m_iterator->m_data;
        }

        /**
         * Selects the next element in the list.
         *
         * @return Reference of the iterator pointing to the next element.
         */
        inline Iterator & operator++()
        {
            m_iterator++;
            return *this;
        }

        /**
         * Returns if the two Iterators point to the same element.
         *
         * @param iterator The other Iterator object.
         * @return True if the two objects point to the same element.
         */
        inline bool operator==(const Iterator & iterator) const
        {
            return m_iterator == iterator.m_iterator;
        }

        /**
         * Returns if the two Iterators point to the same element.
         *
         * @param iterator The other Iterator object.
         * @return False if the two objects point to the same element.
         */
        inline bool operator!=(const Iterator & iterator) const
        {
            return m_iterator != iterator.m_iterator;
        }

        /**
         * Returns the data of the currently pointed element.
         *
         * @return The data of the currently pointed element.
         */
        inline T & getData()
        {
            return m_iterator->m_data;
        }

    };

    /**
     * Returns an iterator pointing to the first element of the list.
     *
     * @return An iterator pointing to the first element of the list.
     */
    inline Iterator begin() const
    {
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator iterator = m_elementProducer.begin();
        return Iterator(iterator);
    }

    /**
     * Returns an iterator pointing to the last element of the list.
     *
     * @return An iterator pointing to the last element of the list.
     */
    inline Iterator end() const
    {
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator iterator = m_elementProducer.end();
        return Iterator(iterator);
    }

};

#endif	/* LIST_H */

