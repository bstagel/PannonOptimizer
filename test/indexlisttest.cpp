/* 
 * File:   indexlisttest.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 12., 19:57
 */

#include <indexlisttest.h>

using namespace std;

IndexListTestSuite::IndexListTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(IndexListTestSuite::init);
    ADD_TEST(IndexListTestSuite::getPartitionCount);
    ADD_TEST(IndexListTestSuite::getIndexCount);
    ADD_TEST(IndexListTestSuite::insert);
    ADD_TEST(IndexListTestSuite::where);
    ADD_TEST(IndexListTestSuite::attachedData);
    ADD_TEST(IndexListTestSuite::remove);
    ADD_TEST(IndexListTestSuite::move);
    ADD_TEST(IndexListTestSuite::firstElement);
}

void IndexListTestSuite::init() {

    unsigned int testCounts[] = {0, 5, 10, 100, 1000};
    unsigned int testPartitions[] = {0, 1, 2, 5, 10};
    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);
    unsigned int testCaseIndex, curCount, curPartitions;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curCount = testCounts[testCaseIndex];
        curPartitions = testPartitions[testCaseIndex];

        IndexList<int> list(curCount, curPartitions);

        if(curCount == 0 || curPartitions == 0)
        {
            TEST_ASSERT(list.m_partitionCount == 0);
            TEST_ASSERT(list.m_heads == 0);
            TEST_ASSERT(list.m_count == 0);
            TEST_ASSERT(list.m_dataArray == 0);
        } else {
            TEST_ASSERT(list.m_partitionCount == curPartitions);
            TEST_ASSERT(list.m_count == curCount);

            IndexList<int>::Element<int> * curElement;
            unsigned int index = 0;
            for(curElement = list.m_heads; index < curPartitions; index++, curElement++)
            {
                TEST_ASSERT(curElement->m_isHeader == true);
                TEST_ASSERT(curElement->m_next == curElement);
                TEST_ASSERT(curElement->m_previous == curElement);
                TEST_ASSERT(curElement->m_data == index);
                TEST_ASSERT(curElement->m_partitionIndex == index);
            }
            for(index = 0, curElement = list.m_dataArray; index < curPartitions; index++, curElement++)
            {
                TEST_ASSERT(curElement->m_isHeader == false);
                TEST_ASSERT(curElement->m_next == 0);
                TEST_ASSERT(curElement->m_previous == 0);
                TEST_ASSERT(curElement->m_data == index);
                TEST_ASSERT(curElement->m_partitionIndex == curPartitions);
            }
        }

        IndexList<int> list2(list);

        if(curCount == 0 || curPartitions == 0)
        {
            TEST_ASSERT(list2.m_partitionCount == 0);
            TEST_ASSERT(list2.m_heads == 0);
            TEST_ASSERT(list2.m_count == 0);
            TEST_ASSERT(list2.m_dataArray == 0);
        } else {
            TEST_ASSERT(list2.m_partitionCount == curPartitions);
            TEST_ASSERT(list2.m_count == curCount);

            IndexList<int>::Element<int> * curElement;
            unsigned int index = 0;
            for(curElement = list2.m_heads; index < curPartitions; index++, curElement++)
            {
                TEST_ASSERT(curElement->m_isHeader == true);
                TEST_ASSERT(curElement->m_next == curElement);
                TEST_ASSERT(curElement->m_previous == curElement);
                TEST_ASSERT(curElement->m_data == index);
                TEST_ASSERT(curElement->m_partitionIndex == index);
            }
            for(index = 0, curElement = list2.m_dataArray; index < curPartitions; index++, curElement++)
            {
                TEST_ASSERT(curElement->m_isHeader == false);
                TEST_ASSERT(curElement->m_next == 0);
                TEST_ASSERT(curElement->m_previous == 0);
                TEST_ASSERT(curElement->m_data == index);
                TEST_ASSERT(curElement->m_partitionIndex == curPartitions);
            }
        }
    }
}

void IndexListTestSuite::getPartitionCount() {
    unsigned int testCounts[] = {0, 5, 10, 100, 1000};
    unsigned int testPartitions[] = {0, 1, 2, 5, 10};
    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);
    unsigned int testCaseIndex, curCount, curPartitions;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testPartitions[testCaseIndex];

        IndexList<int> list(curCount, curPartitions);

        TEST_ASSERT(list.m_partitionCount == curPartitions);
    }
}

