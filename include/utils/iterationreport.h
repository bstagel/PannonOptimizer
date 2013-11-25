#ifndef ITERATIONREPORT_H
#define ITERATIONREPORT_H

#include <globals.h>
#include <string>
#include <vector>
#include <utils/iterationreportfield.h>
#include <utils/iterationreportprovider.h>
#include <utils/reportentry.h>

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

    std::vector<IterationReportField> m_startFields;

    std::vector<IterationReportField> m_iterationFields;

    std::vector<IterationReportField> m_solutionFields;

    std::vector< std::vector< ReportEntry > > m_iterationTable;

    std::vector< ReportEntry > m_startTable;

    std::vector< ReportEntry > m_solutionTable;

    void addFields(const IterationReportProvider & provider,
                    std::vector<IterationReportField> * fields,
                    enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type);

    void getRow(const std::vector<IterationReportField> & fields,
                std::vector< ReportEntry > * row,
                IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    void writeSimpleTable(const std::vector<IterationReportField> & fields,
                          const std::vector< ReportEntry > & row,
                          IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    std::string getContent(const ReportEntry & reportEntry, const IterationReportField & field) const;

    void copy(const IterationReport & orig);

    void clear();
};

#endif // ITERATIONREPORT_H
