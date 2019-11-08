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
 * @file refobject.h This file contains the API of the RefObject class.
 * @author Jozsef Smidla
 */

#ifndef REFOBJECT_H
#define REFOBJECT_H

#include <utils/exceptions.h>

/**
 * This class implements counted pointer references to objects.
 * When the counter drops to zero, the dynamically allocated object can be released.
 *
 * @class RefObject
 */
template <class T>
class RefObject {
    /**
     * This class implements the processing of objects in the RefObject class.
     *
     * @class AuxRefObject
     */
    class AuxRefObject {

        /**
         * Pointer to the represented object.
         */
        mutable T * object;

        /**
         * The count of references to the object.
         */
        mutable int counter;

    public:

        /**
         * Constructor of the AuxRefObject class.
         * The pointed object can be specified.
         *
         * @constructor
         * @param ptr The object to be represented.
         */
        AuxRefObject(T * ptr = nullptr) {
            object = ptr;
            if (ptr)
                counter = 1;
        }

        /**
         * Destructor of the AuxRefObject class.
         *
         * @destructor
         */
        ~AuxRefObject(){
        }

        /**
         * Decreases the counter of the references to the object.
         */
        inline void dec() const {
            counter--;
            if (counter == 0) {
                delete object;
                object = nullptr;
            }
        }

        /**
         * Increases the counter of the references to the object.
         */
        inline void inc() const {
            counter++;
        }

        /**
         * Sets the object and counter to the given values.
         *
         * @param ptr The new pointer to the represented object.
         * @param c The new counter of references to the object.
         */
        inline void set(T * ptr, int c) {
            object = ptr;
            counter = c;
        }

        /**
         * Returns the counter of references to the object.
         *
         * @return The counter of references to the object.
         */
        inline int getCounter() const {
            return counter;
        }

        /**
         * Returns a reference to the represented object.
         *
         * @return Reference to the represented object.
         */
        inline T & getObject() const {
            return *object;
        }
    };

    /**
     * Pointer to the AuxRefObject representing the object.
     */
    mutable AuxRefObject * object;
public:

    /**
     * Constructor of the RefObject class.
     * The pointed object can be specified.
     *
     * @constructor
     * @param ptr Pointer to the represented object.
     */
    RefObject(T * ptr = nullptr);

    /**
     * Constructor of the RefObject class.
     * The pointed object can be specified.
     *
     * @constructor
     * @param ptr Pointer to the represented object.
     */
    RefObject(const void * & ptr);

    /**
     * Constructor of the RefObject class.
     * The pointed object can be specified.
     *
     * @constructor
     * @param ptr Pointer to the represented object.
     */
    RefObject(const int ptr);

    /**
     * Copy constructor of the RefObject class.
     *
     * @constructor
     * @param orig The original RefObject to be copied.
     */
    RefObject(const RefObject& orig);

    /**
     * Cross-template copy constructor of the RefObject class.
     *
     * @constructor
     * @param orig The original RefObject to be copied.
     */
    template <class U> RefObject(const RefObject<U>& orig);

    /**
     * Destructor of the RefObject class.
     *
     * @destructor
     */
    ~RefObject();

    void dec() const;

    RefObject & operator=(const RefObject & obj);

    template <class T2>
    RefObject<T> & operator=(const RefObject<T2> & obj);


    RefObject & operator=(void * obj);

    template <class T2>
    RefObject & operator=(T2 * obj);

    inline T & operator*();

    inline T * operator->();

    inline const T & operator*() const;

    inline const T * operator->() const;

    operator RefObject() { return *this; }

    int getInstanceCount() const {
        if (object)
            return object->getCounter();
        return 0;
    }

    void setObj(void * ptr) {
        object = (AuxRefObject *)ptr;
    }

    template <class U> operator RefObject<U>();

