#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include <map>

class ReportModule;

class ReportTable {    
public:
    enum class ALIGNMENT {LEFT, RIGHT, CENTER};

    enum class TYPE {INTEGER, FLOAT, STRING};

    struct Cell {
        std::string m_content;
        ALIGNMENT m_alignment;
        ReportModule * m_link;
        bool m_head;
    };

    struct Column {
        bool m_sortable;
        TYPE m_type;
    };


    ReportTable(unsigned int columns);

    void setCell(unsigned int row, unsigned int column, const Cell & cell);

    void setColumn(unsigned int columnIndex, const Column & column);

    const std::vector<Column> &getColumns() const;

    const std::vector<std::vector<Cell> > &getTable() const;

    std::string title() const;
    void setTitle(const std::string &title);

private:
    std::string m_title;

    std::vector<Column> m_columns;

    std::vector<std::vector<Cell>> m_cells;
};

class ReportModule {
public:

    std::string name() const;
    void setName(const std::string &name);

    void addTable(const ReportTable &table);

    const std::vector<ReportTable> &getTables() const;

private:
    std::string m_name;
    std::vector<ReportTable> m_tables;
};

class Report {
public:

    std::string title() const;

    void setTitle(const std::string &title);

    void addModule(const ReportModule & module);

    const std::vector<ReportModule> &getModules() const;

private:
    std::string m_title;
    std::vector<ReportModule> m_modules;
};

#endif // REPORT_H
