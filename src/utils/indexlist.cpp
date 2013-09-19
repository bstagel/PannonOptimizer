/**
 * @file indexlist.cpp
 */

#include <utils/indexlist.h>

IndexList::IndexList(unsigned int count, unsigned int partitions)
{
    m_partitionCount = 0;
    m_heads = 0;
    m_count = 0;
    m_dataArray = 0;
    if (partitions != 0 && count != 0) {
        init(count, partitions);
    }
}

IndexList::IndexList(const IndexList & list)
{
    copy(list);
}

IndexList & IndexList::operator=(const IndexList & list)
{
    clear();
    copy(list);
    return *this;
}

IndexList::~IndexList()
{
    clear();
}

void IndexList::init(unsigned int count, unsigned int partitions)
{
    clear();
    Element * pointerIterator;
    Element * pointerIteratorEnd;
    m_partitionCount = partitions;
    m_heads = new Element[partitions];
    pointerIterator = m_heads;
    pointerIteratorEnd = m_heads + partitions;
    unsigned int index = 0;
    for (; pointerIterator < pointerIteratorEnd; pointerIterator++, index++) {
        pointerIterator->m_data = index;
        pointerIterator->m_next = pointerIterator;
        pointerIterator->m_previous = pointerIterator;
        pointerIterator->m_isHeader = true;
        pointerIterator->m_partitionIndex = index;
        pointerIterator->m_pointer = 0;
    }

    m_count = count;
    m_dataArray = new Element[count];
    pointerIterator = m_dataArray;
    pointerIteratorEnd = m_dataArray + count;
    index = 0;
    for (; pointerIterator < pointerIteratorEnd; pointerIterator++, index++) {
        pointerIterator->m_data = index;
        pointerIterator->m_next = 0;
        pointerIterator->m_previous = 0;
        pointerIterator->m_isHeader = false;
        pointerIterator->m_partitionIndex = partitions;
        pointerIterator->m_pointer = 0;
    }
}

void IndexList::clear()
{
    delete [] m_heads;
    delete [] m_dataArray;
    m_partitionCount = 0;
    m_count = 0;
    m_heads = 0;
    m_dataArray = 0;
}

void IndexList::copy(const IndexList & list)
{
    if (list.m_count == 0) {
        m_partitionCount = 0;
        m_heads = 0;
        m_count = 0;
        m_dataArray = 0;
        return;
    }

    Element * myPointerIterator;
    Element * myPointerIteratorEnd;
    Element * hisPointerIterator;
    m_partitionCount = list.m_partitionCount;
    m_heads = new Element[m_partitionCount];
    m_count = list.m_count;
    m_dataArray = new Element[m_count];

    myPointerIterator = m_heads;
    myPointerIteratorEnd = m_heads + m_partitionCount;
    hisPointerIterator = list.m_heads;
    unsigned int index = 0;
    for (; myPointerIterator < myPointerIteratorEnd; myPointerIterator++,
            hisPointerIterator++, index++) {
        myPointerIterator->m_data = index;
        if (hisPointerIterator->m_next != hisPointerIterator) {
            myPointerIterator->m_next = m_dataArray + (hisPointerIterator->m_next - list.m_dataArray);
            myPointerIterator->m_previous = m_dataArray + (hisPointerIterator->m_previous - list.m_dataArray);
        } else {
            myPointerIterator->m_next = myPointerIterator;
            myPointerIterator->m_previous = myPointerIterator;
        }

    }

    myPointerIterator = m_dataArray;
    myPointerIteratorEnd = m_dataArray + m_count;
    hisPointerIterator = list.m_dataArray;
    index = 0;
    for (; myPointerIterator < myPointerIteratorEnd; myPointerIterator++,
            hisPointerIterator++, index++) {
        myPointerIterator->m_data = index;
        if (hisPointerIterator->m_next == 0) {
            myPointerIterator->m_next = 0;
            myPointerIterator->m_previous = 0;
        } else {
            if (hisPointerIterator->m_next >= list.m_dataArray &&
                    hisPointerIterator->m_next < list.m_dataArray + m_count) {
                myPointerIterator->m_next = m_dataArray + (hisPointerIterator->m_next - list.m_dataArray);
            } else {
                myPointerIterator->m_next = m_heads + (hisPointerIterator->m_next - list.m_heads);
            }

            if (hisPointerIterator->m_previous >= list.m_dataArray &&
                    hisPointerIterator->m_previous < list.m_dataArray + m_count) {
                myPointerIterator->m_previous = m_dataArray + (hisPointerIterator->m_previous - list.m_dataArray);
            } else {
                myPointerIterator->m_previous = m_heads + (hisPointerIterator->m_previous - list.m_heads);
            }

        }
    }
}

std::ostream & operator << (std::ostream & os, const IndexList & list) {
    os << "Partitions: " << list.getPartitionCount() << std::endl;
    os << "Indices: " << list.getIndexCount() << std::endl;

    unsigned int index;
    for(index = 0; index < list.getPartitionCount(); index++){
        os << "List " << index << ": " << std::endl;
        IndexList::Iterator iter, iterEnd;
        list.getIterators(&iter, &iterEnd, index);
        for (; iter != iterEnd; iter++) {
            os << "     " << iter.getData() << std::endl;
        }
    }
    return os;
}
