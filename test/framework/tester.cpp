#include <iostream>
#include <fstream>
#include <sstream>
#include <framework/tester.h>
#include <framework/unittest.h>
#include <framework/htmlreportgenerator.h>


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

    sm_executionTime = 0;
    sm_failedUnitTests = 0;
    for (auto & unitTest: sm_tests) {
        std::cout << "Unit test: " << unitTest->getName() << std::endl;
        if (unitTest->run() > 0) {
            sm_failedUnitTests++;
        }
        sm_executionTime += unitTest->getExecutionTime();
        std::cout << std::endl << "\tEllapsed time: " << unitTest->getExecutionTime() << " sec" << std::endl;
        //delete *iter;
    }
    std::cout << std::endl;
    unsigned long long int functions = getTestFunctions();
    unsigned long long int failedFunctions = getFailedTestFunctions();
    std::cout << failedFunctions << " failed test cases from " << functions <<
                 ", " << (1.0 - (failedFunctions / (double) functions))*100 << "%" << std::endl;
    std::cout << std::endl << "Ellapsed time: " << sm_executionTime << " sec" << std::endl;
}

void Tester::generateHtmlOutput(const char *directory)
{
    Report report;
    ReportModule summaryModule;
    generateSummaryReport(&summaryModule);
    report.addModule(summaryModule);
    HtmlReportGenerator generator;
    generator.save(report, directory);
}

void Tester::generateLatexOutput(const char *directory)
{
    directory = nullptr; // avoid warning
}

void Tester::setReportTitle(const char *title)
{
    sm_title = title;
}

unsigned long long Tester::getUnimplementedUnitTests()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        if (unitTest->getAsserts() == 0) {
            result++;
        }
    }
    return result;
}

unsigned long long Tester::getTestFunctions()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        result += unitTest->getTestFunctions();
    }
    return result;
}

unsigned long long Tester::getFailedTestFunctions()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        result += unitTest->getFailedTestFunctions();
    }
    return result;
}

unsigned long long Tester::getUnimplementedTestFunctions()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        result += unitTest->getUnimplementedTestFunctions();
    }
    return result;
}

unsigned long long Tester::getAsserts()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        result += unitTest->getAsserts();
    }
    return result;
}

unsigned long long Tester::getFailedAsserts()
{
    unsigned long long result = 0;
    for (auto & unitTest: sm_tests) {
        result += unitTest->getFailedAsserts();
    }
    return result;
}

std::string Tester::getStatusString()
{
    if (getFailedAsserts() == 0) {
        return "PASSED";
    }
    return "FAILED";
}

