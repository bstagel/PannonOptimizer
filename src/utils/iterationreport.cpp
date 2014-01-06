#include <utils/iterationreport.h>
#include <utils/iterationreportprovider.h>
#include <debug.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <simplex/simplexparameterhandler.h>

IterationReport::IterationReport() {
    m_refreshHeader = true;
    m_debugLevel = 0;
    m_lastOutputEvent = IR_NONE;
}

IterationReport::IterationReport(const IterationReport & orig) {
    copy(orig);
}

IterationReport & IterationReport::operator=(const IterationReport & orig) {
    if (this == &orig) {
        return *this;
    }
    clear();
    copy(orig);
    return *this;
}

IterationReport::~IterationReport() {
    clear();
}

void IterationReport::copy(const IterationReport & orig) {
    m_startFields = orig.m_startFields;
    m_iterationFields = orig.m_iterationFields;
    m_solutionFields = orig.m_solutionFields;
    m_iterationTable = orig.m_iterationTable;
    m_startTable = orig.m_startTable;
    m_solutionTable = orig.m_solutionTable;
    m_refreshHeader = orig.m_refreshHeader;
    m_debugLevel = orig.m_debugLevel;
    m_lastOutputEvent = orig.m_lastOutputEvent;

    unsigned int columnIndex = 0;
    unsigned int index = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_iterationFields, iterIter) {
        if (iterIter->getType() == IterationReportField::IRF_STRING) {
            for (index = 0; index < m_iterationTable.size(); index++) {
                m_iterationTable[index][columnIndex].m_string =
                        new std::string(*m_iterationTable[index][columnIndex].m_string);
            }
        }
        columnIndex++;
    }

    columnIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_startFields, startIter) {
        if (startIter->getType() == IterationReportField::IRF_STRING) {
            m_startTable[index].m_string = new std::string(*m_startTable[index].m_string);
        }
        columnIndex++;
    }

    columnIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_solutionFields, solutionIter) {
        if (solutionIter->getType() == IterationReportField::IRF_STRING) {
            m_solutionTable[index].m_string = new std::string(*m_solutionTable[index].m_string);
        }
        columnIndex++;
    }
}

void IterationReport::clear() {
    unsigned int columnIndex = 0;
    unsigned int index = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_iterationFields, iterIter) {
        if (iterIter->getType() == IterationReportField::IRF_STRING) {
            for (index = 0; index < m_iterationTable.size(); index++) {
                delete m_iterationTable[index][columnIndex].m_string;
            }
        }
        columnIndex++;
    }

    /*columnIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_startFields, startIter) {
        if (startIter->getType() == IterationReportField::IRF_STRING) {
            delete m_startTable[columnIndex].m_string;
        }
        columnIndex++;
    }

    columnIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_solutionFields, solutionIter) {
        if (solutionIter->getType() == IterationReportField::IRF_STRING) {
            delete m_solutionTable[columnIndex].m_string;
        }
        columnIndex++;
    }*/
}

void IterationReport::setDebugLevel(int level) {
    if (m_debugLevel == level) {
        return;
    }
    m_debugLevel = level;
    if (m_lastOutputEvent != IR_NONE) {
        showHeader();
    }
}

int IterationReport::getDebugLevel() const {
    return m_debugLevel;
}

void IterationReport::addFields( const IterationReportProvider & provider,
                                 std::vector<IterationReportField> * fields,
                                 enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) {
    std::vector<IterationReportField> resultFields =
            provider.getIterationReportFields(type);

    STL_FOREACH(std::vector<IterationReportField>, resultFields, iter) {
        fields->push_back(*iter);
    }
}

void IterationReport::addProviderForIteration(
        const IterationReportProvider & provider) {
    addFields(provider, &m_iterationFields, IterationReportProvider::IRF_ITERATION);
}

void IterationReport::addProviderForStart(
        const IterationReportProvider & provider) {
    addFields(provider, &m_startFields, IterationReportProvider::IRF_START);
}

void IterationReport::addProviderForSolution(
        const IterationReportProvider & provider) {
    addFields(provider, &m_solutionFields, IterationReportProvider::IRF_SOLUTION);
}

void IterationReport::addProviderForExport(const IterationReportProvider & provider) {
    addFields(provider, &m_exportFields, IterationReportProvider::IRF_EXPORT);
}

void IterationReport::getRow(const std::vector<IterationReportField> & fields,
                             std::vector< Entry > * row,
                             enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const {
    row->clear();
    STL_FOREACH(std::vector<IterationReportField>,
                fields, fieldIter) {
        IterationReportField field = *fieldIter;
        Entry newEntry;
        std::string name = field.getName();
        newEntry = field.getProvider().getIterationEntry(name, type);

        row->push_back(newEntry);
    }
}

std::string IterationReport::getContent(const Entry & entry, const IterationReportField & field) const {
    std::ostringstream entryString;
    switch ( field.getType() ) {
    case IterationReportField::IRF_BOOL:
        entryString << (entry.m_bool == true ? "true" : "false");
        break;
    case IterationReportField::IRF_FLOAT:
        switch ( field.getFloatFormat() ) {
        case IterationReportField::IRF_FIXED:
            entryString << std::fixed;
            break;
        case IterationReportField::IRF_SCIENTIFIC:
        case IterationReportField::IRF_NONE:
            entryString << std::scientific;
            break;
        }
        if (field.getPrecision() >= 0) {
            entryString << std::setprecision( field.getPrecision() );
        } else {
            entryString.unsetf( std::ios::floatfield );
        }
        entryString << entry.m_double;
        break;
    case IterationReportField::IRF_INT:
        entryString << entry.m_integer;
        break;
    case IterationReportField::IRF_STRING:
        entryString << *entry.m_string;
        break;
    }
    return entryString.str();
}

void IterationReport::writeSimpleTable(const std::vector<IterationReportField> & fields,
                                       const std::vector< Entry > & row,
                                       IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) const {
    if (type == IterationReportProvider::IRF_START) {
        LPINFO("***** STARTING REPORT *****");
    } else if (type == IterationReportProvider::IRF_SOLUTION) {
        LPINFO("***** SOLUTION REPORT *****");
    }
    unsigned int entryIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, fields, fieldsIter) {
        LPINFO(fieldsIter->getName() << ": " << getContent(row[entryIndex], *fieldsIter));
        entryIndex++;
    }
}

