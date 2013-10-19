/**
 * @file heap.h
 */


#ifndef HEAP_H
#define	HEAP_H

#include <globals.h>

#include <limits>
#include <iostream>

using namespace std;

template <class T>
class NoMove
{
public:

    static void move(T & , const unsigned int, void * )
//    static void move(T & obj, const unsigned int to, void * ptr)
    {

    }
};

/**
 * 
 * @param size
 */
template <class K, class T, class MOVE = NoMove<T> >
class Heap
{
public:

    /**
     * 
     * @param size
     */
    Heap(unsigned int size, void * ptr = 0)
    {
        m_size = 0;
        m_capacity = size + 1;
        m_data = new Element[m_capacity];
        for (int i = 0; i < 1; i++) {
            m_data[i].m_data = 0;
            m_data[i].m_data += 5;
        }
        m_actual = m_data + 1;
        m_pointer = ptr;
    }

    /**
     * 
     * @param orig
     */
    Heap(const Heap & orig)
    {
        copy(orig);
    }

    /**
     * 
     * @param orig
     * @return 
     */
    Heap & operator=(const Heap & orig)
    {
        clear();
        copy(orig);
    }

    /**
     * 
     */
    ~Heap()
    {
        clear();
    }

    void startBuild()
    {
        m_actual = m_data + 1;
        m_size = 0;
    }

    void addForBuild(const K & key, const T & data)
    {
        m_actual->m_key = key;
        m_actual->m_data = data;
        m_actual++;
        m_size++;
    }

    void build()
    {
        unsigned int from = m_size >> 1;
        const Element * middleElement = m_data + from + 1;
        int i;
        Element * leftElement;
        Element * rightElement;
        Element * actualElement;
        Element * maxElement;
        switch (m_size) {
            case 2:
                if (m_data[1].m_key < m_data[2].m_key) {
                    Element temp = m_data[1];
                    m_data[1] = m_data[2];
                    m_data[2] = temp;
                    MOVE::move(m_data[1].m_data, 1, m_pointer);
                    MOVE::move(m_data[2].m_data, 0, m_pointer);
                }
                return;
            case 1:
            case 0:
//                if (check() == false) {
//                    cout << "on ellenorzes hibat talalt a build elejen " << endl;
//                    cin.get();
//                }

                return;
        }

        /*for (i = (int) from; i > (int) from; i--) {
            unsigned int index = i;
            actualElement = m_data + index;
            unsigned int left, right;
            if (i <= 0) {
                break;
            }
            do {
                left = index << 1;
                right = left | 0x1;
                leftElement = m_data + left;
                rightElement = leftElement + 1;
                actualElement = m_data + index;
                maxElement = actualElement;

                if (left < m_size && leftElement->m_key > actualElement->m_key) {
                    maxElement = leftElement;
                }

                if (right < m_size && rightElement->m_key > maxElement->m_key) {
                    maxElement = rightElement;
                }

                if (maxElement != actualElement) {
                    MOVE::move(maxElement->m_data, actualElement - m_data - 1);
                    MOVE::move(actualElement->m_data, maxElement - m_data - 1);
                    Element temp = *actualElement;
                    *actualElement = *maxElement;
                    *maxElement = temp;
                    index = maxElement - m_data;
                    //cout << "index: " << index << endl;
                }
            } while (maxElement != actualElement && maxElement < middleElement);

        }*/
        Element * endElement = m_data + m_size + 1;
        for (i = from; i > 0; i--) {
            unsigned int index = i;
            actualElement = m_data + index;
            do {
                leftElement = m_data + (index << 1);
                rightElement = leftElement + 1;
                actualElement = m_data + index;
                maxElement = actualElement;

                if (leftElement < endElement && leftElement->m_key > actualElement->m_key) {
                    maxElement = leftElement;
                }

                if (rightElement < endElement && rightElement->m_key > maxElement->m_key) {
                    maxElement = rightElement;
                }

                if (maxElement != actualElement) {
                    MOVE::move(maxElement->m_data, actualElement - m_data - 1, m_pointer);
                    MOVE::move(actualElement->m_data, maxElement - m_data - 1, m_pointer);
                    Element temp = *actualElement; //m_data[index];
                    *actualElement = *maxElement;
                    *maxElement = temp;
                    index = maxElement - m_data;
                }
            } while (maxElement != actualElement && maxElement < middleElement);

        }
//        if (check() == false) {
//            cout << "on ellenorzes hibat talalt a build vegen " << endl;
//            cin.get();
//        }

    }

