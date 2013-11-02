#ifndef ITERATIONREPORT_H
#define ITERATIONREPORT_H

#include <globals.h>
#include <string>
#include <vector>
#include <utils/iterationreportfield.h>
#include <utils/iterationreportprovider.h>

class IterationReport {

public:

    IterationReport();

    IterationReport(const IterationReport & orig);

    IterationReport & operator=(const IterationReport & orig);

    virtual ~IterationReport();

    void addProviderForIteration(const IterationReportProvider & provider);

    void addProviderForStart(const IterationReportProvider & provider);

    void addProviderForSolution(const IterationReportProvider & provider);

    void createStartReport();

    void writeStartReport() const;

    void createIterationReport();

    void writeIterationReport() const;

    void createSolutionReport();

    void writeSolutionReport() const;

private:

    union Entry {
        std::string * m_string;
        int m_integer;
        double m_float;
        bool m_bool;
    };

    std::vector<IterationReportField> m_startFields;

    std::vector<IterationReportField> m_iterationFields;

    std::vector<IterationReportField> m_solutionFields;

    std::vector< std::vector< Entry > > m_iterationTable;

    std::vector< Entry > m_startTable;

    std::vector< Entry > m_solutionTable;

    void addFields(const IterationReportProvider & provider,
                    std::vector<IterationReportField> * fields,
                    enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type);

    void getRow(const std::vector<IterationReportField> & fields,
                std::vector< Entry > * row,
                IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    void writeSimpleTable(const std::vector<IterationReportField> & fields,
                          const std::vector< Entry > & row,
                          IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    std::string getContent(const Entry & entry, const IterationReportField & field) const;

    void copy(const IterationReport & orig);

    void clear();
};

#endif // ITERATIONREPORT_H
