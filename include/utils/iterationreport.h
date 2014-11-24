/**
 * @file iterationreport.h This file contains the API of the IterationReport class.
 * @author Jozsef Smidla
 */

#ifndef ITERATIONREPORT_H
#define ITERATIONREPORT_H

#include <globals.h>
#include <string>
#include <sstream>
#include <vector>
#include <utils/iterationreportfield.h>
#include <utils/iterationreportprovider.h>
#include <utils/entry.h>

/**
 * This class describes the report providing information about the solver.
 * This consists of information about the algorithm, the problem, the presolving,
 * information about the simplex iterations during the solution method, and
 * information about the found solution.
 *
 * @class IterationReport
 */
class IterationReport {

public:

    /**
     * Default constructor of the IterationReport class.
     *
     * @constructor
     */
    IterationReport();

    /**
     * Copy constructor of the IterationReport class.
     *
     * @constructor
     * @param orig The IterationReport object to be copied.
     */
    IterationReport(const IterationReport & orig);

    /**
     * Assignment operator of the IterationReport class.
     *
     * @param orig The other IterationReport object.
     * @return Reference to the copied IterationReport object.
     */
    IterationReport & operator=(const IterationReport & orig);

    /**
     * Destructor of the IterationReport class.
     *
     * @destructor
     */
    virtual ~IterationReport();

    /**
     * Removes an iteration report provider from the report.
     * Iteration report providers are the source of information.
     *
     * @see IterationReportProvider
     * @param provider The iteration report provider to be added.
     */
    void removeIterationProvider(const IterationReportProvider & provider);

    /**
     * Adds an iteration report provider for the simplex iteration process.
     * Iteration report providers are the source of information.
     *
     * @see IterationReportProvider
     * @param provider The IterationReportProvider to be added.
     */
    void addProviderForIteration(const IterationReportProvider & provider);

    /**
     * Adds an iteration report provider for the start of the solver.
     * Iteration report providers are the source of information.
     *
     * @see IterationReportProvider
     * @param provider The IterationReportProvider to be added.
     */
    void addProviderForStart(const IterationReportProvider & provider);

    /**
     * Adds an iteration report provider for the solution description.
     * Iteration report providers are the source of information.
     *
     * @see IterationReportProvider
     * @param provider The IterationReportProvider to be added.
     */
    void addProviderForSolution(const IterationReportProvider & provider);

    /**
     * Adds an iteration report provider for the exported file.
     * Iteration report providers are the source of information.
     *
     * @see IterationReportProvider
     * @param provider The IterationReportProvider to be added.
     */
    void addProviderForExport(const IterationReportProvider & provider);

    /**
     * Queries the iteration report for the start of the solver.
     * The report entries are collected in the iteration report to be printed out later.
     */
    void createStartReport();

    /**
     * Prints the iteration report for the start of the solver to the output.
     */
    void writeStartReport() const;

    /**
     * Prints the iteration report for the solver iterations to the output.
     */
    void writeIterationReport();

    /**
     * Queries the iteration report for the solution details.
     * The report entries are collected in the iteration report to be printed out later.
     */
    void createSolutionReport();

    /**
     * Prints the iteration report for the solution details to the output.
     */
    void writeSolutionReport() const;

    /**
     * Queries the iteration report for the exported file.
     * The report entries are collected in the iteration report to be printed out later.
     */
    void createExportReport();

    /**
     * Prints the iteration report for the exported file to the specified file.
     *
     * @param filename The file to export the report to.
     */
    void writeExportReport(std::string filename) const;
    /**
     * Returns the debug level of the report.
     *
     * @see m_debugLevel
     * @return The debug level of the report.
     */
    int getDebugLevel() const;

    /**
     * Writes the actual batch from the output stream to the standard output.
     */
    void flushBatch() const;

private:

    /**
     * This type describes an event type printed to the output.
     */
    enum LAST_OUTPUT_EVENT {
        IR_NONE,
        IR_HEADER,
        IR_ROW
    };

