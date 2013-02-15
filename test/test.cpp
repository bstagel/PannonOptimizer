#include <cstdlib>
#include <cstring>
#include <iostream>

#ifdef _MSC_VER
#pragma warning (disable: 4290)
#endif

#include <cpptest.h>
#include "variabletest.h"
#include "constrainttest.h"
#include "matrixtest.h"
#include "manualmodelbuildertest.h"

using namespace std;

// Tests throw asserts
//

enum OutputType
{
    Compiler,
    Html,
    TextTerse,
    TextVerbose
};

static void
usage()
{
    cout << "usage: mytest [MODE]\n"
        << "where MODE may be one of:\n"
        << "  --compiler\n"
        << "  --html\n"
        << "  --text-terse (default)\n"
        << "  --text-verbose\n";
    exit(0);
}

static auto_ptr<Test::Output>
cmdline(int argc, char* argv[])
{
    if (argc > 2)
        usage(); // will not return

    Test::Output* output = 0;

    if (argc == 1)
        output = new Test::TextOutput(Test::TextOutput::Verbose);
    else {
        const char* arg = argv[1];
        if (strcmp(arg, "--compiler") == 0)
            output = new Test::CompilerOutput;
        else if (strcmp(arg, "--html") == 0)
            output = new Test::HtmlOutput;
        else if (strcmp(arg, "--text-terse") == 0)
            output = new Test::TextOutput(Test::TextOutput::Terse);
        else if (strcmp(arg, "--text-verbose") == 0)
            output = new Test::TextOutput(Test::TextOutput::Verbose);
        else {
            cout << "invalid commandline argument: " << arg << endl;
            usage(); // will not return
        }
    }

    return auto_ptr<Test::Output>(output);
}

// Main test program
//

int
main(int argc, char* argv[])
{
    try {
        // Demonstrates the ability to use multiple test suites
        //
        Test::Suite ts;
        ts.add(auto_ptr<Test::Suite>(new VariableTestSuite));
        ts.add(auto_ptr<Test::Suite>(new ConstraintTestSuite));
        ts.add(auto_ptr<Test::Suite>(new MatrixTestSuite));
        ts.add(auto_ptr<Test::Suite>(new ManualModelBuilderTestSuite));

        // Run the tests
        //
        auto_ptr<Test::Output> output(cmdline(argc, argv));
        LPINFO("**********************************************");
        LPINFO("**********************************************");
        ts.run(*output, true);
        LPINFO("**********************************************");
        LPINFO("**********************************************");

        Test::HtmlOutput* const html = dynamic_cast<Test::HtmlOutput*>(output.get());
        if (html)
            html->generate(cout, true, "Pannon Optimizer");

    } catch (...) {
        cout << "unexpected exception encountered\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

