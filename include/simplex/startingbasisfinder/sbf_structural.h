/**
 * @file sbf_structural_h
 */

#ifndef SBF_STRUCTURAL_H
#define SBF_STRUCTURAL_H

#include <globals.h>

#include <simplex/startingbasisfinder/sbf_super.h>

class SbfStructural : public SbfSuper
{
public:
    SbfStructural(const SimplexModel& model,
                  std::vector<int>* basisHead,
                  IndexList<const Numerical::Double*>* variableStates,
                  StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);
    virtual ~SbfStructural();

    virtual void run();
};

#endif // SBF_STRUCTURAL_H
