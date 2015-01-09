#include <iostream>
#include <fstream>
#include <framework/tester.h>
#include <framework/unittest.h>


std::list<UnitTest*> Tester::sm_tests;
std::map<std::string, std::map< std::string, std::list<TestResult> > > Tester::sm_results;
unsigned int Tester::sm_totalCounter = 0;
unsigned int Tester::sm_subCounter = 0;
bool Tester::sm_firstTest = true;
bool Tester::sm_actualIsGood = true;
unsigned int Tester::sm_totalErrorCounter = 0;
std::string Tester::sm_extraInfo;
std::string Tester::sm_title;
tm Tester::sm_startTime;
double Tester::sm_executionTime;
unsigned int Tester::sm_failedUnitTests;

void Tester::run()
{
    time_t now = time(0);
    sm_startTime = *localtime(&now);

    std::list<UnitTest*>::iterator iter = sm_tests.begin();
    std::list<UnitTest*>::iterator iterEnd = sm_tests.end();
    double time;
    sm_executionTime = 0;
    for (; iter != iterEnd; iter++) {
        std::cout << "Unit test: " << (*iter)->getName() << std::endl;
        (*iter)->run(&time);
        sm_executionTime += time;
        std::cout << std::endl << "\tEllapsed time: " << time << " sec" << std::endl;
        delete *iter;
    }
    std::cout << std::endl;
    std::cout << sm_totalErrorCounter << " errors from " << sm_totalCounter <<
        ", " << (1.0 - (sm_totalErrorCounter / (double) sm_totalCounter))*100 << "%" << std::endl;
    std::cout << std::endl << "Ellapsed time: " << sm_executionTime << " sec" << std::endl;
}

void Tester::generateHtmlOutput(const char *directory)
{
    std::ofstream output;
    std::string fileName = std::string(directory);
    char delim;
#ifdef __linux__
    delim = '/';
#endif
#ifdef WIN32
    delim = '\\';
#endif
    if (fileName[ fileName.size() - 1 ] != delim) {
        fileName = fileName + delim + "index.html";
    } else {
        fileName = fileName + "index.html";
    }

    system((std::string("mkdir ") + directory).c_str());

    output.open(fileName.c_str());
    if (output.is_open() == false) {
        // TODO: throw an exception
        return;
    }

    generateHtmlHead(output);
    generateHtmlBody(output);

    output.close();
}

void Tester::generateLatexOutput(const char *directory)
{
    directory = nullptr; // avoid warning
}

void Tester::setReportTitle(const char *title)
{
    sm_title = title;
}

void Tester::generateHtmlHead(std::ostream &os)
{
    os << "<!DOCTYPE html>" << std::endl;
    os << "<html>" << std::endl;
    os << "<head>" << std::endl;
    os << "\t<title>" << sm_title << "</title" << std::endl;
    os << "</head>" << std::endl;
}

void Tester::generateHtmlBody(std::ostream &os)
{
    os << "<body>" << std::endl;

    os << "\t<h1>" << sm_title << "</h1>" << std::endl;

    generateHtmlSummary(os);
    generateUnitTestSummary(os);

    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

void Tester::generateHtmlSummary(std::ostream &os)
{
    os << "\t<h2>Summary </h2>" << std::endl;
    std::vector<std::vector<ReportCell>> summaryTable;

    char timeBuff[80];
    strftime(timeBuff, sizeof(timeBuff), "%Y.%m.%d. %X", &sm_startTime);

    summaryTable = {{ {"Date", "", false}, { timeBuff, "align=\"right\"", false} },
                   { {"Execution time", "", false}, { std::to_string((int)sm_executionTime) + "." + std::to_string((int)(sm_executionTime*100)) + " seconds", "align=\"right\"", false} },
                   { {"Unit tests", "", false}, { std::to_string( sm_tests.size() ), "align=\"right\"", false} },
                   { {"Failed unit tests", "", false}, { std::to_string(sm_failedUnitTests), "align=\"right\"", false} }};

    generateTable(os, summaryTable);
}

void Tester::generateUnitTestSummary(std::ostream &os)
{

}

void Tester::generateTable(std::ostream &os,
                           const std::vector<std::vector<Tester::ReportCell> > &table)
{
    os << "\t<table border=\"1\">" << std::endl;
    for (auto & row: table) {
        os << "\t\t<tr>" << std::endl;
        for (auto & cell: row) {
            if (cell.m_headCell) {
                os << "\t\t\t<th";
            } else {
                os << "\t\t\t<td";
            }
            if (cell.m_attributes.length() > 0) {
                os << " " << cell.m_attributes;
            }
            os << "> " << cell.m_value << " </t";
            if (cell.m_headCell) {
                os << "h>";
            } else {
                os << "d>";
            }
            os << std::endl;
        }
        os << "\t\t</tr>" << std::endl;
    }
    os << "\t</table>" << std::endl;
}

class DemoTestCase : public UnitTest
{
public:

    DemoTestCase(const char * name) : UnitTest(name)
    {
    }

    double run()
    {
        test1();
        test2();
        return 1;
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

    double run()
    {
        foo();
        bar();
        return 1.0;
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
