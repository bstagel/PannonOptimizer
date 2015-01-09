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
#include <sstream>
#include <functional>
#include <algorithm>

#include <framework/tester.h>

static std::basic_ostream<char> & operator<<(std::basic_ostream<char> & str, const std::nullptr_t &) {
    str << "nullptr";
    return str;
}

class UnitTest
{
public:
    typedef void (UnitTest::*TestFunction)();
private:
    unsigned int runTestFunction(TestFunction function)
    {
        // eliminate unused function warning
        std::ostringstream os;
        os << nullptr;

        (this->*function)();
        return m_errorCounter;
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

        unsigned int runTest()
        {
            return m_unitTest->runTestFunction(m_function);
        }

        virtual unsigned int test()
        {
            //TestFunction function = *iter;
            return runTest();
            //            (this->*function)();
        }
        
        void setExtraInfo(const std::string & info) {
            m_unitTest->setExtraInfo(info);
        }
    };


protected:

    struct TestCase
    {
        std::string m_name;
        TestFunction m_function;
        DefaultTester * m_tester;
    };

public:

    UnitTest(const char * name) : m_name(name),
        m_errorCounter(0)
    {
    }

    std::string getName() const {
        return m_name;
    }

    void setExtraInfo(const std::string & info) {
        Tester::setExtraInfo(info);
    }
    
    virtual unsigned int run(double * timePtr)
    {
        clock_t startTime, endTime;
        unsigned int errorCount = 0;
        unsigned int index;
        std::vector<TestCase>::iterator iter = m_testCases.begin();
        std::vector<TestCase>::iterator iterEnd = m_testCases.end();
        startTime = clock();
        for (index = 1; iter != iterEnd; iter++, index++) {
            m_errorCounter = 0;
            std::cout << "\t" << index << ": " << iter->m_name << " ";
            iter->m_tester->init(this, iter->m_function);
            errorCount += iter->m_tester->test();
            if (errorCount == 0) {
                std::cout << " PASSED" << std::endl;
            } else {
                std::cout << " FAILED: " << errorCount << " errors" << std::endl;
            }
            delete iter->m_tester;
        }
        endTime = clock();
        *timePtr = (double) (endTime - startTime) / CLOCKS_PER_SEC;
        return errorCount;
    }

protected:

    template<class TYPE>
    void testWithPermutation(const TYPE * array,
                             unsigned int size,
                             std::function<void (const TYPE *, unsigned int)> test) {
        std::vector<TYPE> input(size);
        input = array;
        do {
            test(input.data(), size);
        } while ( std::next_permutation( input.begin(), input.end() ) );
    }

    template<class TYPE>
    void testWithPermutation(const std::initializer_list<TYPE> & list,
                             std::function<void (const TYPE *, unsigned int)> test) {
        testWithPermutation(&(*list.begin()), list.size(), test);
    }

    template<class TYPE1, class TYPE2>
    void testWithPermutationPair(const TYPE1 * array1,
                                 unsigned int size1,
                                 const TYPE2 * array2,
                                 unsigned int size2,
                                 std::function<void (const TYPE1 *, unsigned int,
                                                     const TYPE2 *, unsigned int)> test) {
        std::vector<TYPE1> input1(size1);
        std::copy(array1, array1 + size1, input1.begin());
        //input1 = array1;

        do {
            std::vector<TYPE2> input2(size2);
            std::copy(array2, array2 + size2, input2.begin());

            do {

                test(input1.data(), size1, input2.data(), size2);

            } while (std::next_permutation( input2.begin(), input2.end() ) );
        } while ( std::next_permutation( input1.begin(), input1.end() ) );
    }

