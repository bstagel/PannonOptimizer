/*
 * sbf_super.h
 *
 *  Created on: Apr 1, 2012
 *      Author: greenvirag
 */

#ifndef SBF_SUPER_H_
#define SBF_SUPER_H_

#include "globals.h"

#include <vector>
#include <set>
#include <assert.h>

#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

class SbfSuper
{
public:
    SbfSuper(const SimplexModel& model, std::vector<int>& basisHead, IndexList& variableStates);
	virtual ~SbfSuper();

    virtual void run() = 0;

protected:
    const SimplexModel & m_model;
    std::vector<int> & m_basisHead;
    IndexList & m_variableStates;

//    unsigned int            	m_basisStructVarCount;

//    void adjustVariableByType(Variable *v, Variable::STATE t);
//	void calculateNonZeros(std::vector<unsigned int>* rows, std::vector<unsigned int>* columns,
//			std::map< unsigned int, std::set<unsigned int> >* rowIndeces = NULL);
};


//inline unsigned int SbfSuper::getBasisStructVarCount()
//{
//	return m_basisStructVarCount;
//}

//inline void SbfSuper::adjustVariableByType(Variable* v, Variable::STATE t)
//{
//    if (t == Variable::NONBASIC_AT_LB || t == Variable::NONBASIC_AT_UB) {
//        if (v->getType() == Variable::FREE) {
//            v->setState(Variable::SUPERBASIC);
//            v->setValue(0.);
//        } else if (v->getType() == Variable::MINUS) {
//            goto SETUPPER;
//        } else if (v->getType() == Variable::PLUS) {
//            goto SETLOWER;
//        } else {
//            if (t == Variable::NONBASIC_AT_LB) {
//                goto SETLOWER;
//            } else {
//                goto SETUPPER;
//            }
//        }
//    } else {
//        v->setState(t);
//    }

//    return;

//    SETUPPER: {
//        Numerical::Double temp = v->getUpperBound();
//        v->setState(Variable::NONBASIC_AT_UB);
//        v->setValue(temp);
//        return;
//    }

//    SETLOWER: {
//        Numerical::Double temp = v->getLowerBound();
//        v->setState(Variable::NONBASIC_AT_LB);
//        v->setValue(temp);
//        return;
//    }

//}

//inline void SbfSuper::calculateNonZeros(std::vector<unsigned int>* rows, std::vector<unsigned int>* columns,
//        std::map< unsigned int, std::set<unsigned int> >* rowIndeces)
//{
//    unsigned int i, j, k;

//    /* Number of nonzeros per row. Note: just for matrix A_s
//      * If it is 0, then no need for further calculations.
//      * rowIndeces: key = number of nonzeros; value = row index set
//      */
//     for (i=0; i<orig_basisSize; i++) {
//         if (sb_freeBasisPositions.at(i)) {
//             const Vector& v = orig_matrix.row(i);
//             j = v.nonZeros();
//             DEVINFO(D::STARTINGBASISFINDER, "Original row nonzero count: " << i << ". row: " << j);
//             if (j > 0) {
//                 for (k=0; k<orig_structVarCount; k++) {
//                     if (sb_freeStructVars.at(k) == false && !Numerical::isZero(v.at(k))) {
//                    	 j--;
//                     }
//                 }
//                 DEVINFO(D::STARTINGBASISFINDER, "Calculated row nonzero count: " << i << ". row: " << j);
//                 (*rows)[i] = j;
//                 if (j && rowIndeces) {
//                     (*rowIndeces)[j].insert(i);
//                 }
//             }
//         } else {
//             (*rows)[i] = 0;
//         }
//     }

//     /* Number of nonzeros per column. Note: just for matrix A_s */
//     for (i=0; i<orig_structVarCount; i++) {
//         if (sb_freeStructVars.at(i)) {
//             const Vector& v = orig_matrix.column(i);
//             j = v.nonZeros();
//             DEVINFO(D::STARTINGBASISFINDER, "Original col nonzero count: " << i << ". col: " << j);
//             if (j > 0) {
//                 (*columns)[i] = j;
//             }
//         } else {
//             (*columns)[i] = 0;
//         }
//     }

//}

#endif /* SBF_SUPER_H_ */
