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
#include <map>

#include <framework/tester.h>
#include <framework/unittest.h>

static std::basic_ostream<char> & operator<<(std::basic_ostream<char> & str, const std::nullptr_t &) {
    str << "nullptr";
    return str;
}

struct TestResult
{
    enum class TYPE {
        SIMPLE,
        EQUALITY,
        INEQUALITY,
        GREATER_OR_EQUAL,
        EMITTED,
        REGULAR_EXPRESSION
    };
    std::string m_file;
    unsigned int m_line;
    bool m_result;
    std::string m_source;
    std::string m_function;
    std::string m_comment;
    TYPE m_type;
};

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
        return m_failedAsserts;
    }
public:

    virtual ~UnitTest() {}

    enum class STATUS {PASSED, FAILED, FLAWLESS};

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
        double m_initTime;
        double m_executionTime;
        STATUS m_status;
        unsigned long long int m_asserts;
        unsigned long long int m_failedAsserts;
        std::vector<TestResult> m_results;
    };

public:

    UnitTest(const char * name) : m_name(name),
        m_failedTestFunctions(0),
        m_unimplementedTestFunctions(0),
        m_asserts(0),
        m_failedAsserts(0),
        m_executionTime(0),
        m_actualTestCase(nullptr)
    {
    }

    std::string getName() const {
        return m_name;
    }

    void setExtraInfo(const std::string & info) {
        Tester::setExtraInfo(info);
    }
    
    virtual unsigned int run()
    {
        clock_t startTime, endTime;
        clock_t functionStartTime, functionEndTime;
        unsigned int errorCount = 0;
        m_failedTestFunctions = 0;
        m_asserts = 0;
        m_failedAsserts = 0;
        m_unimplementedTestFunctions = 0;
        unsigned int index;
        std::vector<TestCase>::iterator iter = m_testCases.begin();
        std::vector<TestCase>::iterator iterEnd = m_testCases.end();
        startTime = clock();
        for (index = 1; iter != iterEnd; iter++, index++) {
            m_actualTestCase = &(*iter);
            auto oldAsserts = m_asserts;
            std::cout << "\t" << index << ": " << iter->m_name << " ";
            functionStartTime = clock();
            iter->m_tester->init(this, iter->m_function);
            functionEndTime = clock();
            iter->m_initTime = (double)(functionEndTime - functionStartTime) / CLOCKS_PER_SEC;

            functionStartTime = clock();
            errorCount += iter->m_tester->test();
            functionEndTime = clock();
            iter->m_executionTime = (double)(functionEndTime - functionStartTime) / CLOCKS_PER_SEC;

            unsigned int failedAsserts = iter->m_results.size();
            if (failedAsserts == 0) {
                std::cout << " PASSED" << std::endl;
                if (m_asserts == oldAsserts) {
                    iter->m_status = STATUS::PASSED;
                } else {
                    iter->m_status = STATUS::FLAWLESS;
                }
            } else {
                m_failedTestFunctions++;
                std::cout << " FAILED: " << errorCount << " errors" << std::endl;
                iter->m_status = STATUS::FAILED;
            }
            if (m_asserts == oldAsserts) {
                m_unimplementedTestFunctions++;
            }
            iter->m_asserts = m_asserts - oldAsserts;
            iter->m_failedAsserts = iter->m_results.size();
            m_failedAsserts += iter->m_results.size();
            delete iter->m_tester;
        }
        endTime = clock();
        m_executionTime = (double) (endTime - startTime) / CLOCKS_PER_SEC;

        return errorCount;
    }

    unsigned long long int getTestFunctions() const {
        return m_testCases.size();
    }

    unsigned long long int getFailedTestFunctions() const {
        return m_failedTestFunctions;
    }

    unsigned long long int getAsserts() const {
        return m_asserts;
    }

    unsigned long long int getFailedAsserts() const {
        return m_failedAsserts;
    }

    std::string getStatusString() const {
        if (m_failedAsserts == 0) {
            return "PASSED";
        }
        return "FAILED";
    }

    unsigned long long getUnimplementedTestFunctions() const {
        return m_unimplementedTestFunctions;
    }

    double getExecutionTime() const {
        return m_executionTime;
    }

    void addExtraInfo(const std::string & key, const std::string & value) {
        m_extraInfos.insert( {key, value} );
    }

    const std::map<std::string, std::string> & getExtraInfos() const {
        return m_extraInfos;
    }

    ReportModule * getReportModule() const
    {
        ReportModule * module = new ReportModule;

        generateSummaryReport(module);
        generateFunctionSummaryReport(module);

        return module;
    }

    void generateSummaryReport(ReportModule * module) const {
        ReportTable table(2);
        table.setTitle("Summary");

        std::string executionTime = std::to_string((int)m_executionTime) + "." + std::to_string((int)(m_executionTime*100) % 100) + " seconds";
        std::string testFunctions = std::to_string(getTestFunctions());
        std::string failedTestFunctions = std::to_string(getFailedTestFunctions());
        std::string unimplementedTestFunctions = std::to_string(getUnimplementedTestFunctions());
        std::string asserts = std::to_string(getAsserts());
        std::string failedAsserts = std::to_string(getFailedAsserts());
        std::string status = getStatusString();

        ReportTable::ALIGNMENT left = ReportTable::ALIGNMENT::LEFT;
        ReportTable::ALIGNMENT right = ReportTable::ALIGNMENT::RIGHT;

        table.setCell(0, 0, {"Execution time", left, nullptr, false });
        table.setCell(0, 1, {executionTime, right, nullptr, false });

        table.setCell(1, 0, {"Test functions", left, nullptr, false });
        table.setCell(1, 1, {testFunctions, right, nullptr, false });

        table.setCell(2, 0, {"Failed test functions", left, nullptr, false });
        table.setCell(2, 1, {failedTestFunctions, right, nullptr, false });

        table.setCell(3, 0, {"Unimplemented test functions", left, nullptr, false });
        table.setCell(3, 1, {unimplementedTestFunctions, right, nullptr, false });

        table.setCell(4, 0, {"Asserts", left, nullptr, false });
        table.setCell(4, 1, {asserts, right, nullptr, false });

        table.setCell(5, 0, {"Failed asserts", left, nullptr, false });
        table.setCell(5, 1, {failedAsserts, right, nullptr, false });

        table.setCell(6, 0, {"Status", left, nullptr, false });
        table.setCell(6, 1, {status, right, nullptr, false });

        module->addTable(table);

    }

    void generateFunctionSummaryReport(ReportModule * module) const {
        ReportTable table(7);

        ReportTable::ALIGNMENT left = ReportTable::ALIGNMENT::LEFT;
        ReportTable::ALIGNMENT right = ReportTable::ALIGNMENT::RIGHT;
        ReportTable::ALIGNMENT center = ReportTable::ALIGNMENT::CENTER;

        table.setTitle("Test function summary");
        table.setCell(0, 0, {"Name", center, nullptr, true});
        table.setCell(0, 1, {"Initialization time", center, nullptr, true});
        table.setCell(0, 2, {"Execution time", center, nullptr, true});
        table.setCell(0, 3, {"Status", center, nullptr, true});
        table.setCell(0, 4, {"Asserts", center, nullptr, true});
        table.setCell(0, 5, {"Failed asserts", center, nullptr, true});
        table.setCell(0, 6, {"Details", center, nullptr, true});

        table.setColumn(0, {true, ReportTable::TYPE::STRING});
        table.setColumn(1, {true, ReportTable::TYPE::FLOAT});
        table.setColumn(2, {true, ReportTable::TYPE::FLOAT});
        table.setColumn(3, {true, ReportTable::TYPE::STRING});
        table.setColumn(4, {true, ReportTable::TYPE::INTEGER});
        table.setColumn(5, {true, ReportTable::TYPE::INTEGER});
        table.setColumn(6, {false, ReportTable::TYPE::STRING});

        unsigned int functionIndex = 1;
        for (auto & function: m_testCases) {

            std::string initTime = std::to_string((int)function.m_initTime) + "." + std::to_string((int)(function.m_initTime*100) % 100) + " seconds";
            std::string executionTime = std::to_string((int)function.m_executionTime) + "." + std::to_string((int)(function.m_executionTime*100) % 100) + " seconds";
            std::string status;
            switch (function.m_status) {
            case STATUS::FAILED:
                status = "FAILED";
                break;
            case STATUS::PASSED:
                status = "PASSED";
                break;
            case STATUS::FLAWLESS:
                status = "FLAWLESS";
                break;
            }
            std::string asserts = std::to_string(function.m_asserts);
            std::string failedAsserts = std::to_string(function.m_failedAsserts);

            table.setCell(functionIndex, 0, { function.m_name, left, nullptr, false });
            table.setCell(functionIndex, 1, { initTime, right, nullptr, false });
            table.setCell(functionIndex, 2, { executionTime, right, nullptr, false });
            table.setCell(functionIndex, 3, { status, right, nullptr, false});
            table.setCell(functionIndex, 4, { asserts, right, nullptr, false});
            table.setCell(functionIndex, 5, { failedAsserts, right, nullptr, false});

            // generate module for the current function

            if ( function.m_results.size() > 0) {
                ReportModule * newModule = generateFunctionModule(function);
                table.setCell(functionIndex, 6, { "link", right, newModule, false});
            }

            functionIndex++;
        }

        module->addTable(table);
    }

    ReportModule * generateFunctionModule( const TestCase & test ) const {
        ReportModule * newModule = new ReportModule;
        newModule->setName( test.m_name + " summary" );
        ReportTable table(6);

        ReportTable::ALIGNMENT left = ReportTable::ALIGNMENT::LEFT;
        ReportTable::ALIGNMENT right = ReportTable::ALIGNMENT::RIGHT;
        ReportTable::ALIGNMENT center = ReportTable::ALIGNMENT::CENTER;

        table.setTitle("Assert summary");
        table.setCell(0, 0, {"File", center, nullptr, true});
        table.setCell(0, 1, {"Function", center, nullptr, true});
        table.setCell(0, 2, {"Line", center, nullptr, true});
        table.setCell(0, 3, {"Type", center, nullptr, true});
        table.setCell(0, 4, {"Source", center, nullptr, true});
        table.setCell(0, 5, {"Comment", center, nullptr, true});

        table.setColumn(0, {true, ReportTable::TYPE::STRING});
        table.setColumn(1, {true, ReportTable::TYPE::STRING});
        table.setColumn(2, {true, ReportTable::TYPE::INTEGER});
        table.setColumn(3, {true, ReportTable::TYPE::STRING});
        table.setColumn(4, {true, ReportTable::TYPE::STRING});
        table.setColumn(5, {true, ReportTable::TYPE::STRING});

        unsigned int assertIndex = 1;
        for (auto & assert: test.m_results) {

            std::string file = assert.m_file;
            std::string function = assert.m_function;
            std::string line = std::to_string( assert.m_line );
            std::string type;
            switch ( assert.m_type ) {
            case TestResult::TYPE::SIMPLE:
                type = "simple";
                break;
            case TestResult::TYPE::EQUALITY:
                type = "equality";
                break;
            case TestResult::TYPE::INEQUALITY:
                type = "inequality";
                break;
            case TestResult::TYPE::GREATER_OR_EQUAL:
                type = "greater or equal";
                break;
            case TestResult::TYPE::EMITTED:
                type = "emitted";
                break;
            case TestResult::TYPE::REGULAR_EXPRESSION:
                type = "regular expresssion";
                break;
            }
            std::string source = assert.m_source;
            std::string comment = assert.m_comment;

            table.setCell(assertIndex, 0, {file, left, nullptr, false});
            table.setCell(assertIndex, 1, {function, left, nullptr, false});
            table.setCell(assertIndex, 2, {line, right, nullptr, false});
            table.setCell(assertIndex, 3, {type, left, nullptr, false});
            table.setCell(assertIndex, 4, {source, left, nullptr, false});
            table.setCell(assertIndex, 5, {comment, left, nullptr, false});

            assertIndex++;
        }

        newModule->addTable(table);
        return newModule;
    }

