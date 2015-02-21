#include <framework/htmlreportgenerator.h>
#include <sstream>
#include <iostream>

void HtmlReportGenerator::save(const Report &report,
                               const char *dest) const
{
    clearDirectory(dest);
    std::ofstream output;

    for ( auto & module: report.getModules() ) {
        getFile(dest, "index.html", &output);
        generateHead(output, module);
        generateBody(output, module, dest);
        output.close();
    }
}

void HtmlReportGenerator::generateHead(std::ostream &output,
                                       const ReportModule &module) const
{
    output << "<!DOCTYPE html>" << std::endl;
    output << "<html>" << std::endl;
    output << "<head>" << std::endl;

    /*output << "<style>" << std::endl;
    output << "table, th, td {" << std::endl;
    output << "\tborder: 1px solid black;" << std::endl;
    output << "\tborder-collapse: collapse;" << std::endl;
    output << "}" << std::endl;
    output << "</style>" << std::endl;*/

    output << "<style>" << std::endl;
    output << "table {" << std::endl;
    output << "font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;" << std::endl;
    //output << "width: 100%;" << std::endl;
    output << "border-collapse: collapse;" << std::endl;
    output << "}" << std::endl;

    output << "table td, th {" << std::endl;
    output << "font-size: 1em;" << std::endl;
    output << "border: 1px solid #a0b4bb;" << std::endl;
    // 98bf21
    output << "padding: 3px 7px 2px 7px;" << std::endl;
    output << "}" << std::endl;

    output << "table th {" << std::endl;
    output << "font-size: 1.1em;" << std::endl;
    output << "text-align: left;" << std::endl;
    output << "padding-top: 5px;" << std::endl;
    output << "padding-bottom: 4px;" << std::endl;
    output << "background-color: #5081bb;" << std::endl;
    // green: A7C942
    output << "color: #ffffff;" << std::endl;
    output << "}" << std::endl;

    output << "table tr.alt td {" << std::endl;
    output << "color: #000000;" << std::endl;
    output << "background-color: #d3e0e9;" << std::endl;
    // EAF2D3
    output << "}" << std::endl;

    output << "a:link.column {" << std::endl;
    output << "color: #FFFFFF;" << std::endl;
    output << "}" << std::endl;

    output << "a:visited.column {" << std::endl;
    output << "color: #FFFFFF;" << std::endl;
    output << "}" << std::endl;

    output << "a:hover.column {" << std::endl;
    output << "color: #d3e0e9;" << std::endl;
    output << "}" << std::endl;

    output << "a:active.column {" << std::endl;
    output << "color: #d3e0e9;" << std::endl;
    output << "}" << std::endl;

    output << "</style>" << std::endl;

    output << "\t<title>" << module.name() << "</title" << std::endl;
    output << "</head>" << std::endl;
}

void HtmlReportGenerator::generateBody(std::ostream &output,
                                       const ReportModule &module,
                                       const std::string & dest) const
{
    std::string script;
    script = "\t<script>";
    output << "<body>" << std::endl;

    for (auto & table: module.getTables()) {
        generateTable(output, table, dest);
        generateTableScript(table, &script);
    }
    script = script + "\t</script>";

    output << script << std::endl;

    output << "</body>" << std::endl;
    output << "</html>" << std::endl;
}

void HtmlReportGenerator::generateTable(std::ostream &output,
                                        const ReportTable &table,
                                        const std::string dest) const
{
    output << "\t<h1>" << table.title() << "</h1>" << std::endl;
    output << "\t<table ";
    std::string name = getCamelCase( table.title(), true ) + "Table";
    output << " id=\"" << name << "\"";
    output << ">" << std::endl;

    unsigned int rowIndex = 0;
    for (auto & row: table.getTable()) {
        output << "\t\t<tr ";
        if (rowIndex % 2 == 0) {
            output << "class=\"alt\"" << std::endl;
        }
        output << ">" << std::endl;
        unsigned int columnIndex = 0;
        for (auto & cell: row) {
            //
            if (cell.m_head) {
                output << "\t\t\t<th";
            } else {
                output << "\t\t\t<td";
            }
            switch (cell.m_alignment) {
            case ReportTable::ALIGNMENT::CENTER:
                output << " align=\"center\"";
                break;
            case ReportTable::ALIGNMENT::RIGHT:
                output << " align=\"right\"";
                break;
            case ReportTable::ALIGNMENT::LEFT:
                output << " align=\"left\"";
                break;
            }
            output << "> ";
            //<a onclick="unitTestSummarySort(compareByName, nameOrder); nameOrder = 1 - nameOrder;" href="#" >
            if (rowIndex == 0 && table.getColumns()[columnIndex].m_sortable ) {
                output << "<a class=\"column\" onclick=\"";
                output << name << "Sort(compareBy";
                output << getCamelCase(cell.m_content) << ", ";
                output << getCamelCase(cell.m_content, true) << "Order); ";
                output << getCamelCase(cell.m_content, true) << "Order = 1 - ";
                output << getCamelCase(cell.m_content, true) << "Order;\" href=\"#\" > ";
            }
            if (cell.m_link != nullptr) {
                std::string fileName = getNewIndexedFileName(fixPath(dest), ".html");
                fileName = fileName.substr( dest.length() + 1 );
                std::cout << "\tFILE NAME: " << fileName << std::endl;
                output << "<a href=\"" << fileName << "\"> " << cell.m_content << "</a> </t";

                // save
                std::ofstream output2;

                getFile(dest, fileName.c_str(), &output2);
                generateHead(output2, *cell.m_link);
                generateBody(output2, *cell.m_link, dest);
                output2.close();

            } else {
                output << cell.m_content << " </t";
            }
            if (cell.m_head) {
                output << "h>";
            } else {
                output << "d>";
            }
            output << std::endl;
            columnIndex++;
        }
        output << "\t\t</tr>" << std::endl;
        rowIndex++;
    }
    output << "\t</table>" << std::endl;
}

