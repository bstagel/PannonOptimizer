#ifndef REFOBJECT_H
#define REFOBJECT_H

#include <utils/exceptions.h>

template <class T>
class RefObject {
    class AuxRefObject {
        mutable T * object;
        mutable int counter;

    public:
        AuxRefObject(T * ptr = 0) {
            object = ptr;
            if (ptr)
                counter = 1;
        }

        ~AuxRefObject(){
        }

        inline void dec() const {
            counter--;
            if (counter == 0) {
                delete object;
                object = 0;
            }

        }

        inline void inc() const {
            counter++;
        }

        inline void set(T * ptr, int c) {
            object = ptr;
            counter = c;
        }

        inline int getCounter() const {
            return counter;
        }

        inline T & getObject() const {
            return *object;
        }
    };

    mutable AuxRefObject * object;
public:

    RefObject(T * ptr = 0);

    RefObject(const void * & ptr);

    RefObject(const int ptr);

    RefObject(const RefObject& orig);

    template <class U> RefObject(const RefObject<U>& orig);

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
        object = 0;
}

template <class T>
RefObject<T>::RefObject(const int ptr) {
    void * _ptr = (void *)(ptr);
    if (_ptr)
        object = new AuxRefObject((T*)_ptr);
    else
        object = 0;
}

template <class T>
RefObject<T>::RefObject(const void * & ptr) {
    if (ptr)
        object = new AuxRefObject( (T*)ptr );
    else
        object = 0;
}

template <class T>
RefObject<T>::RefObject(const RefObject<T> & orig) {
    if (orig.object) {
        object = orig.object;
        object->inc();
    } else {
        object = 0;
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
        object = 0;
    }

}

template <class T>
template <class T2>
RefObject<T> & RefObject<T>::operator=(const RefObject<T2> & obj) {
    if (obj.object == 0) {
        dec();
        object = 0;
    } else {
        dec();
        obj.object->inc();
        object = obj.object;
    }
}

template <class T>
RefObject<T> & RefObject<T>::operator=(const RefObject<T> & obj) {
    if (obj.object == 0) {
        dec();
        object = 0;
    } else {
        dec();
        obj.object->inc();
        object = obj.object;
    }
    return *this;
}

template <class T>
RefObject<T> & RefObject<T>::operator=(void * obj) {
    if (obj == 0) {
        dec();
        object = 0;
    } else {
        dec();
        object = new AuxRefObject((T*)obj);
    }
    return *this;
}

template <class T>
template <class T2>
RefObject<T> & RefObject<T>::operator=(T2 * obj) {
    if (obj == 0) {
        dec();
        object = 0;
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
