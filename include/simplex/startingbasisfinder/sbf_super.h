/**
 * @file sbf_super.h
 */

#ifndef SBF_SUPER_H_
#define SBF_SUPER_H_

#include <globals.h>

#include <vector>
#include <set>
#include <assert.h>

#include <simplex/simplexmodel.h>
#include <simplex/simplex.h>
#include <simplex/startingbasisfinder.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

class SbfSuper
{
public:
    SbfSuper(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);
	virtual ~SbfSuper();

    virtual void run() = 0;

protected:
    const SimplexModel & m_model;
    std::vector<int> * m_basisHead;
    IndexList<const Numerical::Double*> * m_variableStates;
    StartingBasisFinder::STARTING_NONBASIC_STATES m_nonbasicStates;

    void adjustVariableByType(unsigned int variableIndex, Simplex::VARIABLE_STATE state);
};


#endif /* SBF_SUPER_H_ */
