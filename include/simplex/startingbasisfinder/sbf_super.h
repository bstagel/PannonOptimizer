/*
 * sbf_super.h
 *
 *  Created on: Apr 1, 2012
 *      Author: greenvirag
 */

#ifndef SBF_SUPER_H_
#define SBF_SUPER_H_

#include <globals.h>

#include <vector>
#include <set>
#include <assert.h>

#include <simplex/simplexmodel.h>
#include <simplex/simplex.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

class SbfSuper
{
public:
    SbfSuper(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             Vector* basicVariableValues);
	virtual ~SbfSuper();

    virtual void run() = 0;

protected:
    const SimplexModel & m_model;
    std::vector<int> * m_basisHead;
    IndexList<const Numerical::Double*> * m_variableStates;
    Vector* m_basicVariableValues;

//    unsigned int            	m_basisStructVarCount;

    void adjustVariableByType(unsigned int variableIndex, Simplex::VARIABLE_STATE state);
//	void calculateNonZeros(std::vector<unsigned int>* rows, std::vector<unsigned int>* columns,
//			std::map< unsigned int, std::set<unsigned int> >* rowIndeces = NULL);
};


//inline unsigned int SbfSuper::getBasisStructVarCount()
//{
//	return m_basisStructVarCount;
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
