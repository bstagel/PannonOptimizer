/* 
 * File:   elementproducer.h
 * Author: smidla
 *
 * Created on 2011. február 22., 14:49
 */

#ifndef ELEMENTPRODUCER_H
#define	ELEMENTPRODUCER_H

template <class T, int N>
class ElementProducer
{

    struct ElementList
    {
        int m_index;
        T m_elements[N];
        ElementList * m_next;
    };
    ElementList * m_head;
    ElementList * m_actual;
public:

    class Iterator
    {
        ElementList * m_actualList;
        T * m_actualElement;
        T * m_lastElement;
    public:

        inline Iterator()
        {

        }

        inline Iterator(ElementList * list, T * element, T * last)
        {
            m_actualList = list;
            m_actualElement = element;
            m_lastElement = last;
        }

        inline T & operator*() const
        {
            return * m_actualElement;
        }

        inline T * operator->() const
        {
            return m_actualElement;
        }

        inline Iterator & operator++()
        {
            m_actualElement++;
            if (m_actualElement >= m_lastElement) {
                m_actualList = m_actualList->m_next;
                if (m_actualList != 0) {
                    m_actualElement = m_actualList->m_elements;
                    m_lastElement = m_actualElement + N;
                } else {
                    m_actualElement = 0;
                }
            }
            return *this;
        }

        inline Iterator & operator++(int)
        {
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
            return *this;
        }

        inline bool operator==(const Iterator & iterator) const
        {
            return m_actualElement == iterator.m_actualElement;
        }

        inline bool operator!=(const Iterator & iterator) const
        {
            return m_actualElement != iterator.m_actualElement;
        }

    };

    inline ElementProducer()
    {
        m_head = m_actual = 0;
    }

    inline ElementProducer(const ElementProducer & orig)
    {

    }

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

    inline T * getNewElement()
    {
        if (m_head == 0) {
            m_head = m_actual = new ElementList; // (ElementList*) malloc(sizeof (ElementList));
            //memset(hkp._head->_keys, 0, MPS_KEY_SIZE * KEY_COUNT);
            m_head->m_index = 1;
            m_head->m_next = 0;
            return m_head->m_elements;
        } else {
            if (m_actual->m_index >= N) {
                //std::cout<<"uj lista lefoglalasa"<<std::endl;
                //cin.get();
                m_actual->m_next = new ElementList; // (ElementList*) malloc(sizeof (ElementList));
                m_actual = m_actual->m_next;
                //memset(hkp._actual->_keys, 0, MPS_KEY_SIZE * KEY_COUNT);
                m_actual->m_index = 0;
                m_actual->m_next = 0;
            }
            T * res = m_actual->m_elements + m_actual->m_index;
            m_actual->m_index++;
            return res;
        }
        return 0;
    }

    inline Iterator begin() const
    {
        if (m_head == 0)
            return end();
        return Iterator(m_head, m_head->m_elements, m_head->m_elements + m_head->m_index);
    }

    inline Iterator end() const
    {
        return Iterator(0, 0, 0);
    }

};

#endif	/* ELEMENTPRODUCER_H */