    template<class TYPE1, class TYPE2>
    void testWithPermutationPair(const std::initializer_list<TYPE1> & list1,
                                 const std::initializer_list<TYPE2> & list2,
                                 std::function<void (const TYPE1 *, unsigned int,
                                                     const TYPE2 *, unsigned int)> test) {
        const TYPE1 * array1 = &(*list1.begin());
        const TYPE2 * array2 = &(*list2.begin());
        testWithPermutationPair(array1, list1.size(),
                                array2, list2.size(), test);
    }

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
        newResult.m_source = std::string("Source: ") + source;
        m_errorCounter += result == false;
        Tester::addNewResult(m_name, function, newResult);
        //Tester::m_results[m_name][function].push_back(newResult);
    }

    template<class COMPARABLE_TYPE1, class COMPARABLE_TYPE2>
    void saveEqualityTestResult(const char * file,
                                const char * function,
                                unsigned int line,
                                const COMPARABLE_TYPE1 & testedValue,
                                const COMPARABLE_TYPE2 & referenceValue,
                                const std::string & source) {
        TestResult newResult;
        newResult.m_file = file;
        newResult.m_line = line;
        newResult.m_result = testedValue == referenceValue;
        //std::cout << "itt jo" << std::endl;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[EQUALITY TEST FAILED] " << testedValue << " != " << referenceValue << "\n\tsource: " << source;
            newResult.m_source = os.str();
            m_errorCounter++;
        } else {
            newResult.m_source = std::string("[EQUALITY] ") + std::string(source);
        }
        Tester::addNewResult(m_name, function, newResult);
    }

    template<class COMPARABLE_TYPE1, class COMPARABLE_TYPE2>
    void saveInequalityTestResult(const char * file,
                                  const char * function,
                                  unsigned int line,
                                  const COMPARABLE_TYPE1 & testedValue,
                                  const COMPARABLE_TYPE2 & referenceValue,
                                  const std::string & source) {
        TestResult newResult;
        newResult.m_file = file;
        newResult.m_line = line;
        newResult.m_result = testedValue != referenceValue;
        //std::cout << "itt jo" << std::endl;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[INEQUALITY TEST FAILED] " << testedValue << " == " << referenceValue << "\n\tsource: " << source;
            newResult.m_source = os.str();
            m_errorCounter++;
        } else {
            newResult.m_source = std::string("[INEQUALITY] ") + std::string(source);
        }
        Tester::addNewResult(m_name, function, newResult);
    }

    template<class COMPARABLE_TYPE1, class COMPARABLE_TYPE2>
    void saveGreaterOrEqualTestResult(const char * file,
                                      const char * function,
                                      unsigned int line,
                                      const COMPARABLE_TYPE1 & testedValue,
                                      const COMPARABLE_TYPE2 & referenceValue,
                                      const std::string & source) {
        TestResult newResult;
        newResult.m_file = file;
        newResult.m_line = line;
        newResult.m_result = testedValue >= referenceValue;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[INEQUALITY TEST FAILED] " << testedValue << " < " << referenceValue << "\n\tsource: " << source;
            newResult.m_source = os.str();
            m_errorCounter++;
        } else {
            newResult.m_source = std::string("[INEQUALITY] ") + std::string(source);
        }
        Tester::addNewResult(m_name, function, newResult);
    }

    std::vector<TestCase> m_testCases;
private:
    std::string m_name;

    unsigned int m_errorCounter;
};

#define EMIT_ERROR(expr)                                                \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, false, #expr);     \
    }


#define TEST_ASSERT(expr)                                               \
{                                                                       \
    saveTestResult(__FILE__, __FUNCTION__, __LINE__, (expr), #expr);     \
    }

#define TEST_ASSERT_EQUALITY(testValue, referenceValue) \
{ \
    saveEqualityTestResult<decltype(testValue), decltype(referenceValue)>(__FILE__, __FUNCTION__, __LINE__,  \
    testValue, referenceValue, std::string(#testValue) + ", " + std::string(#referenceValue) ); \
    }

#define TEST_ASSERT_INEQUALITY(testValue, referenceValue) \
{ \
    saveInequalityTestResult<decltype(testValue)>(__FILE__, __FUNCTION__, __LINE__,  \
    testValue, referenceValue, std::string(#testValue) + ", " + std::string(#referenceValue) ); \
    }

#define TEST_ASSERT_GREATER_OR_EQUAL(testValue, referenceValue) \
{ \
    saveGreaterOrEqualTestResult<decltype(testValue)>(__FILE__, __FUNCTION__, __LINE__,  \
    testValue, referenceValue, std::string(#testValue) + ", " + std::string(#referenceValue) ); \
    }

#define ADD_TEST(function)                                              \
{                                                                       \
    TestCase testCase;                                                  \
    testCase.m_name = #function;                                        \
    std::string::size_type lastScopeIndex = testCase.m_name.find_last_of(":");    \
    if (lastScopeIndex != std::string::npos) {                          \
        testCase.m_name = testCase.m_name.substr(lastScopeIndex + 1);   \
    }                                                                   \
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

