#ifndef ITERATIONREPORT_H
#define ITERATIONREPORT_H

#include <globals.h>
#include <string>
#include <vector>
#include <utils/iterationreportfield.h>
#include <utils/iterationreportprovider.h>
#include <utils/entry.h>

class IterationReport {

public:

    IterationReport();

    IterationReport(const IterationReport & orig);

    IterationReport & operator=(const IterationReport & orig);

    virtual ~IterationReport();

    void removeIterationProvider(const IterationReportProvider & provider);

    void addProviderForIteration(const IterationReportProvider & provider);

    void addProviderForStart(const IterationReportProvider & provider);

    void addProviderForSolution(const IterationReportProvider & provider);

    void addProviderForExport(const IterationReportProvider & provider);

    void createStartReport();

    void writeStartReport() const;

    void createIterationReport();

    void writeIterationReport();

    void createSolutionReport();

    void writeSolutionReport() const;

    void createExportReport();

    void writeExportReport(std::string filename) const;

    int getDebugLevel() const;

private:

    enum LAST_OUTPUT_EVENT {
        IR_NONE,
        IR_HEADER,
        IR_ROW
    };

    bool m_providerChanged;

    LAST_OUTPUT_EVENT m_lastOutputEvent;

    std::vector<IterationReportField> m_startFields;

    std::vector<IterationReportField> m_iterationFields;

    std::vector<IterationReportField> m_solutionFields;

    std::vector<IterationReportField> m_exportFields;

//    std::vector< std::vector< Entry > > m_iterationTable;

    std::vector< Entry > m_startTable;

    std::vector< Entry > m_solutionTable;

    std::vector< Entry > m_exportTable;

    bool m_refreshHeader;

    const int & m_debugLevel;
    int m_lastDebugLevel;

    void showHeader();

    void addFields(const IterationReportProvider & provider,
                    std::vector<IterationReportField> * fields,
                    enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type);

    void getRow(const std::vector<IterationReportField> & fields,
                std::vector< Entry > * row,
                IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    void writeSimpleTable(const std::vector<IterationReportField> & fields,
                          const std::vector< Entry > & row,
                          IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    void writeExportTable(const std::vector<IterationReportField> & fields,
                          const std::vector< Entry > & row,
                          const std::string filename) const;

    std::string getContent(const Entry & entry, const IterationReportField & field) const;

    void copy(const IterationReport & orig);

    void clear();
};

#endif // ITERATIONREPORT_H
