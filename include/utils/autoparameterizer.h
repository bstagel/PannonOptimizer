#ifndef AUTOPARAMETERIZER_H
#define AUTOPARAMETERIZER_H

#include <lp/model.h>
#include <simplex/simplexmodel.h>

class AutoParameterizer {
public:
    void selfParameterize();
    void printStatistics();

    AutoParameterizer(const SimplexModel& model);
    virtual ~AutoParameterizer();
private:
    SimplexModel m_model;
};


#endif // AUTOPARAMETERIZER_H