protected:

    void addNewResult(std::string function, TestResult result)
    {
        m_asserts++;
        if (result.m_result == false) {
            result.m_function = function;
            std::cout << std::endl << std::endl << "\tFile: " << result.m_file << std::endl;
            std::cout << "\tFunction: " << function << std::endl;
            std::cout << "\tLine: " << result.m_line << std::endl;
            std::cout << "\t" << result.m_source << std::endl;
            if (result.m_comment.length() > 0) {
                std::cout << "\t" << result.m_comment << std::endl;
            }
            if (Tester::getExtraInfo().length() > 0) {
                std::cout << "\t Extra info: " << Tester::getExtraInfo() << std::endl;
            }
            std::cout << std::endl;
            m_actualTestCase->m_results.push_back(result);
        }
    }

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
        newResult.m_type = TestResult::TYPE::SIMPLE;
        addNewResult(function, newResult);
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
        newResult.m_type = TestResult::TYPE::EQUALITY;
        newResult.m_source = source;
        //std::cout << "itt jo" << std::endl;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[EQUALITY TEST FAILED] " << testedValue << " != " << referenceValue;
            newResult.m_comment = os.str();
        } else {
            newResult.m_comment = std::string("[EQUALITY] ") + std::string(source);
        }
        addNewResult(function, newResult);
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
        newResult.m_type = TestResult::TYPE::INEQUALITY;
        newResult.m_source = source;
        //std::cout << "itt jo" << std::endl;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[INEQUALITY TEST FAILED] " << testedValue << " == " << referenceValue;
            newResult.m_comment = os.str();
        } else {
            newResult.m_comment = std::string("[INEQUALITY] ") + std::string(source);
        }
        addNewResult(function, newResult);
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
        newResult.m_type = TestResult::TYPE::GREATER_OR_EQUAL;
        newResult.m_source = source;
        if (newResult.m_result == false) {
            std::ostringstream os;
            os << "[INEQUALITY TEST FAILED] " << testedValue << " < " << referenceValue;
            newResult.m_comment = os.str();
        } else {
            newResult.m_comment = std::string("[INEQUALITY] ") + std::string(source);
        }
        addNewResult(function, newResult);
    }

    std::vector<TestCase> m_testCases;
private:
    std::string m_name;

    unsigned long long int m_failedTestFunctions;

    unsigned long long int m_unimplementedTestFunctions;

    unsigned long long int m_asserts;

    unsigned long long int m_failedAsserts;

    double m_executionTime;

    TestCase * m_actualTestCase;

    std::map<std::string, std::string> m_extraInfos;
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

