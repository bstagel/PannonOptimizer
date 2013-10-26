#include <utils/iterationreport.h>
#include <utils/iterationreportprovider.h>
#include <debug.h>
#include <sstream>
#include <iomanip>

IterationReport::IterationReport() {

}

IterationReport::~IterationReport() {

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

void IterationReport::createStartReport() {

}

void IterationReport::writeStartReport() {

}

void IterationReport::createIterationReport() {
    std::vector<Entry> newRow;

    STL_FOREACH(std::vector<IterationReportField>,
                m_iterationFields, startFieldIter) {
        IterationReportField field = *startFieldIter;
        Entry newEntry;
        std::string name = field.getName();
        enum IterationReportProvider::ITERATION_REPORT_FIELD_TYPE type = IterationReportProvider::IRF_ITERATION;
        switch ( field.getType() ) {
        case IterationReportField::IRF_BOOL:
            newEntry.m_bool = field.getProvider().getIterationReportBool(name, type);
            break;
        case IterationReportField::IRF_FLOAT:
            newEntry.m_float = field.getProvider().getIterationReportFloat(name, type);
            break;
        case IterationReportField::IRF_INT:
            newEntry.m_integer = field.getProvider().getIterationReportInteger(name, type);
            break;
        case IterationReportField::IRF_STRING:
            newEntry.m_string = new std::string(field.getProvider().getIterationReportString(name, type));
            break;
        }

        newRow.push_back(newEntry);
    }
    m_iterationTable.push_back(newRow);
}

void IterationReport::writeIterationReport() {
    if (m_iterationTable.size() == 0) {
        return;
    }

    if (m_iterationTable.size() == 1) {
        std::ostringstream headerString;
        unsigned int index = 0;
        STL_FOREACH(std::vector<IterationReportField>, m_iterationFields, fieldsIter) {
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

    std::ostringstream row;
    const std::vector<Entry> & lastRow = m_iterationTable[ m_iterationTable.size() - 1 ];

    unsigned int index;
    for (index = 0; index < lastRow.size(); index++) {
        std::ostringstream entryString;
        IterationReportField field = m_iterationFields[index];
        const Entry & newEntry = lastRow[index];
        switch ( field.getType() ) {
        case IterationReportField::IRF_BOOL:
            entryString << (newEntry.m_bool == true ? "true" : "false");
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
            entryString << newEntry.m_float;
            break;
        case IterationReportField::IRF_INT:
            entryString << newEntry.m_integer;
            break;
        case IterationReportField::IRF_STRING:
            entryString << *newEntry.m_string;
            break;
        }

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
    LPINFO(row.str());
}

void IterationReport::createSolutionReport() {

}

void IterationReport::writeSolutionReport() {

}
