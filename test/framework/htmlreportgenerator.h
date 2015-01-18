#ifndef HTMLREPORTGENERATOR_H
#define HTMLREPORTGENERATOR_H

#include <framework/filereportgenerator.h>

class HtmlReportGenerator: public FileReportGenerator {
public:
    void save(const Report & report,
              const char * dest) const;
private:
    void generateHead(std::ostream & output, const ReportModule & module) const;

    void generateBody(std::ostream & output, const ReportModule & module, const std::string &dest) const;

    void generateTable(std::ostream & output, const ReportTable & table, const std::string dest) const;

    std::string getCamelCase(const std::string &value, bool lower = false) const;

    void generateTableScript(const ReportTable & table, std::string *script) const;
};

#endif // HTMLREPORTGENERATOR_H
