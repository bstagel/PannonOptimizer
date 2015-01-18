#include "report.h"

std::string Report::title() const
{
    return m_title;
}

void Report::setTitle(const std::string &title)
{
    m_title = title;
}

void Report::addModule(const ReportModule &module)
{
    m_modules.push_back(module);
}

const std::vector<ReportModule> &Report::getModules() const
{
    return m_modules;
}

std::string ReportModule::name() const
{
    return m_name;
}

void ReportModule::setName(const std::string &name)
{
    m_name = name;
}

void ReportModule::addTable(const ReportTable &table)
{
    m_tables.push_back(table);
}

const std::vector<ReportTable> &ReportModule::getTables() const
{
    return m_tables;
}

ReportTable::ReportTable(unsigned int columns)
{
    m_columns.resize(columns);
}

void ReportTable::setCell(unsigned int row, unsigned int column, const ReportTable::Cell &cell)
{
    if (m_cells.size() <= row) {
        unsigned int oldSize = m_cells.size();
        m_cells.resize(row + 1);
        unsigned int rowIndex;
        for (rowIndex = oldSize; rowIndex <= row; rowIndex++) {
            m_cells[rowIndex].resize( m_columns.size() );
        }
    }
    m_cells[row][column] = cell;
}

void ReportTable::setColumn(unsigned int columnIndex, const ReportTable::Column &column)
{
    m_columns[columnIndex] = column;
}

const std::vector<ReportTable::Column> &ReportTable::getColumns() const
{
    return m_columns;
}

const std::vector<std::vector<ReportTable::Cell> > &ReportTable::getTable() const
{
    return m_cells;
}

std::string ReportTable::title() const
{
    return m_title;
}

void ReportTable::setTitle(const std::string &title)
{
    m_title = title;
}
