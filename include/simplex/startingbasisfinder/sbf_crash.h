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

    Numerical::Double getTime() const {
        return m_time;
    }

    unsigned int getStructuralVariableCount() const {
        return m_structuralVariables;
    }
private:

    Numerical::Double m_time;

    unsigned int m_structuralVariables;
};

#endif // SBF_CRASH_H