    /**
     * This is true the iteration report providers assigned to this iteration report have been changed since initialization.
     */
    bool m_providerChanged;

    /**
     * The type of the last output event in this iteration report.
     */
    LAST_OUTPUT_EVENT m_lastOutputEvent;

    /**
     * The fields of the iteration report for the start of the solver.
     */
    std::vector<IterationReportField> m_startFields;

    /**
     * The fields of the iteration report for the iterations of the solver.
     */
    std::vector<IterationReportField> m_iterationFields;

    /**
     * The fields of the iteration report for the solution details.
     */
    std::vector<IterationReportField> m_solutionFields;

    /**
     * The fields of the iteration report for the exported file.
     */
    std::vector<IterationReportField> m_exportFields;

    /**
     * The table with the entries for the start of the solver to be printed.
     */
    std::vector< Entry > m_startTable;

    /**
     * The table with the entries for the solution details to be printed.
     */
    std::vector< Entry > m_solutionTable;

    /**
     * The table with the entries for the export file to be printed.
     */
    std::vector< Entry > m_exportTable;

    /**
     *
     */
    bool m_refreshHeader;

    /**
     * The debug level of the report.
     */
    const int & m_debugLevel;

    /**
     * The last debug level of the report.
     * If this is not equal to the current debug level, the next header needs to be printed.
     */
    int m_lastDebugLevel;

    /**
     * A flag indicating the parallel mode.
     */
    const bool & m_parallelMode;

    /**
     * Provides a batch output mode.
     */
    const bool & m_batchMode;

    /**
     * Provides the batch sizes of the output.
     * Zero means that there is only one batch.
     */
    const int & m_batchSize;

    /**
     * The size of the actual batch.
     */
    mutable int m_actualBatch;

    /**
     * A mutex to lock the output.
     */
    mutable std::mutex m_outputMutex;

    /**
     * The string stream of one output batch.
     */
    mutable std::ostringstream m_outputStream;

    /**
     * Prints out the header describing the name of the fields in the rows under it.
     */
    void showHeader();

    /**
     * Adds fields to the report from the specified provider with the specified type.
     *
     * @param provider The provider of the report fields.
     * @param fields The field vector the new fields to be added to.
     * @param type The type of the added report fields.
     */
    void addFields(const IterationReportProvider & provider,
                    std::vector<IterationReportField> * fields,
                    enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type);

    /**
     * Sets up a row of entries to be printed.
     *
     * @param fields The fields to be set up in the row.
     * @param row The row of entries to build.
     * @param type The type of the report row.
     */
    void getRow(const std::vector<IterationReportField> & fields,
                std::vector< Entry > * row,
                IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    /**
     * Sets up a report table of the specified type to be printed.
     *
     * @param fields The fields of the table.
     * @param row The content data of the table.
     * @param type The type of the report table.
     */
    void writeSimpleTable(const std::vector<IterationReportField> & fields,
                          const std::vector< Entry > & row,
                          IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const;

    /**
     * Sets up a report table of the specified type to be exported
     *
     * @param fields The fields of the table.
     * @param row The content data of the table.
     * @param filename The name of the file the table is exported to.
     */
    void writeExportTable(const std::vector<IterationReportField> & fields,
                          const std::vector< Entry > & row,
                          const std::string filename) const;

    /**
     * Returns the content of a specified entry in the format given in the report field.
     *
     * @param entry The entry with the content.
     * @param field The iteration report field describing the type of data in the entry.
     * @return The formatted string with the content of the entry.
     */
    std::string getContent(const Entry & entry, const IterationReportField & field) const;

    /**
     * Copies another iteration report.
     * Helper function of the copy contructor and assignment operator.
     *
     * @param orig The other iteration report object to be copied.
     */
    void copy(const IterationReport & orig);

    /**
     * Clears the iteration report.
     */
    void clear();
};

#endif // ITERATIONREPORT_H