    inline unsigned int size() const
    {
        return m_size;
    }

    inline K getMax() const
    {
        return m_data[1].m_key;
    }

    inline K getKey(unsigned int index) const
    {
        return m_data[index + 1].m_key;
    }

    inline T getData(unsigned int index) const
    {
        return m_data[index + 1].m_data;
    }

    inline void setData(unsigned int index, const T & data) {
        m_data[index + 1].m_data = data;
    }
    
    void insert(const K & key, const T & data, bool debug = false)
    {
        //        static int count = 0;
        //count++;

        m_size++;
        Element * actualElement = m_data + m_size;
        unsigned int index = m_size >> 1;
        Element * parentElement = m_data + (index);
        //        if (count == 4) {
        //            cout << "size: " << m_size << endl;
        //            cout << "index: " << index << endl;
        //            cout << "key: " << key << endl;
        //            cout << "parent element: (" << parentElement->m_key << "; " << parentElement->m_data << ")" << endl;
        //        }

        m_actual++;
        const Element * firstElement = m_data + 1;

        while (actualElement > firstElement && parentElement->m_key < key) {
            //cout << "ciklusba belep" << endl;
            //            if (count == 4) {
            //                cout << "parent key: " << parentElement->m_key << endl;
            //                cout << "ezt mozgatom " << parentElement->m_data->m_index << endl;
            //                cout << "ide: " << actualElement - m_data - 1 << endl;
            //                //cout << count << endl;
            //
            //            }
            //            cout << actualElement - m_data << " " << parentElement - m_data << endl;
            MOVE::move(parentElement->m_data, actualElement - m_data - 1, m_pointer);
            *actualElement = *parentElement;
            actualElement = parentElement;
            index >>= 1;
            parentElement = m_data + index;
            //            cout << actualElement - m_data << " " << parentElement - m_data << endl;
            //            cout << "****************" << endl;
            //MOVE::move(actualElement->m_data, index - 1);
        }
        actualElement->m_key = key;
        actualElement->m_data = data;
        //        if (actualElement - m_data - 1 == 250) {
        //            cout << "masodik move" << endl;
        //        }

        MOVE::move(actualElement->m_data, actualElement - m_data - 1, m_pointer);
//        if (check() == false) {
//            cout << "on ellenorzes hibat talalt az insertnel " << endl;
//            cin.get();
//        }

    }

    void show() const
    {
        unsigned int i;
        for (i = 1; i <= m_size; i++) {
            std::cout << i << ".: ";
            std::cout << m_data[i].m_key << "; " << m_data[i].m_data << std::endl;
        }
    }