void IterationReport::writeExportTable(const std::vector<IterationReportField> &fields,
                                       const std::vector<Entry> &row,
                                       const std::string filename) const {
    std::ofstream exportfile;
    exportfile.open(filename, std::ios::out | std::ios::app);
    if(exportfile.is_open()){
        unsigned int entryIndex = 0;
        STL_FOREACH(std::vector<IterationReportField>, fields, fieldsIter) {
            exportfile << getContent(row[entryIndex], *fieldsIter);
            if (entryIndex < fields.size()-1){
                exportfile << " , ";
            } else {
                exportfile << "\n";
            }
            entryIndex++;
        }
    } else {
        throw ParameterException("Export file cannot be opened!");
    }
}

void IterationReport::createStartReport() {
    getRow(m_startFields, &m_startTable, IterationReportProvider::IRF_START);
}

void IterationReport::writeStartReport() const {
    writeSimpleTable(m_startFields, m_startTable, IterationReportProvider::IRF_START);
}

void IterationReport::createIterationReport() {
    std::vector<Entry> newRow;
    getRow(m_iterationFields, &newRow, IterationReportProvider::IRF_ITERATION);
    m_iterationTable.push_back(newRow);
}

void IterationReport::showHeader() {
    m_lastOutputEvent = IR_HEADER;
    std::ostringstream headerString;
    unsigned int index = 0;
    STL_FOREACH(std::vector<IterationReportField>, m_iterationFields, fieldsIter) {
        if(m_debugLevel  < fieldsIter->getDebugLevel() ){
            continue;
        }
        // TODO: ezt az igazitosdit majd kiszedni fuggvenyekbe, baromi hulyen
        // jon ki, hogy kicsit lejjebb is kb ugyanez a kod van
        unsigned int width = fieldsIter->getName().length();
        if (width < fieldsIter->getWidth()) {
            unsigned int spaceCount = fieldsIter->getWidth() - width;

            switch ( fieldsIter->getAlignment() ) {
            case IterationReportField::IRF_CENTER:
                headerString << std::string(spaceCount / 2, ' ') << fieldsIter->getName() << std::string(spaceCount / 2 + spaceCount % 2, ' ');
                break;
            case IterationReportField::IRF_LEFT:
                headerString << fieldsIter->getName() << std::string(spaceCount, ' ');
                break;
            case IterationReportField::IRF_RIGHT:
                headerString << std::string(spaceCount, ' ') << fieldsIter->getName();
                break;
            }

        } else {
            headerString << fieldsIter->getName();
        }
        if (index < m_iterationFields.size() - 1) {
            headerString << "|";
        }
        index++;
    }
    LPINFO(headerString.str());
}

void IterationReport::writeIterationReport() {
    if (m_iterationTable.size() == 0) {
        return;
    }

    if (m_iterationTable.size() == 1) {
        showHeader();
    }

    std::ostringstream row;
    const std::vector<Entry> & lastRow = m_iterationTable[ m_iterationTable.size() - 1 ];

    unsigned int index;
    unsigned int fieldCounter = 0;
    for (index = 0; index < lastRow.size(); index++) {
        std::ostringstream entryString;
        IterationReportField field = m_iterationFields[index];
        if (m_debugLevel < field.getDebugLevel()) {
            continue;
        }
        fieldCounter++;
        const Entry & newEntry = lastRow[index];

        entryString << getContent(newEntry, field);

        unsigned int width = entryString.str().length();
        if (width > field.getWidth()) {
            std::string str = entryString.str().substr(0, field.getWidth() - 1);
            row << str << "*";
        } else if (width < field.getWidth()) {
            unsigned int spaceCount = field.getWidth() - width;

            switch ( field.getAlignment() ) {
            case IterationReportField::IRF_CENTER:
                row << std::string(spaceCount / 2, ' ') << entryString.str() << std::string(spaceCount / 2 + spaceCount % 2, ' ');
                break;
            case IterationReportField::IRF_LEFT:
                row << entryString.str() << std::string(spaceCount, ' ');
                break;
            case IterationReportField::IRF_RIGHT:
                row << std::string(spaceCount, ' ') << entryString.str();
                break;
            }

        } else {
            row << entryString.str();
        }
        if (index < lastRow.size() - 1) {
            row << "|";
        }
    }
    if (fieldCounter > 0) {
        LPINFO(row.str());
    }
    m_lastOutputEvent = IR_ROW;
}

void IterationReport::createSolutionReport() {
    getRow(m_solutionFields, &m_solutionTable, IterationReportProvider::IRF_SOLUTION);
}

void IterationReport::writeSolutionReport() const {
    writeSimpleTable(m_solutionFields, m_solutionTable, IterationReportProvider::IRF_SOLUTION);
}

void IterationReport::createExportReport() {
    getRow(m_exportFields, &m_exportTable, IterationReportProvider::IRF_EXPORT);
}

void IterationReport::writeExportReport(std::string filename) const {
    writeExportTable(m_exportFields, m_exportTable, filename);
}