void Tester::generateSummaryReport(ReportModule *module)
{
    ReportTable summaryTable(2);
    ReportTable unitTestTables(9);

    // summary table
    summaryTable.setTitle("Summary");

    char dateBuff[80];
    strftime(dateBuff, sizeof(dateBuff), "%Y.%m.%d. %X", &sm_startTime);

    std::string executionTime = std::to_string((int)sm_executionTime) + "." + std::to_string((int)(sm_executionTime*100) % 100) + " seconds";
    std::string unitTests = std::to_string( sm_tests.size() );
    std::string failedUnitTests = std::to_string(sm_failedUnitTests);
    std::string unimplementedUnitTests = std::to_string(getUnimplementedUnitTests());
    std::string testFunctions = std::to_string(getTestFunctions());
    std::string failedTestFunctions = std::to_string(getFailedTestFunctions());
    std::string unimplementedTestFunctions = std::to_string(getUnimplementedTestFunctions());
    std::string asserts = std::to_string(getAsserts());
    std::string failedAsserts = std::to_string(getFailedAsserts());

    ReportTable::ALIGNMENT left = ReportTable::ALIGNMENT::LEFT;
    ReportTable::ALIGNMENT right = ReportTable::ALIGNMENT::RIGHT;
    ReportTable::ALIGNMENT center = ReportTable::ALIGNMENT::CENTER;

    summaryTable.setCell(0, 0, {"Date", left, nullptr, false });
    summaryTable.setCell(0, 1, {dateBuff, right, nullptr, false });

    summaryTable.setCell(1, 0, {"Execution time", left, nullptr, false });
    summaryTable.setCell(1, 1, {executionTime, right, nullptr, false });

    summaryTable.setCell(2, 0, {"Unit tests", left, nullptr, false });
    summaryTable.setCell(2, 1, {unitTests, right, nullptr, false });

    summaryTable.setCell(3, 0, {"Failed unit tests", left, nullptr, false });
    summaryTable.setCell(3, 1, {failedUnitTests, right, nullptr, false });

    summaryTable.setCell(4, 0, {"Unimplemented unit tests", left, nullptr, false });
    summaryTable.setCell(4, 1, {unimplementedUnitTests, right, nullptr, false });

    summaryTable.setCell(5, 0, {"Test functions", left, nullptr, false });
    summaryTable.setCell(5, 1, {testFunctions, right, nullptr, false });

    summaryTable.setCell(6, 0, {"Failed test functions", left, nullptr, false });
    summaryTable.setCell(6, 1, {failedTestFunctions, right, nullptr, false });

    summaryTable.setCell(7, 0, {"Unimplemented test functions", left, nullptr, false });
    summaryTable.setCell(7, 1, {unimplementedTestFunctions, right, nullptr, false });

    summaryTable.setCell(8, 0, {"Asserts", left, nullptr, false });
    summaryTable.setCell(8, 1, {asserts, right, nullptr, false });

    summaryTable.setCell(9, 0, {"Failed asserts", left, nullptr, false });
    summaryTable.setCell(9, 1, {failedAsserts, right, nullptr, false });

    summaryTable.setCell(10, 0, {"Status", left, nullptr, false });
    summaryTable.setCell(10, 1, {getStatusString(), right, nullptr, false });


    // unit test summary table
    unitTestTables.setTitle("Unit test summary");

    unitTestTables.setCell(0, 0, {"Name", center, nullptr, true});
    unitTestTables.setCell(0, 1, {"Execution time", center, nullptr, true});
    unitTestTables.setCell(0, 2, {"Status", center, nullptr, true});
    unitTestTables.setCell(0, 3, {"Test functions", center, nullptr, true});
    unitTestTables.setCell(0, 4, {"Failed test functions", center, nullptr, true});
    unitTestTables.setCell(0, 5, {"Unimplemented test functions", center, nullptr, true});
    unitTestTables.setCell(0, 6, {"Asserts", center, nullptr, true});
    unitTestTables.setCell(0, 7, {"Failed asserts", center, nullptr, true});
    unitTestTables.setCell(0, 8, {"Details", center, nullptr, true});

    unsigned int unitTestIndex = 1;
    for (auto & unitTest: sm_tests) {

        unsigned int time = (int)unitTest->getExecutionTime();
        std::string timeStr = std::to_string(time);
        time = (int)(unitTest->getExecutionTime() * 100) % 100;
        timeStr = timeStr + "." + std::to_string( time ) + " seconds";

        unitTestTables.setCell(unitTestIndex, 0, {unitTest->getName(), left, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 1, {timeStr, right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 2, {unitTest->getStatusString(), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 3, {std::to_string(unitTest->getTestFunctions()), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 4, {std::to_string(unitTest->getFailedTestFunctions()), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 5, {std::to_string(unitTest->getUnimplementedTestFunctions()), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 6, {std::to_string(unitTest->getAsserts()), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 7, {std::to_string(unitTest->getFailedAsserts()), right, nullptr, false});
        unitTestTables.setCell(unitTestIndex, 8, {"link", right, unitTest->getReportModule(), false});

        unitTestIndex++;
    }

    unitTestTables.setColumn(0, {true, ReportTable::TYPE::STRING});
    unitTestTables.setColumn(1, {true, ReportTable::TYPE::FLOAT});
    unitTestTables.setColumn(2, {true, ReportTable::TYPE::STRING});
    unitTestTables.setColumn(3, {true, ReportTable::TYPE::INTEGER});
    unitTestTables.setColumn(4, {true, ReportTable::TYPE::INTEGER});
    unitTestTables.setColumn(5, {true, ReportTable::TYPE::INTEGER});
    unitTestTables.setColumn(6, {true, ReportTable::TYPE::INTEGER});
    unitTestTables.setColumn(7, {true, ReportTable::TYPE::INTEGER});
    unitTestTables.setColumn(7, {false, ReportTable::TYPE::STRING});

    module->setName(sm_title);
    module->addTable(summaryTable);
    module->addTable(unitTestTables);
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
    std::string script;
    os << "<body>" << std::endl;

    os << "\t<h1>" << sm_title << "</h1>" << std::endl;

    generateHtmlSummary(os);
    generateUnitTestSummary(os, &script);

    os << script << std::endl;

    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

void Tester::generateHtmlSummary(std::ostream &os)
{
    os << "\t<h2>Summary </h2>" << std::endl;
    std::vector<std::vector<ReportCell>> summaryTable;

    char dateBuff[80];
    strftime(dateBuff, sizeof(dateBuff), "%Y.%m.%d. %X", &sm_startTime);

    std::string executionTime = std::to_string((int)sm_executionTime) + "." + std::to_string((int)(sm_executionTime*100) % 100) + " seconds";
    std::string unitTests = std::to_string( sm_tests.size() );
    std::string failedUnitTests = std::to_string(sm_failedUnitTests);
    std::string unimplementedUnitTests = std::to_string(getUnimplementedUnitTests());
    std::string testFunctions = std::to_string(getTestFunctions());
    std::string failedTestFunctions = std::to_string(getFailedTestFunctions());
    std::string unimplementedTestFunctions = std::to_string(getUnimplementedTestFunctions());
    std::string asserts = std::to_string(getAsserts());
    std::string failedAsserts = std::to_string(getFailedAsserts());
    std::string alignment = "align=\"right\"";

    unsigned int attrib = sm_simpleCell;

    summaryTable = { { {"Date", "", attrib}, { dateBuff, alignment, attrib} },
                     { {"Execution time", "", attrib}, { executionTime, alignment, attrib} },
                     { {"Unit tests", "", attrib}, { unitTests, alignment, attrib} },
                     { {"Failed unit tests", "", attrib}, { failedUnitTests, alignment, attrib} },
                     { {"Unimplemented unit tests", "", attrib}, { unimplementedUnitTests, alignment, attrib} },
                     { {"Test functions", "", attrib}, { testFunctions, alignment, attrib} },
                     { {"Failed test functions", "", attrib}, { failedTestFunctions, alignment, attrib} },
                     { {"Unimplemented test functions", "", attrib}, { unimplementedTestFunctions, alignment, attrib} },
                     { {"Asserts", "", attrib}, { asserts, alignment, attrib} },
                     { {"Failed asserts", "", attrib}, { failedAsserts, alignment, attrib} },
                     { {"Status", "", attrib}, { getStatusString(), alignment, attrib} }};

    generateTable(os, summaryTable);
}

void Tester::generateUnitTestSummary(std::ostream &os, std::string * script)
{
    os << "\t<h2>Unit test summary</h2>" << std::endl;

    std::vector<std::vector<ReportCell>> summaryTable;
    summaryTable.push_back(
    {{"Name", "", sm_sortCell | sm_headCell},
     {"Execution time", "", sm_sortCell | sm_headCell},
     {"Status", "", sm_sortCell | sm_headCell},
     {"Test functions", "", sm_sortCell | sm_headCell},
     {"Failed test functions", "", sm_sortCell | sm_headCell},
     {"Unimplemented test functions", "", sm_sortCell | sm_headCell},
     {"Asserts", "", sm_sortCell | sm_headCell},
     {"Failed asserts","", sm_sortCell | sm_headCell},
     {"Details", "", sm_headCell} } );
    unsigned int unitTestIndex = 0;
    for (auto & unitTest: sm_tests) {
        std::string link = std::string("<a href=\"unit_test_");
        link = link + std::to_string(unitTestIndex);
        link = link + ".html\"> link </a>";

        unsigned int time = (int)unitTest->getExecutionTime();
        std::string timeStr = std::to_string(time);
        time = (int)(unitTest->getExecutionTime() * 100) % 100;
        timeStr = timeStr + "." + std::to_string( time ) + " seconds";

        summaryTable.push_back(
        {{ unitTest->getName(), "", false},
         {timeStr, "align=\"right\"", false},
         {unitTest->getStatusString(), "align=\"right\"", false},
         {std::to_string(unitTest->getTestFunctions()), "align=\"right\"", false},
         {std::to_string(unitTest->getFailedTestFunctions()), "align=\"right\"", false},
         {std::to_string(unitTest->getUnimplementedTestFunctions()), "align=\"right\"", false},
         {std::to_string(unitTest->getAsserts()), "align=\"right\"", false},
         {std::to_string(unitTest->getFailedAsserts()), "align=\"right\"", false},
         {link, "align=\"right\"", false} } );

        unitTestIndex++;
    }
    generateTable(os, summaryTable, "unitTestSummaryTable");
    generateTableScript(summaryTable, "unitTestSummaryTable", script);
}

void Tester::generateTable(std::ostream &os,
                           const std::vector<std::vector<Tester::ReportCell> > &table,
                           const std::string & name)
{
    os << "\t<table border=\"1\"";
    if (name != "") {
        os << " id=\"" << name << "\"";
    }
    os << ">" << std::endl;
    for (auto & row: table) {
        os << "\t\t<tr>" << std::endl;
        for (auto & cell: row) {
            if (cell.m_httpSettings & sm_headCell) {
                os << "\t\t\t<th";
            } else {
                os << "\t\t\t<td";
            }
            if (cell.m_attributes.length() > 0) {
                os << " " << cell.m_attributes;
            }
            os << "> ";
            //<a onclick="unitTestSummarySort(compareByName, nameOrder); nameOrder = 1 - nameOrder;" href="#" >
            if (cell.m_httpSettings & sm_sortCell) {
                os << "<a onclick=\"";
                os << name << "Sort(compareBy";
                os << getCamelCase(cell.m_value) << ", ";
                os << getCamelCase(cell.m_value, true) << "Order); ";
                os << getCamelCase(cell.m_value, true) << "Order = 1 - ";
                os << getCamelCase(cell.m_value, true) << "Order;\" href=\"#\" > ";
            }
            os << cell.m_value << " </t";
            if (cell.m_httpSettings & sm_headCell) {
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

void Tester::generateTableScript(const std::vector<std::vector<Tester::ReportCell> > &table,
                                 const std::string & tableName,
                                 std::string *script)
{
    std::ostringstream stream;
    stream << "\t<script>" << std::endl;

    // get column names
    std::vector<std::string> columnNames;
    for (auto & name: table[0]) {
        if (name.m_httpSettings & sm_sortCell) {
            columnNames.push_back( name.m_value );
        }
    }

    stream << std::endl;
    // generate order variables
    for (auto & name: columnNames) {
        stream << "\t\tvar " << getCamelCase(name, true) << "Order = 0;" << std::endl;
    }

    // generate compare functions
    for (auto & name: columnNames) {
        stream << std::endl;
        stream << "\t\tfunction compareBy" << getCamelCase(name) << "(a, b) {" << std::endl;
        stream << "\t\t\tvar x = a." << getCamelCase(name, true) << ";" << std::endl;
        stream << "\t\t\tvar y = b." << getCamelCase(name, true) << ";" << std::endl;
        stream << "\t\t\treturn (x < y) ? -1 : ((x > y) ? 1 : 0);" << std::endl;
        stream << "\t\t}" << std::endl;
    }

    // generate sorter function
    stream << std::endl;
    stream << "\t\tfunction " << tableName << "Sort(compare, order) {" << std::endl;
    stream << "\t\t\tvar table = document.getElementById(\'" << tableName << "\');" << std::endl;
    stream << std::endl;
    stream << "\t\t\tvar rowIndex;" << std::endl;
    stream << "\t\t\tvar content = [];" << std::endl;
    stream << "\t\t\tfor (rowIndex = 1; rowIndex < table.rows.length; rowIndex++) {" << std::endl;
    stream << "\t\t\t\tvar cells = table.rows[rowIndex].cells;" << std::endl;
    stream << std::endl;
    stream << "\t\t\t\tvar rowContent = {};" << std::endl;
    unsigned int index = 0;
    for (auto & name: table[0]) {
        stream << "\t\t\trowContent." << getCamelCase(name.m_value, true) << " = cells[" << index << "].innerHTML;" << std::endl;
        index++;
    }
    stream << "\t\t\tcontent[content.length] = rowContent;" << std::endl;
    stream << "\t\t}" << std::endl;
    stream << std::endl;


    stream << "\t\tcontent.sort(compare);" << std::endl;
    stream << "\t\tvar start;" << std::endl;
    stream << "\t\tvar end;" << std::endl;
    stream << "\t\tvar step;" << std::endl;
    stream << "\t\tif (order < 1) {" << std::endl;
    stream << "\t\t\tstart = 0;" << std::endl;
    stream << "\t\t\tstep = 1;" << std::endl;
    stream << "\t\t} else {" << std::endl;
    stream << "\t\t\tstart = content.length - 1;" << std::endl;
    stream << "\t\t\tstep = -1;" << std::endl;
    stream << "\t\t}" << std::endl;
    stream << "\t\tvar contentIndex = start;" << std::endl;
    stream << "\t\tfor (rowIndex = 1; rowIndex < table.rows.length; rowIndex++, contentIndex += step) {" << std::endl;
    stream << "\t\t\tvar cells = table.rows[rowIndex].cells;" << std::endl;

    index = 0;
    for (auto & name: table[0]) {
        stream << "\t\t\tcells[" << index << "].innerHTML = content[contentIndex]." << getCamelCase(name.m_value, true) << ";" << std::endl;
        index++;
    }
    stream << "\t\t}" << std::endl;
    stream << "\t}" << std::endl;
    stream << "\t</script>" << std::endl;
    *script = stream.str();
}

std::string Tester::getCamelCase(const std::string &value, bool lower)
{
    std::ostringstream stream;
    for (auto ch: value) {
        if (ch == ' ') {
            lower = false;
            continue;
        }
        if (lower) {
            stream << (char)(ch | 32);
        } else {
            stream << (char)(ch & (255 - 32));
        }
        lower = true;
    }
    return stream.str();
}

class DemoTestCase : public UnitTest
{
public:

    DemoTestCase(const char * name) : UnitTest(name)
    {
    }

    unsigned int run()
    {
        test1();
        test2();
        return 0;
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

    unsigned int run()
    {
        foo();
        bar();
        return 0;
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