    void remove(unsigned int index)
    {
//        static int counter = 0;
//        counter++;

        const K minKey = numeric_limits<K>::min();
        // setting removable element to minKey, and
        // call heapify
        //        LPINFO("index: " << index);
        index++;
        Element * leftElement;
        Element * rightElement;
        Element * actualElement;
        Element * maxElement;
        Element * endElement = m_data + m_size;
        actualElement = m_data + index;
        actualElement->m_key = minKey;
        Element temp;
        //if (index > 1) {

//        if (counter == 347) {
//            cout << "ezt kell a vegere helyezni: " << *actualElement << endl;
//            cout << "a 144-es es 72-es elemek: " << m_data[144] << " " << m_data[72] << endl;
//        }

        MOVE::move(actualElement->m_data, m_size - 1, m_pointer);
        MOVE::move(endElement->m_data, index - 1, m_pointer);
        temp = *actualElement;
        *actualElement = *endElement;
        *endElement = temp;
        // elofordulhat, hogy a szulonel nagyobb elem kerul ide
        if (index > 1 && actualElement->m_key > m_data[index >> 1].m_key) {
            keyIncrement(index, actualElement->m_key);
            m_size--;
            endElement--;
//            if (check() == false) {
//                cout << "on ellenorzes hibat talalt a keyIncrementnel removeban" << endl;
//                cin.get();
//            }
            return;
        }
//        if (counter == 347) {
//            cout << "m_size: " << m_size << endl;
//            cout << "csere utan aktualis: " << *actualElement << endl;
//            cout << "index: " << index << endl;
//            cout << "bal gyerek: " << m_data[index * 2] << endl;
//            cout << "jobb gyerek: " << m_data[index * 2 + 1] << endl;
//            cout << "a 144-es es 72-es elemek: " << m_data[144] << " " << m_data[72] << endl;
//        }

        //        LPWARNING(actualElement->m_key << " " << actualElement->m_data << " " << actualElement - m_data);
        //}
        /*if (index > 1) {
            unsigned int index2 = index >> 1;
            if (m_data[index2].m_key < actualElement->m_key) {
                index = index2;
                actualElement = m_data + index;
            }
        }*/
        m_size--;
        endElement--;

        do {
            leftElement = m_data + (index << 1);
            rightElement = leftElement + 1;
            actualElement = m_data + index;
            maxElement = actualElement;

//            if (counter == 347) {
//                cout << endl << endl;
//                cout << "a 144-es es 72-es elemek: " << m_data[144] << " " << m_data[72] << endl;
//                cout << "csere utan aktualis: " << *actualElement << endl;
//                cout << "index: " << index << endl;
//                if (index * 2 < m_size)
//                    cout << "bal gyerek: " << m_data[index * 2] << endl;
//                if (index * 2 + 1 < m_size)
//                    cout << "jobb gyerek: " << m_data[index * 2 + 1] << endl;
//
//            }



            //cout << *actualElement << " " << *leftElement << " " << *rightElement << endl;

            if (leftElement <= endElement && leftElement->m_key > actualElement->m_key) {
                maxElement = leftElement;
////                if (counter == 347) {
////                    cout << "bal gyerek a nagyobb" << endl;
////                }
            }

            if (rightElement <= endElement && rightElement->m_key > maxElement->m_key) {
                maxElement = rightElement;
//                if (counter == 347) {
//                    cout << "de a jobb gyerek meg nagyobb" << endl;
//                }

            }

            if (maxElement != actualElement) {
                //cout << "rearranging" << endl;
                MOVE::move(maxElement->m_data, actualElement - m_data - 1, m_pointer);
                MOVE::move(actualElement->m_data, maxElement - m_data - 1, m_pointer);

                //                MOVE::move(maxElement->m_data, index - 1);
                temp = *actualElement; //m_data[index];
                *actualElement = *maxElement;
                *maxElement = temp;
                index = maxElement - m_data;
                //                MOVE::move(actualElement->m_data, index - 1);
            }
        } while (maxElement != actualElement);
        //m_size--;
//        if (check() == false) {
//            cout << "on ellenorzes hibat talalt a torlesnel " << counter << endl;
//            cin.get();
//        }
    }

