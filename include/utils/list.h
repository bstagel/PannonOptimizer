/**
 * @file list.h
 */


#ifndef LIST_H
#define	LIST_H

#include <globals.h>

#include <utils/elementproducer.h>
#include <cstring>

#define LIST_NODE_COUNT 10

template <class T>
class List
{
public:
    class Iterator;

private:
    class Node
    {
        friend class List;
        friend class List::Iterator;
        T m_data;
        Node * m_next;
        Node * m_previous;
    public:

        Node()
        {
            //memset(this, 0, sizeof(*this));
            m_next = 0;
            m_previous = 0;
        }
    };
    Node * m_head;
    Node * m_last;
    unsigned int m_size;
    ElementProducer<Node, LIST_NODE_COUNT> m_elementProducer;

public:

    inline List()
    {
        m_head = 0;
        m_last = 0;
        m_size = 0;
    }

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

    inline ~List()
    {

    }

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

    inline unsigned int size() const
    {
        return m_size;
    }

    class Iterator
    {
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator m_iterator;
    public:

        inline Iterator()
        {
        }

        inline Iterator(typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator & iterator)
        {
            m_iterator = iterator;
        }

        inline T & getData() const
        {
            return m_iterator->m_data;
        }

        inline Iterator & operator++()
        {
            m_iterator++;
            return *this;
        }

        inline Iterator & operator++(int)
        {
            m_iterator++;
            return *this;
        }

        inline bool operator==(const Iterator & iterator) const
        {
            return m_iterator == iterator.m_iterator;
        }

        inline bool operator!=(const Iterator & iterator) const
        {
            return m_iterator != iterator.m_iterator;
        }

        inline T & getData()
        {
            return m_iterator->m_data;
        }

    };

    inline Iterator begin() const
    {
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator iterator = m_elementProducer.begin();
        return Iterator(iterator);
    }

    inline Iterator end() const
    {
        typename ElementProducer<Node, LIST_NODE_COUNT>::Iterator iterator = m_elementProducer.end();
        return Iterator(iterator);
    }

};

#endif	/* LIST_H */

