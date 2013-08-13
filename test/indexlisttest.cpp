/* 
 * File:   indexlisttest.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 12., 19:57
 */

#include "indexlisttest.h"

using namespace std;

IndexListTestSuite::IndexListTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(IndexListTestSuite::init);
}

void IndexListTestSuite::init() {
    IndexList list(100, 4);
    
    list.insert(0, 0);
    list.insert(0, 1);
    list.insert(0, 2);
    list.insert(3, 3);
    list.insert(3, 4);
    list.insert(2, 5);
    list.insert(2, 6);
    
    //IndexList::Iterator iter = list.begin(0);
    //IndexList::Iterator iterEnd = list.end(0);
    IndexList::Iterator iter, iterEnd;
    list.getIterators(&iter, &iterEnd, 0, 2);
    int counter = 0;
    for (; iter != iterEnd && counter < 10; iter++, counter++) {
        cout << iter.getData() << endl;
    }
}