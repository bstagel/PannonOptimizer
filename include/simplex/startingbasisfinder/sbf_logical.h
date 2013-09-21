/*
 * sbf_logical.h
 *
 *  Created on: Apr 1, 2012
 *      Author: greenvirag
 */

#ifndef SBF_LOGICAL_H_
#define SBF_LOGICAL_H_

#include "globals.h"

#include <simplex/startingbasisfinder/sbf_super.h>

class SbfLogical : public SbfSuper
{

public:
    enum LogicalStrategy {
        LOWER_LOGICAL,
        UPPER_LOGICAL,
        MIXED_LOGICAL
    };

    SbfLogical(const SimplexModel& model, std::vector<int>& basisHead, IndexList& variableStates, LogicalStrategy strategy);
	virtual ~SbfLogical();

	virtual void run();

private:
    LogicalStrategy m_strategy;
};

#endif /* SBF_LOGICAL_H_ */
