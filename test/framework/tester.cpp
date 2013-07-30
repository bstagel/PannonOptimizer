#include <iostream>
#include "tester.h"
#include "unittest.h"


std::list<UnitTest*> Tester::sm_tests;
std::map<std::string, std::map< std::string, std::list<TestResult> > > Tester::sm_results;
unsigned int Tester::sm_totalCounter = 0;
unsigned int Tester::sm_subCounter = 0;
bool Tester::sm_firstTest = true;
bool Tester::sm_actualIsGood = true;
unsigned int Tester::sm_totalErrorCounter = 0;

void Tester::run()
{
    std::list<UnitTest*>::iterator iter = sm_tests.begin();
    std::list<UnitTest*>::iterator iterEnd = sm_tests.end();
    double time;
    double totalTime = 0;
    for (; iter != iterEnd; iter++) {
        time = (*iter)->run();
        totalTime += time;
        std::cout << std::endl << "\tEllapsed time: " << time << " sec" << std::endl;
        delete *iter;
    }
    if (sm_actualIsGood) {
        std::cout << " PASSED" << std::endl;
    } else {
        sm_totalErrorCounter++;
        std::cout << " FAILED" << std::endl;
    }
    std::cout << std::endl;
    std::cout << sm_totalErrorCounter << " errors from " << sm_totalCounter <<
        ", " << (1.0 - (sm_totalErrorCounter / (double) sm_totalCounter))*100 << "%" << std::endl;
    std::cout << std::endl << "Ellapsed time: " << totalTime << " sec" << std::endl;
}

class DemoTestCase : public UnitTest
{
public:

    DemoTestCase(const char * name) : UnitTest(name)
    {
    }

    void run()
    {
        test1();
        test2();
    }

    void test1()
    {
        TEST_ASSERT(3 + 1 == 4);
        TEST_ASSERT(2 + 10 == 11);
        TEST_ASSERT(3 * 3 == 9);
    }

    void test2()
    {
        TEST_ASSERT(4 == 4);
        TEST_ASSERT(4 == 2 * 2);
    }
};

class DemoTestCase2 : public UnitTest
{
public:

    DemoTestCase2(const char * name) : UnitTest(name)
    {
    }

    void run()
    {
        foo();
        bar();
    }

    void foo()
    {
        TEST_ASSERT(3 + 1 == 4);
        TEST_ASSERT(false);
        TEST_ASSERT(3 * 3 == 9);
    }

    void bar()
    {
        TEST_ASSERT(4 == 4);
        TEST_ASSERT(4 == 2 * 2);
    }
};

/*int main()
{
    Tester::addUnitTest(new DemoTestCase("Demo Test"));
    Tester::addUnitTest(new DemoTestCase2("FooBar Test"));

    Tester::run();
}*/