    void changeKey(unsigned int index, const K & key)
    {
        index++;
        //        cout << index << " " << key << " -> " << m_data[index].m_key << endl;
        if (m_data[index].m_key < key) {
            keyIncrement(index, key);
//            if (check() == false) {
//                cout << "on ellenorzes hibat talalt a keyIncrementnel " << endl;
//                cin.get();
//            }
            return;
        }
        //        if (index > 1) { // nem gyerek
        //            cout << "szulo: " << m_data[index >> 1].m_key << endl;
        //            cout << "bal gyerek: " << m_data[index * 2 ].m_key << endl;
        //            cout << "jobb gyerek: " << m_data[index * 2 + 1].m_key << endl;
        //        }
        Element * leftElement;
        Element * rightElement;
        Element * actualElement;
        Element * maxElement;
        Element * endElement = m_data + m_size;
        actualElement = m_data + index;
        actualElement->m_key = key;
        Element temp;

        /*if (index > 1) {
            unsigned int parent = index >> 1;
            if (m_data[parent].m_key < actualElement->m_key) {
                while (parent > 0 && m_data[parent].m_key < actualElement->m_key) {
                    cout << "a szulore kell rendezkedni " << endl;
                    temp = m_data[parent];
                    m_data[parent] = *actualElement;
         *actualElement = temp;
                    index = parent;
                    actualElement = m_data + index;
                    parent = parent >> 1;
                }
                return;
            }
        }*/

        do {
            leftElement = m_data + (index << 1);
            rightElement = leftElement + 1;
            actualElement = m_data + index;
            maxElement = actualElement;

            //cout << *actualElement << " " << *leftElement << " " << *rightElement << endl;

            if (leftElement <= endElement && leftElement->m_key > actualElement->m_key) {
                maxElement = leftElement;
                //cout << "bal gyerek nagyobb" << endl;
            }

            if (rightElement <= endElement && rightElement->m_key > maxElement->m_key) {
                maxElement = rightElement;
                //cout << "jobb gyerek nagyobb" << endl;
            }

            if (maxElement != actualElement) {
                //cout << "rearranging" << endl;
                //MOVE::move(maxElement->m_data, index - 1);
                MOVE::move(maxElement->m_data, actualElement - m_data - 1, m_pointer);
                MOVE::move(actualElement->m_data, maxElement - m_data - 1, m_pointer);

                temp = *actualElement; //m_data[index];
                *actualElement = *maxElement;
                *maxElement = temp;
                index = maxElement - m_data;
                //MOVE::move(actualElement->m_data, index - 1);
            }
        } while (maxElement != actualElement);
//        if (check() == false) {
//            cout << "on ellenorzes hibat talalt a changeKey-nel " << endl;
//            cin.get();
//        }

    }

    bool check() const
    {
        return true;
        unsigned int index = 1;
        for (; index <= m_size; index++) {
            unsigned int left = index * 2;
            unsigned int right = left + 1;
            K key = m_data[index].m_key;
            if (left <= m_size && m_data[left].m_key > key) {
                cout << "left" << endl;
                cout << index << ", " << left << endl;
                cout << m_data[left].m_key << " > " << key << endl;
                return false;
            }
            if (right <= m_size && m_data[right].m_key > key) {
                cout << "right     actual" << endl;
                cout << right << ", " << index << endl;
                cout << m_data[right].m_key << " > " << key << endl;
                return false;
            }
        }
        return true;
    }

private:

    struct Element
    {
        K m_key;
        T m_data;

        friend ostream & operator<<(ostream & os, const Element & e)
        {
            os << "(" << e.m_key << "; " << e.m_data << ")";
            return os;
        }
    };

    unsigned int m_size;

    unsigned int m_capacity;

    Element * m_data;

    Element * m_actual;
    
    void * m_pointer;

    void copy(const Heap & orig)
    {
        m_size = orig.m_size;
        m_capacity = orig.m_capacity;
        m_data = new Element[ m_capacity ];
        memcpy(m_data + 1, orig.m_data + 1, sizeof (Element) * m_size);
        m_actual = m_data + (orig.m_actual - orig.m_data);
    }

    void clear()
    {
        delete [] m_data;
        m_data = 0;
        m_actual = 0;
        m_size = 0;
        m_capacity = 0;
    }

    void keyIncrement(unsigned int index, const K & key)
    {
        // kulcs novelese
        // a gyerekekkel nincs gond
        // para akkor van, ha nagyobb, mint a szulo
        Element temp;
        //cout << "\tkeyIncrement" << endl;
        unsigned int parent = index >> 1;
        Element * actualElement = m_data + index;
        Element * parentElement = m_data + parent;
        actualElement->m_key = key;
        while (parent > 0 && parentElement->m_key < actualElement->m_key) {
            //cout << "a szulore kell rendezkedni, mert ";
            //cout << parent << ": " << parentElement->m_key << " < ";
            //cout << index << ": " << actualElement->m_key << endl;
            MOVE::move(parentElement->m_data, index - 1, m_pointer);
            MOVE::move(actualElement->m_data, parent - 1, m_pointer);
            temp = *parentElement;
            *parentElement = *actualElement;
            *actualElement = temp;
            index = parent;
            actualElement = m_data + index;
            parent = parent >> 1;
            parentElement = m_data + parent;
        }
        return;

    }

};

#endif	/* HEAP_H */

