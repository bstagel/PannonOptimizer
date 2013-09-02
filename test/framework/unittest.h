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
public:
    typedef void (UnitTest::*TestFunction)();
private:
    void runTestFunction(TestFunction function)
    {
        (this->*function)();
    }
public:

    virtual ~UnitTest() {}

    class DefaultTester
    {
        UnitTest * m_unitTest;
        TestFunction m_function;
    public:

        virtual ~DefaultTester() {}

        void init(UnitTest * unitTest, TestFunction function)
        {
            m_unitTest = unitTest;
            m_function = function;
        }

        void runTest()
        {
            m_unitTest->runTestFunction(m_function);
        }

        virtual void test()
        {
            //TestFunction function = *iter;
            runTest();
            //            (this->*function)();
        }
        
        void setExtraInfo(const std::string & info) {
            m_unitTest->setExtraInfo(info);
        }
    };


protected:

    struct TestCase
    {
        TestFunction m_function;
        DefaultTester * m_tester;
    };

public:

    UnitTest(const char * name) : m_name(name)
    {
    }

    void setExtraInfo(const std::string & info) {
        Tester::setExtraInfo(info);
    }
    
    virtual double run()
    {
        clock_t startTime, endTime;
        std::vector<TestCase>::iterator iter = m_testCases.begin();
        std::vector<TestCase>::iterator iterEnd = m_testCases.end();
        startTime = clock();
        for (; iter != iterEnd; iter++) {
            iter->m_tester->init(this, iter->m_function);
            iter->m_tester->test();
            delete iter->m_tester;
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

    std::vector<TestCase> m_testCases;
private:
    std::string m_name;
};

#define EMIT_ERROR(expr)                                                \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, false, #expr);     \
}


#define TEST_ASSERT(expr)                                               \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, (expr), #expr);     \
}

#define ADD_TEST(function)                                              \
{                                                                       \
    TestCase testCase;                                                  \
    testCase.m_function = static_cast<TestFunction>(&function);          \
    testCase.m_tester = new DefaultTester;                               \
    m_testCases.push_back(testCase);    \
}

#define ADD_TEST_EXTRA(function, operation)                             \
{                                                                       \
    TestCase testCase;                                                  \
    testCase.m_function = static_cast<TestFunction>(&function);          \
    testCase.m_tester = new operation;                                     \
    m_testCases.push_back(testCase);    \
}


#endif	/* UNITTEST_H */