void IndexListTestSuite::getIndexCount() {
    unsigned int testCounts[] = {0, 5, 10, 100, 1000};
    unsigned int testPartitions[] = {0, 1, 2, 5, 10};
    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);
    unsigned int testCaseIndex, curCount, curPartitions;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testPartitions[testCaseIndex];

        IndexList<int> list(curCount, curPartitions);

        TEST_ASSERT(list.m_partitionCount == curCount);
    }
}

void IndexListTestSuite::insert() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0, index2 = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        //Test

        IndexList<int>::Element<int> * curElement;

        for(index = 0; index < curPartitions; index++)
        {
            //set pointer to current partition header's previous
            curElement = list.m_heads;
            curElement += index;
            curElement = curElement->m_previous;

            //current partition Indices
            unsigned int curPartIndices [curCount];
            unsigned int curPartCount = 0;
            for(index2 = 0; index2 < curInsertCount; index2++)
            {
                if(insertPartitions[index2] == index)
                {
                    curPartIndices[curPartCount] = insertIndices[index2];
                    curPartCount++;
                }
            }

            //test partition
            for(index2 = 0; index2 < curPartCount; index2++)
            {
                TEST_ASSERT(curElement->m_data == curPartIndices[index2]);
                TEST_ASSERT(curElement->m_partitionIndex == index);
                TEST_ASSERT(curElement->m_isHeader == false);

                curElement = curElement->m_previous;

                //test if the last element points to his partition's header
                if(index2 == curPartCount - 1)
                {
                    TEST_ASSERT(curElement->m_data == index);
                    TEST_ASSERT(curElement->m_partitionIndex == index);
                    TEST_ASSERT(curElement->m_isHeader == true);
                }
            }
        }
    }
}

void IndexListTestSuite::where() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        //Test

        for(index = 0; index < curInsertCount; index++)
        {
            TEST_ASSERT(list.where(insertIndices[index]) == insertPartitions[index]);
        }
    }
}

void IndexListTestSuite::attachedData() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};
    int attachedData[] = {6, 7, 12, 67, 100, 9, 65, -6, -111, 4, 0, 7, -1000, 1000, 77, 22, 88, 112, -322, 1};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);
    const unsigned int attachedCount = sizeof (attachedData) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        //Set attached data

        unsigned int curAttachedCount = attachedCount < curInsertCount ? attachedCount : curInsertCount;
        for(index = 0; index < curAttachedCount; index++)
        {
            list.setAttachedData(insertIndices[index], attachedData[index]);
        }

        //Test

        IndexList<int>::Element<int> * curElement;
        for(index = 0; index < curInsertCount; index++)
        {
            curElement = list.m_dataArray;
            curElement += insertIndices[index];

            TEST_ASSERT(curElement->m_attached == attachedData[index]);
            TEST_ASSERT(list.getAttachedData(insertIndices[index]) == attachedData[index]);
        }
    }
}

void IndexListTestSuite::remove() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};
    unsigned int removeCount[] = {1, 2, 6, 7, 18};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount, curRemoveCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;
        curRemoveCount = removeCount[testCaseIndex] < curInsertCount ? removeCount[testCaseIndex] : curInsertCount;

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0, index2 = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        for(index = 0; index < curRemoveCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.remove(insertIndices[index]);
            }
        }

        //Test

        IndexList<int>::Element<int> * curElement;

        for(index = 0; index < curPartitions; index++)
        {
            //set pointer to current partition header's previous
            curElement = list.m_heads;
            curElement += index;
            curElement = curElement->m_previous;

            //current partition Indices
            unsigned int curPartIndices [curCount];
            unsigned int curPartCount = 0;
            for(index2 = 0; index2 < curInsertCount; index2++)
            {
                if(insertPartitions[index2] == index && index2 >= curRemoveCount)
                {
                    curPartIndices[curPartCount] = insertIndices[index2];
                    curPartCount++;
                }
            }

            //test partition
            for(index2 = 0; index2 < curPartCount; index2++)
            {
                TEST_ASSERT(curElement->m_data == curPartIndices[index2]);
                TEST_ASSERT(curElement->m_partitionIndex == index);
                TEST_ASSERT(curElement->m_isHeader == false);

                curElement = curElement->m_previous;

                //test if the last element points to his partition's header
                if(index2 == curPartCount - 1)
                {
                    TEST_ASSERT(curElement->m_data == index);
                    TEST_ASSERT(curElement->m_partitionIndex == index);
                    TEST_ASSERT(curElement->m_isHeader == true);
                }
            }
        }
    }
}

