#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <framework/report.h>

class ReportGenerator {
public:
    virtual void save(const Report & report,
                      const char * dest) const = 0;
};

#endif // REPORTGENERATOR_H
