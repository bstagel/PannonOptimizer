#ifndef ITERATIONREPORTPROVIDER_H
#define ITERATIONREPORTPROVIDER_H

#include <globals.h>
#include <utils/iterationreportfield.h>
#include <utils/entry.h>
#include <vector>
#include <string>

class IterationReportProvider {
public:

    enum ITERATION_REPORT_FIELD_TYPE {
        IRF_START,
        IRF_ITERATION,
        IRF_SOLUTION
    };

    virtual std::vector<IterationReportField> getIterationReportFields(
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

    virtual Entry getIterationEntry(
            const std::string & name,
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

};

#endif // ITERATIONREPORTPROVIDER_H