void IndexListTestSuite::move() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};
    unsigned int moveCount[] = {0, 4, 6, 10, 20};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int movePartitions[] = {1, 1, 0, 0, 4, 3, 6, 6, 5, 5, 1, 0, 8, 5, 5, 7, 6, 6, 9, 5};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount, curMoveCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;
        curMoveCount = moveCount[testCaseIndex];

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0, index2 = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        //Move the indices

        for(index = 0; index < curMoveCount; index++)
        {
            list.move(insertIndices[index], movePartitions[index]);
        }

        //Test

        IndexList<int>::Element<int> * curElement;

        for(index = 0; index < curPartitions; index++)
        {
            //set pointer to current partition header's previous
            curElement = list.m_heads;
            curElement += index;
            curElement = curElement->m_previous;

            //current partition Indices
            unsigned int curPartIndices [curCount];
            unsigned int curPartCount = 0;
            for(index2 = 0; index2 < curInsertCount; index2++)
            {
                if(index2 >= curMoveCount) {
                    if(insertPartitions[index2] == index)
                    {
                        curPartIndices[curPartCount] = insertIndices[index2];
                        curPartCount++;
                    }
                } else {
                    if(movePartitions[index2] == index)
                    {
                        curPartIndices[curPartCount] = insertIndices[index2];
                        curPartCount++;
                    }
                }
            }

            //test partition
            for(index2 = 0; index2 < curPartCount; index2++)
            {
                TEST_ASSERT(curElement->m_data == curPartIndices[index2]);
                TEST_ASSERT(curElement->m_partitionIndex == index);
                TEST_ASSERT(curElement->m_isHeader == false);

                curElement = curElement->m_previous;

                //test if the last element points to his partition's header
                if(index2 == curPartCount - 1)
                {
                    TEST_ASSERT(curElement->m_data == index);
                    TEST_ASSERT(curElement->m_partitionIndex == index);
                    TEST_ASSERT(curElement->m_isHeader == true);
                }
            }
        }
    }
}

void IndexListTestSuite::firstElement() {
    unsigned int testCounts[] = {5, 10, 100, 500, 1000};
    unsigned int testPartitions[] = {1, 2, 5, 7, 10};
    unsigned int insertCount[] = {2, 4, 6, 10, 20};

    //Indices to insert

    unsigned int insertPartitions[] = {0, 0, 1, 1, 3, 4, 5, 5, 6, 6, 0, 1, 5, 8, 8, 6, 7, 9, 6, 6};
    unsigned int insertIndices[] = {1, 4, 0, 6, 32, 11, 132, 200, 9, 311, 488, 234, 666, 12, 43, 796, 100, 999, 55, 322};

    const unsigned int testCount = (sizeof (testCounts) / sizeof (unsigned int)) > (sizeof (testPartitions) / sizeof (unsigned int)) ?
                                    sizeof (testPartitions) / sizeof (unsigned int) :
                                    sizeof (testCounts) / sizeof (unsigned int);

    unsigned int testCaseIndex, curCount, curPartitions, curInsertCount;

    for (testCaseIndex = 0; testCaseIndex < testCount; testCaseIndex++) {
        curPartitions = testPartitions[testCaseIndex];
        curCount = testCounts[testCaseIndex];
        curInsertCount = insertCount[testCaseIndex] < curCount ? insertCount[testCaseIndex] : curCount - 1;

        IndexList<int> list(curCount, curPartitions);

        unsigned int index = 0, index2 = 0;
        for(;index < curInsertCount; index++)
        {
            //Make sure that the test contains only valid cases
            if(insertPartitions[index] < curPartitions && insertIndices[index] < curCount)
            {
                list.insert(insertPartitions[index], insertIndices[index]);
            }
        }

        //Test

        for(index = 0; index < curPartitions; index++)
        {
            //Search for last added element in index partition
            int lastAdded = -1;
            for(index2 = 0; index2 < curInsertCount; index2++)
                if(insertPartitions[index2] == index)
                    lastAdded = insertIndices[index2];

            TEST_ASSERT(list.firstElement(index) == lastAdded);
        }
    }
}
