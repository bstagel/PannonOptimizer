/* 
 * File:   unittest.h
 * Author: smidla
 *
 * Created on 2013. march 2., 21:40
 */

#ifndef UNITTEST_H
#define	UNITTEST_H

#include <string>
#include "tester.h"

class UnitTest
{
public:

    UnitTest(const char * name) : m_name(name)
    {
    }

    virtual void run() = 0;
    
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
private:
    std::string m_name;
};

#define TEST_ASSERT(expr)                                               \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, (expr), #expr);     \
}

#endif	/* UNITTEST_H */

