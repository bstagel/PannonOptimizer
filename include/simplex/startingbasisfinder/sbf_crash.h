#ifndef SBF_CRASH_H
#define SBF_CRASH_H

#include <globals.h>

#include <simplex/startingbasisfinder/sbf_super.h>

class SbfCrash : public SbfSuper
{
public:
    SbfCrash(const SimplexModel& model,
               std::vector<int>* basisHead,
               IndexList<const Numerical::Double*>* variableStates,
               StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);

    virtual ~SbfCrash();

    virtual void run();
};

#endif // SBF_CRASH_H
