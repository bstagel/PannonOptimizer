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

#ifndef ITERATOR_H
#define ITERATOR_H

#include <globals.h>

/**
 * A general purpose iterator template.
 * It iterates on the elements of a vector.
 */
template <class DATA_TYPE>
class NormalIterator {
public:

    /**
     * Constructor of the class Iterator.
     *
     * @param ptr Starting address.
     */
    ALWAYS_INLINE NormalIterator(DATA_TYPE * data = nullptr): m_data(data) {

    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE DATA_TYPE operator*() {
        return *m_data;
    }

    ALWAYS_INLINE const DATA_TYPE * operator->() const { return m_data; }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        m_data++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void operator--() {
        m_data--;
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
        return m_data == iter.m_data;
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
        return m_data != iter.m_data;
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
        return m_data < iter.m_data;
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
        return m_data > iter.m_data;
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
        return m_data <= iter.m_data;
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
        return m_data >= iter.m_data;
    }

private:

    /**
     * This pointer points to a vector element.
     */
    const DATA_TYPE * m_data;
};

/**
 * A general purpose indexed iterator template.
 */
template<class DATA_TYPE, class INDEX_TYPE>
class IndexedIterator {
public:

    /**
     * Constructor of the class IndexedIterator.
     */
    ALWAYS_INLINE IndexedIterator():
        m_index(nullptr),
        m_data(nullptr) {

    }

    /**
     * Constructor of the class IndexedIterator.
     *
     * @param ptr Starting index address.
     * @param data Dense array address.
     */
    ALWAYS_INLINE IndexedIterator(INDEX_TYPE * ptr,
                                  const DATA_TYPE * data):
        m_index(ptr),
        m_data(data) {

    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE const DATA_TYPE & operator*() {
        return m_data[ *m_index ];
    }

    ALWAYS_INLINE DATA_TYPE * operator->() { return m_data; }

    /**
     * Returns with the current index.
     *
     * @return The current index.
     */
    ALWAYS_INLINE INDEX_TYPE getIndex() const {
        return *m_index;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        m_index++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void operator--() {
        m_index--;
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
        return m_index == iter.m_index;
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
        return m_index != iter.m_index;
    }

private:

    /**
     * This pointer points to an index element.
     */
    const INDEX_TYPE * m_index;

    /**
     * This pointer points to the dense array.
     */
    const DATA_TYPE * m_data;

};

/**
 * A general purpose iterator template, which skips some elements.
 */
template<class DATA_TYPE, class SKIP>
class SkipIterator {
public:

    /**
     * Constructor of the class IndexedIterator.
     *
     */
    ALWAYS_INLINE SkipIterator():
        m_dataStart(nullptr),
        m_data(nullptr),
        m_dataEnd(nullptr) {

    }

    /**
     * Constructor of the class IndexedIterator.
     *
     * @param ptr Starting address.
     * @param end Address of the end of the vector.
     */
    ALWAYS_INLINE SkipIterator(const DATA_TYPE * start, const DATA_TYPE * ptr, const DATA_TYPE * end):
        m_dataStart(start),
        m_data(ptr),
        m_dataEnd(end) {
        SKIP::init(m_data, m_dataEnd);
    }

    ALWAYS_INLINE unsigned getIndex() const {
        return m_data - m_dataStart;
    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE const DATA_TYPE & operator*() {
        return *m_data;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        SKIP::next(m_data, m_dataEnd);
    }

    /**
     * Equality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to the
     * same element.
     */
    ALWAYS_INLINE bool operator==(const SkipIterator & iter) const {
        return m_data == iter.m_data;
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
        return m_data != iter.m_data;
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
        return m_data < iter.m_data;
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
        return m_data > iter.m_data;
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
        return m_data <= iter.m_data;
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
        return m_data >= iter.m_data;
    }

private:

    const DATA_TYPE * m_dataStart;

    /**
     * This pointer points to the dense array.
     */
    const DATA_TYPE * m_data;

    /**
     * This pointer points to the end of the dense array.
     */
    const DATA_TYPE * m_dataEnd;

};


/**
 * A general purpose indexed iterator template.
 */
template<class DATA_TYPE, class INDEX_TYPE>
class ValueIndexPairIterator {
public:

    /**
     * Constructor of the class ValueIndexPairIterator.
     */
    ALWAYS_INLINE ValueIndexPairIterator():
        m_data(nullptr),
        m_index(nullptr){

    }

    /**
     * Constructor of the class ValueIndexPairIterator.
     *
     * @param ptr Starting index address.
     * @param data Dense array address.
     */
    ALWAYS_INLINE ValueIndexPairIterator(const DATA_TYPE * data,
                                         INDEX_TYPE * ptr):
        m_data(data),
        m_index(ptr)
    {

    }

    /**
     * Returns with the referenced number.
     *
     * @return The current number.
     */
    ALWAYS_INLINE const DATA_TYPE & operator*() {
        return *m_data;
    }

    /**
     * Returns with the current index.
     *
     * @return The current index.
     */
    ALWAYS_INLINE INDEX_TYPE getIndex() const {
        return *m_index;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void next() {
        m_data++;
        m_index++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void previous() {
        m_data--;
        m_index--;
    }

    /**
     * Steps the iterator to the following value.
     */
    ALWAYS_INLINE void operator++() {
        m_data++;
        m_index++;
    }

    /**
     * Steps the iterator to the previous value.
     */
    ALWAYS_INLINE void operator--() {
        m_data--;
        m_index--;
    }

    /**
     * Equality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to the
     * same element.
     */
    ALWAYS_INLINE bool operator==(const ValueIndexPairIterator & iter) const {
        return m_index == iter.m_index;
    }

    /**
     * Inequality operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the iterators points to different
     * elements.
     */
    ALWAYS_INLINE bool operator!=(const ValueIndexPairIterator & iter) const {
        return m_index != iter.m_index;
    }

    /**
     * Less operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less then the address of the second element.
     */
    ALWAYS_INLINE bool operator<(const ValueIndexPairIterator & iter) const {
        return m_index < iter.m_index;
    }

    /**
     * Greater operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater then the address of the second element.
     */
    ALWAYS_INLINE bool operator>(const ValueIndexPairIterator & iter) const {
        return m_index > iter.m_index;
    }

    /**
     * Less or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is less or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator<=(const ValueIndexPairIterator & iter) const {
        return m_index <= iter.m_index;
    }

    /**
     * Greater or equal operator.
     *
     * @param iter The operator compares the current iterator with this
     * iterator.
     * @return The result is true, if the address of the first element
     * is greater or equal then the address of the second element.
     */
    ALWAYS_INLINE bool operator>=(const ValueIndexPairIterator & iter) const {
        return m_index >= iter.m_index;
    }

private:
    /**
     * This pointer points to the data array.
     */
    const DATA_TYPE * m_data;

    /**
     * This pointer points to an index element.
     */
    const INDEX_TYPE * m_index;

};


#endif // ITERATOR_H
