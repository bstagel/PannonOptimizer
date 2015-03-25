/**
 * @file sbf_logical.h
 */

#ifndef SBF_LOGICAL_H_
#define SBF_LOGICAL_H_

#include <globals.h>

#include <simplex/startingbasisfinder/sbf_super.h>

class SbfLogical : public SbfSuper
{
public:
    SbfLogical(const SimplexModel& model,
               std::vector<int>* basisHead,
               IndexList<const Numerical::Double *> *variableStates,
               StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);
	virtual ~SbfLogical();

	virtual void run();
};

#endif /* SBF_LOGICAL_H_ */