std::string HtmlReportGenerator::getCamelCase(const std::string &value, bool lower) const
{
    std::ostringstream stream;
    for (auto ch: value) {
        if (ch == ' ') {
            lower = false;
            continue;
        }
        if (lower) {
            stream << (char)(ch | 32);
        } else {
            stream << (char)(ch & (255 - 32));
        }
        lower = true;
    }
    return stream.str();
}

void HtmlReportGenerator::generateTableScript(const ReportTable &table,
                                              std::string * script) const
{
    std::ostringstream stream;
    std::string tableName = getCamelCase(table.title(), true) + "Table";
    // get column names
    std::vector<std::string> columnNames;
    std::vector<ReportTable::Column> columns;
    unsigned int columnIndex = 0;
    for (auto & name: table.getTable()[0]) {
        if (table.getColumns()[columnIndex].m_sortable) {
            columnNames.push_back( name.m_content );
            columns.push_back(table.getColumns()[columnIndex]);
        }
        columnIndex++;
    }

    if (columnNames.size() == 0) {
        return;
    }

    stream << std::endl;
    // generate order variables
    for (auto & name: columnNames) {
        stream << "\t\tvar " << getCamelCase(name, true) << "Order = 0;" << std::endl;
    }

    // generate compare functions
    columnIndex = 0;
    for (auto & name: columnNames) {
        stream << std::endl;
        stream << "\t\tfunction compareBy" << getCamelCase(name) << "(a, b) {" << std::endl;

        stream << "\t\t\tvar x = ";
        if (columns[columnIndex].m_type == ReportTable::TYPE::INTEGER) {
            stream << "parseInt(";
        }
        if (columns[columnIndex].m_type == ReportTable::TYPE::FLOAT) {
            stream << "parseFloat(";
        }
        stream << "a." << getCamelCase(name, true);
        if (columns[columnIndex].m_type != ReportTable::TYPE::STRING) {
            stream << ")";
        }
        stream << ";" << std::endl;

        stream << "\t\t\tvar y = ";
        if (columns[columnIndex].m_type == ReportTable::TYPE::INTEGER) {
            stream << "parseInt(";
        }
        if (columns[columnIndex].m_type == ReportTable::TYPE::FLOAT) {
            stream << "parseFloat(";
        }
        stream << "b." << getCamelCase(name, true);
        if (columns[columnIndex].m_type != ReportTable::TYPE::STRING) {
            stream << ")";
        }
        stream << ";" << std::endl;

        //stream << "\t\t\tvar y = b." << getCamelCase(name, true) << ";" << std::endl;

        stream << "\t\t\treturn (x < y) ? -1 : ((x > y) ? 1 : 0);" << std::endl;
        stream << "\t\t}" << std::endl;
        columnIndex++;
    }

    // generate sorter function
    stream << std::endl;
    stream << "\t\tfunction " << tableName << "Sort(compare, order) {" << std::endl;
    stream << "\t\t\tvar table = document.getElementById(\'" << tableName << "\');" << std::endl;
    stream << std::endl;
    stream << "\t\t\tvar rowIndex;" << std::endl;
    stream << "\t\t\tvar content = [];" << std::endl;
    stream << "\t\t\tfor (rowIndex = 1; rowIndex < table.rows.length; rowIndex++) {" << std::endl;
    stream << "\t\t\t\tvar cells = table.rows[rowIndex].cells;" << std::endl;
    stream << std::endl;
    stream << "\t\t\t\tvar rowContent = {};" << std::endl;
    unsigned int index = 0;
    for (auto & name: table.getTable()[0]) {
        stream << "\t\t\trowContent." << getCamelCase(name.m_content, true) << " = cells[" << index << "].innerHTML;" << std::endl;
        index++;
    }
    stream << "\t\t\tcontent[content.length] = rowContent;" << std::endl;
    stream << "\t\t}" << std::endl;
    stream << std::endl;


    stream << "\t\tcontent.sort(compare);" << std::endl;
    stream << "\t\tvar start;" << std::endl;
    stream << "\t\tvar end;" << std::endl;
    stream << "\t\tvar step;" << std::endl;
    stream << "\t\tif (order < 1) {" << std::endl;
    stream << "\t\t\tstart = 0;" << std::endl;
    stream << "\t\t\tstep = 1;" << std::endl;
    stream << "\t\t} else {" << std::endl;
    stream << "\t\t\tstart = content.length - 1;" << std::endl;
    stream << "\t\t\tstep = -1;" << std::endl;
    stream << "\t\t}" << std::endl;
    stream << "\t\tvar contentIndex = start;" << std::endl;
    stream << "\t\tfor (rowIndex = 1; rowIndex < table.rows.length; rowIndex++, contentIndex += step) {" << std::endl;
    stream << "\t\t\tvar cells = table.rows[rowIndex].cells;" << std::endl;

    index = 0;
    for (auto & name: table.getTable()[0]) {
        stream << "\t\t\tcells[" << index << "].innerHTML = content[contentIndex]." << getCamelCase(name.m_content, true) << ";" << std::endl;
        index++;
    }
    stream << "\t\t}" << std::endl;
    stream << "\t}" << std::endl;

    *script = *script + stream.str();
}
