/* 
 * File:   tester.h
 * Author: smidla
 *
 * Created on 2013. march 2., 21:36
 */

#ifndef TESTER_H
#define	TESTER_H

#include <iostream>
#include <list>
#include <map>
#include <string>

struct TestResult
{
    std::string m_file;
    unsigned int m_line;
    bool m_result;
    std::string m_source;
};

class UnitTest;

class Tester
{
    friend class UnitTest;
public:

    static void addUnitTest(UnitTest * newTest)
    {
        sm_tests.push_back(newTest);
    }

    static void run();

private:
    static std::list<UnitTest*> sm_tests;
    static unsigned int sm_totalCounter;
    static unsigned int sm_subCounter;
    static bool sm_firstTest;
    static bool sm_actualIsGood;
    static unsigned int sm_totalErrorCounter;

    // key: name of the test unit
    static std::map<std::string, std::map< std::string, std::list<TestResult> > > sm_results;

    static void addNewResult(std::string unit, std::string test, TestResult result)
    {
        std::map<std::string, std::map< std::string, std::list<TestResult> > >::iterator iter = sm_results.find(unit);
        std::map<std::string, std::map< std::string, std::list<TestResult> > >::iterator iterEnd = sm_results.end();
        if (iter == iterEnd) {
            if (!sm_firstTest) {
                if (sm_actualIsGood) {
                    std::cout << " PASSED" << std::endl;
                } else {
                    sm_totalErrorCounter++;
                    std::cout << " FAILED" << std::endl;
                }
            }
            sm_totalCounter++;
            sm_firstTest = false;
            sm_actualIsGood = true;
            std::cout << "Unit test: " << unit << std::endl;
            sm_subCounter = 1;
            std::cout << "\t" << sm_subCounter << ": " << test;

        } else {
            std::map< std::string, std::list<TestResult> >::iterator iter2 = (*iter).second.find(test);
            std::map< std::string, std::list<TestResult> >::iterator iterEnd2 = (*iter).second.end();
            if (iter2 == iterEnd2) { // new test case
                if (sm_actualIsGood) {
                    std::cout << " PASSED" << std::endl;
                } else {
                    sm_totalErrorCounter++;
                    std::cout << " FAILED" << std::endl;
                }

                sm_totalCounter++;
                sm_subCounter++;
                sm_actualIsGood = true;
                std::cout << "\t" << sm_subCounter << ": " << test;
            }
        }
        if (result.m_result == false) {
            std::cout << std::endl << std::endl << "\tFile: " << result.m_file << std::endl;
            std::cout << "\tFunction: " << test << std::endl;
            std::cout << "\tLine: " << result.m_line << std::endl;
            std::cout << "\tSource:" << result.m_source << std::endl << std::endl;
            sm_actualIsGood = false;
        }
        sm_results[unit][test].push_back(result);
    }

    static void generateHtmlOutput(std::ostream &)
    {

    }
};

#endif	/* TESTER_H */

