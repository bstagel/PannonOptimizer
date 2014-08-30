#ifndef ITERATOR_H
#define ITERATOR_H

#include <globals.h>

/**
 * A general purpose iterator template.
 * It iterates on the elements of a vector.
 */
template <class T>
class NormalIterator {
public:

    /**
     * Constructor of the class Iterator.
     *
     * @param ptr Starting address.
     */
    ALWAYS_INLINE NormalIterator(T * ptr = nullptr): m_ptr(ptr) {

    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE T operator*() {
        return *m_ptr;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void next() {
        m_ptr++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void previous() {
        m_ptr--;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        m_ptr++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void operator--() {
        m_ptr--;
    }

    /**
     * Equality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to the
     * same element.
     */
    ALWAYS_INLINE bool operator==(const NormalIterator & iter) const {
        return m_ptr == iter.m_ptr;
    }

    /**
     * Inequality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to different
     * elements.
     */
    ALWAYS_INLINE bool operator!=(const NormalIterator & iter) const {
        return m_ptr != iter.m_ptr;
    }

    /**
     * Less operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less then the address of the second element.
     */
    ALWAYS_INLINE bool operator<(const NormalIterator & iter) const {
        return m_ptr < iter.m_ptr;
    }

    /**
     * Greater operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater then the address of the second element.
     */
    ALWAYS_INLINE bool operator>(const NormalIterator & iter) const {
        return m_ptr > iter.m_ptr;
    }

    /**
     * Less or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator<=(const NormalIterator & iter) const {
        return m_ptr <= iter.m_ptr;
    }

    /**
     * Greater or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator>=(const NormalIterator & iter) const {
        return m_ptr >= iter.m_ptr;
    }

private:

    /**
     * This pointer points to a vector element.
     */
    const T * m_ptr;
};

/**
 * A general purpose indexed iterator template.
 */
template<class T, class INDEX_TYPE>
class IndexedIterator {
public:

    /**
     * Constructor of the class IndexedIterator.
     */
    ALWAYS_INLINE IndexedIterator():
        m_ptr(nullptr),
        m_data(nullptr) {

    }

    /**
     * Constructor of the class IndexedIterator.
     *
     * @param ptr Starting index address.
     * @param data Dense array address.
     */
    ALWAYS_INLINE IndexedIterator(INDEX_TYPE * ptr,
                                  const T * data):
        m_ptr(ptr),
        m_data(data) {

    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE const T & operator*() {
        return m_data[ *m_ptr ];
    }

    /**
     * Returns with the current index.
     *
     * @return The current index.
     */
    ALWAYS_INLINE INDEX_TYPE getIndex() const {
        return *m_ptr;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void next() {
        m_ptr++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void previous() {
        m_ptr--;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        m_ptr++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void operator--() {
        m_ptr--;
    }

    /**
     * Equality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to the
     * same element.
     */
    ALWAYS_INLINE bool operator==(const IndexedIterator & iter) const {
        return m_ptr == iter.m_ptr;
    }

    /**
     * Inequality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to different
     * elements.
     */
    ALWAYS_INLINE bool operator!=(const IndexedIterator & iter) const {
        return m_ptr != iter.m_ptr;
    }

    /**
     * Less operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less then the address of the second element.
     */
    ALWAYS_INLINE bool operator<(const IndexedIterator & iter) const {
        return m_ptr < iter.m_ptr;
    }

    /**
     * Greater operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater then the address of the second element.
     */
    ALWAYS_INLINE bool operator>(const IndexedIterator & iter) const {
        return m_ptr > iter.m_ptr;
    }

    /**
     * Less or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator<=(const IndexedIterator & iter) const {
        return m_ptr <= iter.m_ptr;
    }

    /**
     * Greater or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator>=(const IndexedIterator & iter) const {
        return m_ptr >= iter.m_ptr;
    }

private:

    /**
     * This pointer points to an index element.
     */
    const INDEX_TYPE * m_ptr;

    /**
     * This pointer points to the dense array.
     */
    const T * m_data;

};

/**
 * A general purpose iterator template, which skips some elements.
 */
template<class T, class SKIP>
class SkipIterator {
public:

    /**
     * Constructor of the class IndexedIterator.
     *
     */
    ALWAYS_INLINE SkipIterator():
        m_ptr(nullptr),
        m_endPtr(nullptr) {

    }

    /**
     * Constructor of the class IndexedIterator.
     *
     * @param ptr Starting address.
     * @param end Address of the end of the vector.
     */
    ALWAYS_INLINE SkipIterator(const T * ptr, const T * end):
        m_ptr(ptr),
        m_endPtr(end) {
        SKIP::init(m_ptr, m_endPtr);
    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE const T & operator*() {
        return *m_ptr;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void next() {
        SKIP::next(m_ptr, m_endPtr);
    }

    ///**
    // * Steps the iterator to the previous value.
    // */
    /*ALWAYS_INLINE void previous() {
        SKIP::previous(m_ptr, m_endPtr);
    }*/

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        SKIP::next(m_ptr, m_endPtr);
    }

    ///**
    // * Steps the iterator to the previous value.
    // */
    /*ALWAYS_INLINE void operator--() {
        SKIP::previous(m_ptr, m_endPtr);
    }*/

    /**
     * Equality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to the
     * same element.
     */
    ALWAYS_INLINE bool operator==(const SkipIterator & iter) const {
        return m_ptr == iter.m_ptr;
    }

    /**
     * Inequality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to different
     * elements.
     */
    ALWAYS_INLINE bool operator!=(const SkipIterator & iter) const {
        return m_ptr != iter.m_ptr;
    }

    /**
     * Less operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less then the address of the second element.
     */
    ALWAYS_INLINE bool operator<(const SkipIterator & iter) const {
        return m_ptr < iter.m_ptr;
    }

    /**
     * Greater operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater then the address of the second element.
     */
    ALWAYS_INLINE bool operator>(const SkipIterator & iter) const {
        return m_ptr > iter.m_ptr;
    }

    /**
     * Less or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator<=(const SkipIterator & iter) const {
        return m_ptr <= iter.m_ptr;
    }

    /**
     * Greater or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator>=(const SkipIterator & iter) const {
        return m_ptr >= iter.m_ptr;
    }

private:

    /**
     * This pointer points to the dense array.
     */
    const T * m_ptr;

    /**
     * This pointer points to the end of the dense array.
     */
    const T * m_endPtr;

};


#endif // ITERATOR_H
