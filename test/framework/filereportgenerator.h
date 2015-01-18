#ifndef FILEREPORTGENERATOR_H
#define FILEREPORTGENERATOR_H

#include <framework/reportgenerator.h>
#include <fstream>

class FileReportGenerator: public ReportGenerator {
public:
    void getFile(const std::string &directory,
                 const std::string &fileName,
                 std::ofstream *output) const;

    void clearDirectory(const std::string & directory) const;
protected:
    std::string getNewIndexedFileName(const std::string &prefix,
                                      const std::string &postfix) const;

    std::string fixPath(const std::string &path) const;
};

#endif // FILEREPORTGENERATOR_H
