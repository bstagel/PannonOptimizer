/* 
 * File:   unittest.h
 * Author: smidla
 *
 * Created on 2013. march 2., 21:40
 */

#ifndef UNITTEST_H
#define	UNITTEST_H

#include <string>
#include <vector>
#include <ctime>
#include "tester.h"

class UnitTest
{
protected:
    typedef void (UnitTest::*TestFunction)();

public:

    UnitTest(const char * name) : m_name(name)
    {
    }

    double run()
    {
        clock_t startTime, endTime;
        std::vector<TestFunction>::const_iterator iter = m_testFunctions.begin();
        std::vector<TestFunction>::const_iterator iterEnd = m_testFunctions.end();
        startTime = clock();
        for (; iter != iterEnd; iter++) {
            TestFunction function = *iter;
            (this->*function)();
        }
        endTime = clock();
        return (double) (endTime - startTime) / CLOCKS_PER_SEC;
    }

protected:

    void saveTestResult(const char * file,
        const char * function,
        unsigned int line,
        bool result,
        const char * source)
    {
        TestResult newResult;
        newResult.m_file = file;
        newResult.m_line = line;
        newResult.m_result = result;
        newResult.m_source = source;
        Tester::addNewResult(m_name, function, newResult);
        //Tester::m_results[m_name][function].push_back(newResult);
    }

    std::vector<TestFunction> m_testFunctions;
private:
    std::string m_name;
};

#define TEST_ASSERT(expr)                                               \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, (expr), #expr);     \
}

#define ADD_TEST(function)                                              \
{                                                                       \
    m_testFunctions.push_back(static_cast<TestFunction>(&function));    \
}

#endif	/* UNITTEST_H */

