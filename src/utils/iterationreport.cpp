//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#include <utils/iterationreport.h>
#include <utils/iterationreportprovider.h>
#include <debug.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <simplex/simplexparameterhandler.h>

IterationReport::IterationReport():
    m_providerChanged(false),
    m_refreshHeader(true),
    m_debugLevel(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.debug_level")),
    m_lastDebugLevel(m_debugLevel),
    m_parallelMode(SimplexParameterHandler::getInstance().getBoolParameterValue("Parallel.enable_parallelization")),
    m_batchMode(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.batch_output")),
    m_batchSize(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.batch_size")),
    m_actualBatch(0)
{
    m_lastOutputEvent = IR_NONE;
}

IterationReport::IterationReport(const IterationReport & orig):
    m_debugLevel(orig.m_debugLevel),
    m_parallelMode(SimplexParameterHandler::getInstance().getBoolParameterValue("Parallel.enable_parallelization")),
    m_batchMode(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.batch_output")),
    m_batchSize(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.batch_size"))
{
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
    m_providerChanged = orig.m_providerChanged;
    m_startFields = orig.m_startFields;
    m_iterationFields = orig.m_iterationFields;
    m_solutionFields = orig.m_solutionFields;
    m_startTable = orig.m_startTable;
    m_solutionTable = orig.m_solutionTable;
    m_refreshHeader = orig.m_refreshHeader;
    m_lastOutputEvent = orig.m_lastOutputEvent;

    unsigned int columnIndex = 0;
    unsigned int index = 0;

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
    int columnIndex = 0;
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
    }
}

int IterationReport::getDebugLevel() const {
    return m_debugLevel;
}

void IterationReport::addFields( const IterationReportProvider & provider,
                                 std::vector<IterationReportField> * fields,
                                 enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type) {
    std::vector<IterationReportField> resultFields = provider.getIterationReportFields(type);

    STL_FOREACH(std::vector<IterationReportField>, resultFields, iter) {
        fields->push_back(*iter);
    }
}

void IterationReport::removeIterationProvider(const IterationReportProvider &provider)
{
    m_providerChanged = true;

    std::vector<IterationReportField> newFields;

    STL_FOREACH(std::vector<IterationReportField>, m_iterationFields, iter) {
        if (&iter->getProvider() != &provider) {
            newFields.push_back(*iter);
        }
    }
    m_iterationFields = newFields;

}

void IterationReport::addProviderForIteration(
        const IterationReportProvider & provider) {
    m_providerChanged = true;
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
        LPREPORT("***** STARTING SOLUTION *****", m_outputStream);
    } else if (type == IterationReportProvider::IRF_SOLUTION) {
        LPREPORT("***** SOLUTION REPORT *****", m_outputStream);
    }
    unsigned int entryIndex = 0;
    STL_FOREACH(std::vector<IterationReportField>, fields, fieldsIter) {
        LPREPORT(fieldsIter->getName() << ": " << getContent(row[entryIndex], *fieldsIter), m_outputStream);
        m_actualBatch++;
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

    if(!m_batchMode || m_actualBatch > m_batchSize){
        flushBatch(); //Flush if necessary
    }
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

    LPREPORT(headerString.str(), m_outputStream);
    m_actualBatch++;
}

void IterationReport::writeIterationReport() {
    std::vector<Entry> newRow;
    getRow(m_iterationFields, &newRow, IterationReportProvider::IRF_ITERATION);

    if (m_lastDebugLevel != m_debugLevel || m_providerChanged) {
        showHeader();
        m_lastDebugLevel = m_debugLevel;
        m_providerChanged = false;
    }

    std::ostringstream row;
    //const std::vector<Entry> & lastRow = m_iterationTable[ m_iterationTable.size() - 1 ];

    unsigned int index;
    unsigned int fieldCounter = 0;
    for (index = 0; index < newRow.size(); index++) {
        std::ostringstream entryString;
        IterationReportField field = m_iterationFields[index];
        if (m_debugLevel < field.getDebugLevel()) {
            continue;
        }
        fieldCounter++;
        const Entry & newEntry = newRow[index];

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
        if (index < newRow.size() - 1) {
            row << "|";
        }
        //Free the string entries
        if (field.getType() == IterationReportField::IRF_STRING){
            delete newRow[index].m_string;
        }
    }
    if (fieldCounter > 0) {
        LPREPORT(row.str(),m_outputStream);
        m_actualBatch++;
    }
    m_lastOutputEvent = IR_ROW;

    if(!m_batchMode || m_actualBatch > m_batchSize){
        flushBatch(); //Flush if necessary
    }
}

void IterationReport::createSolutionReport() {
    getRow(m_solutionFields, &m_solutionTable, IterationReportProvider::IRF_SOLUTION);
}

void IterationReport::writeSolutionReport() const {

    writeSimpleTable(m_solutionFields, m_solutionTable, IterationReportProvider::IRF_SOLUTION);

    flushBatch(); //Always flush after the solution report
}

void IterationReport::createExportReport() {
    getRow(m_exportFields, &m_exportTable, IterationReportProvider::IRF_EXPORT);
}

void IterationReport::writeExportReport(std::string filename) const {
    writeExportTable(m_exportFields, m_exportTable, filename);
}

void IterationReport::flushBatch() const {
    GeneralMessageHandler::getMutex().lock();
    std::cerr << m_outputStream.str();
    GeneralMessageHandler::getMutex().unlock();
    m_outputStream.str("");
    m_actualBatch = 0;
}