    template <class U> friend bool operator!=(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator==(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator>(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator<(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator>=(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator<=(const RefObject<U> & obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator!=(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator==(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator>(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator<(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator>=(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator<=(const void * obj1, const RefObject<U> & obj2);

    template <class U> friend bool operator!=(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend bool operator==(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend bool operator>(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend bool operator<(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend bool operator>=(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend bool operator<=(const RefObject<U> & obj1, const void * obj2);

    template <class U> friend std::ostream & operator << (std::ostream & os, const RefObject<U> & obj);

};

template <class T>
RefObject<T>::RefObject(T * ptr) {
    if (ptr)
        object = new AuxRefObject(ptr);
    else
        object = nullptr;
}

template <class T>
RefObject<T>::RefObject(const int ptr) {
    void * _ptr = (void *)(ptr);
    if (_ptr)
        object = new AuxRefObject((T*)_ptr);
    else
        object = nullptr;
}

template <class T>
RefObject<T>::RefObject(const void * & ptr) {
    if (ptr)
        object = new AuxRefObject( (T*)ptr );
    else
        object = nullptr;
}

template <class T>
RefObject<T>::RefObject(const RefObject<T> & orig) {
    if (orig.object) {
        object = orig.object;
        object->inc();
    } else {
        object = nullptr;
    }
}

template<class T> RefObject<T>::~RefObject() {
    dec();
}

template<class T> void RefObject<T>::dec() const {
    if (!object)
        return;
    object->dec();
    if (object->getCounter() == 0) {
        delete object;
        object = nullptr;
    }

}

template <class T>
template <class T2>
RefObject<T> & RefObject<T>::operator=(const RefObject<T2> & obj) {
    if (obj.object == nullptr) {
        dec();
        object = nullptr;
    } else {
        dec();
        obj.object->inc();
        object = obj.object;
    }
}

template <class T>
RefObject<T> & RefObject<T>::operator=(const RefObject<T> & obj) {
    if (obj.object == nullptr) {
        dec();
        object = nullptr;
    } else {
        dec();
        obj.object->inc();
        object = obj.object;
    }
    return *this;
}

template <class T>
RefObject<T> & RefObject<T>::operator=(void * obj) {
    if (obj == nullptr) {
        dec();
        object = nullptr;
    } else {
        dec();
        object = new AuxRefObject((T*)obj);
    }
    return *this;
}

template <class T>
template <class T2>
RefObject<T> & RefObject<T>::operator=(T2 * obj) {
    if (obj == nullptr) {
        dec();
        object = nullptr;
    } else {
        dec();
        object = new AuxRefObject(obj);
    }
    return *this;
}

template <class T>
template <class U>
RefObject<T>::operator RefObject<U>() {
    RefObject<U> ref;
    ref.setObj(object);
    return ref;
}

template <class T>
T & RefObject<T>::operator*() {
    if (object)
        return object->getObject();
    else
        throw NullPointerException("Null void * exception az operator* in RefObject");
}

template <class T>
T * RefObject<T>::operator->() {
    if (object)
        return &object->getObject();
    else
        throw NullPointerException("Null void * exception az operator* in RefObject");
};

template <class T>
const T & RefObject<T>::operator*() const {
    if (object)
        return object->getObject();
    else
        throw NullPointerException("Null void * exception az operator* in RefObject");
}

template <class T>
const T * RefObject<T>::operator->() const {
    if (object)
        return &object->getObject();
    else
        throw NullPointerException("Null void * exception az operator* in RefObject");
};

template <class U>
bool operator!=(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object != obj2.object;
}

template <class U>
bool operator==(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object == obj2.object;
}

template <class U>
bool operator>(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object > obj2.object;
}

template <class U>
bool operator<(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object < obj2.object;
}

template <class U>
bool operator>=(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object >= obj2.object;
}

template <class U>
bool operator<=(const RefObject<U> & obj1, const RefObject<U> & obj2) {
    return obj1.object <= obj2.object;
}

template <class U>
bool operator!=(const void * obj1, const RefObject<U> & obj2) {
    return obj1 != (void *)obj2.object;
}

template <class U>
bool operator==(const void * obj1, RefObject<U> const & obj2) {
    return obj1 == (void *)obj2.object;
}

template <class U>
bool operator>(const void * obj1, const RefObject<U> & obj2) {
    return obj1 > (void *)obj2.object;
}

template <class U>
bool operator<(const void * obj1, const RefObject<U> & obj2) {
    return obj1 < (void *)obj2.object;
}

template <class U>
bool operator>=(const void * obj1, const RefObject<U> & obj2) {
    return obj1 >= (void *)obj2.object;
}

template <class U>
bool operator<=(const void * obj1, const RefObject<U> & obj2) {
    return obj1 <= (void *)obj2.object;
}

template <class U>
bool operator!=(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object != obj2;
}

template <class U>
bool operator==(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object == obj2;
}

template <class U>
bool operator>(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object > obj2;
}

template <class U>
bool operator<(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object < obj2;
}

template <class U>
bool operator>=(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object >= obj2;
}

template <class U>
bool operator<=(const RefObject<U> & obj1, const void * obj2) {
    return (void *)obj1.object <= obj2;
}

template <class U>
std::ostream & operator << (std::ostream & os, const RefObject<U> & obj) {
    os << obj.object;
    return os;
}

#endif // REFOBJECT_H
