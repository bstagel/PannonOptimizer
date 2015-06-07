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
#include <vector>
#include <ctime>
#include <framework/report.h>

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

    static void generateHtmlOutput(const char * directory);

    static void generateLatexOutput(const char * directory);

    static void setReportTitle(const char * title);

    static unsigned long long int getUnimplementedUnitTests();

    static unsigned long long int getTestFunctions();

    static unsigned long long int getFailedTestFunctions();

    static unsigned long long int getUnimplementedTestFunctions();

    static unsigned long long int getAsserts();

    static unsigned long long int getFailedAsserts();

    static std::string getStatusString();

    static std::string getExtraInfo();
private:

    static const unsigned int sm_simpleCell = 0;
    static const unsigned int sm_headCell = 1;
    static const unsigned int sm_sortCell = 2;

    struct ReportCell {
        std::string m_value;
        std::string m_attributes;
        unsigned int m_httpSettings;
    };

    static std::list<UnitTest*> sm_tests;
    static unsigned int sm_totalErrorCounter;
    static std::string sm_extraInfo;
    static std::string sm_title;
    static tm sm_startTime;

    static double sm_executionTime;
    static unsigned int sm_failedUnitTests;

    static void setExtraInfo(const std::string & info)
    {
        sm_extraInfo = info;
    }

    static void generateSummaryReport(ReportModule * module);

    static void generateHtmlHead(std::ostream & os);

    static void generateHtmlBody(std::ostream & os);

    static void generateHtmlSummary(std::ostream & os);

    static void generateUnitTestSummary(std::ostream & os, std::string *script);

    static void generateTable(std::ostream & os,
                              const std::vector<std::vector<ReportCell>> & table,
                              const std::string & name = "");

    static void generateTableScript(const std::vector<std::vector<ReportCell>> & table,
                                    const std::string &tableName,
                                    std::string * script);

    static std::string getCamelCase(const std::string & value, bool lower = false);
};

#endif	/* TESTER_H */

