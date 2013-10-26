#ifndef ITERATIONREPORTPROVIDER_H
#define ITERATIONREPORTPROVIDER_H

#include <globals.h>
#include <utils/iterationreportfield.h>
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

    virtual std::string getIterationReportString(
            const std::string & name,
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

    virtual int getIterationReportInteger(
            const std::string & name,
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

    virtual double getIterationReportFloat(
            const std::string & name,
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

    virtual bool getIterationReportBool(
            const std::string & name,
            enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

};

#endif // ITERATIONREPORTPROVIDER_H
