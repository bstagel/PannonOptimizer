/**
 * @file sbf_super.cpp
 */

#include <sstream>
#include <map>
#include <set>
#include <vector>

#include <simplex/startingbasisfinder/sbf_super.h>
#include <simplex/simplex.h>

#include <debug.h>

using namespace std;

typedef pair<int,int> intpair;

SbfSuper::SbfSuper(const SimplexModel& model,
                   std::vector<int>* basisHead,
                   IndexList<const Numerical::Double *> *variableStates,
                   Vector* basicVariableValues):
    m_model(model),
    m_basisHead(basisHead),
    m_variableStates(variableStates),
    m_basicVariableValues(basicVariableValues)
{

}

SbfSuper::~SbfSuper()
{

}

void SbfSuper::adjustVariableByType(unsigned int variableIndex, Simplex::VARIABLE_STATE state)
{
    const Variable& variable = m_model.getVariable(variableIndex);
    if (state == Simplex::NONBASIC_AT_LB || state == Simplex::NONBASIC_AT_UB) {
        if (variable.getType() == Variable::FREE) {
            m_variableStates->insert(Simplex::NONBASIC_FREE, variableIndex, &ZERO);
        } else if (variable.getType() == Variable::MINUS) {
            m_variableStates->insert(Simplex::NONBASIC_AT_UB, variableIndex, &(variable.getUpperBound()));
            return;
        } else if (variable.getType() == Variable::PLUS) {
            m_variableStates->insert(Simplex::NONBASIC_AT_LB, variableIndex, &(variable.getLowerBound()));
            return;
        } else if ( variable.getType() == Variable::FIXED ) {
            m_variableStates->insert(Simplex::NONBASIC_FIXED, variableIndex, &(variable.getLowerBound()));
            return;
        } else { // BOUNDED
            if (state == Simplex::NONBASIC_AT_LB) {
                m_variableStates->insert(Simplex::NONBASIC_AT_LB, variableIndex, &(variable.getLowerBound()));
                return;
            } else{
                m_variableStates->insert(Simplex::NONBASIC_AT_UB, variableIndex, &(variable.getUpperBound()));
                return;
            }
        }
    } else {
        LPWARNING("Bad state given in adjustVariableByType");
    }

    return;
}

//bool SbfSuper::testTriangularity()
//{
//    unsigned int i;    // index variable
//    unsigned int j;    // index variable
//    unsigned int k;    // index variable

//    vector< intpair > pivots;  // pivot positions

//    map< unsigned int, set<unsigned int> > rowNzIndeces;
//    map< unsigned int, set<unsigned int> > :: iterator mapit;
//    map< unsigned int, set<unsigned int> > :: iterator mapjt;
//    set<unsigned int> :: iterator setit;

//    /* Number of nonzeros per a row */
//    vector<unsigned int> rowNzCount(orig_basisSize, 0);
//    /* Number of nonzeros per a column */
//    vector<unsigned int> columnNzCount(orig_basisSize, 0);

//    /* Calculations for the first iteration*/
//    /* Number of nonzeros per row.
//      * If it is 0, then no need for further calculations.
//      * rowIndeces: key = number of nonzeros; value = row index set
//      */
//    DEVINFO(D::SBF_SUPER, "orig_basisSize: " << orig_basisSize << " " << orig_matrix.rowCount() << " " << orig_matrix.columnCount());
//    for (i=0; i < sb_basisHead.size(); i++) {
//    	DEVINFO(D::SBF_SUPER,  i << ". " << sb_basisHead.at(i));
//    }

//     for (i=0; i < orig_basisSize; i++) {
//    	 rowNzCount[i] = 0;
//    	 for (j=0; j < sb_basisHead.size(); j++) {
//			 /* Structural variable */
//             if (sb_basisHead.at(j) < orig_structVarCount) {
//				 const Vector& v = orig_matrix.column(sb_basisHead.at(j));
//				 if (!Numerical::isZero(v.at(i)) ) {
//					 rowNzCount[i]++;
//				 }
//			 } else {
//				 if (i== (sb_basisHead.at(j)-orig_structVarCount)) {
//					 rowNzCount[i]++;
//				 }
//			 }
//    	 }
//		 if (rowNzCount[i]) {
//			 rowNzIndeces[rowNzCount[i]].insert(i);
//		 }
//     }

//     DEVINFO(D::SBF_SUPER, "Count column  nonz...");

//     /* Number of nonzeros per column. */
//     for (i=0; i < sb_basisHead.size(); i++) {
//    	 DEVINFO(D::SBF_SUPER, "Column " << i);
//    	 /* Structural variable */
//    	 DEVINFO(D::SBF_SUPER, "Real Column " << sb_basisHead.at(i));
//    	 if (sb_basisHead.at(i) < orig_structVarCount) {
//    		 DEVINFO(D::SBF_SUPER, ".. struct var");
//             const Vector& v = orig_matrix.column(sb_basisHead.at(i));
//             j = v.nonZeros();
//             DEVINFO(D::SBF_SUPER, "Orig column count " << j);
//             if (j > 0) {
//                 columnNzCount[i] = j;
//             }
//         } else {
//        	 DEVINFO(D::SBF_SUPER, ".. logical var");
//             columnNzCount[i] = 1;
//         }
//    	 DEVINFO(D::SBF_SUPER, "Count column ok.");
//     }

//     DEVINFO(D::SBF_SUPER, "Column calculating done");

//    bool change = true;
//    /* Loop: while change exists and basis size is not reached */
//    while (change == true) {

//        change = false;

//        unsigned int minRCount = orig_structVarCount + 1;    // initialize with a big number
//        unsigned int minRIdx = -1, minCIdx = -1;

//        // Find the row with minim non-zeros
//        mapit = rowNzIndeces.begin();  // first set in the map
//        /* Clear the first part of the map from the unnecessary sets */
//        while (mapit != rowNzIndeces.end() && (*mapit).second.size() == 0) {
//            mapjt = mapit;
//            mapit++;
//            rowNzIndeces.erase(mapjt);
//        }

//        /* If the map did not became empty */
//        if (rowNzIndeces.size() > 0) {
//            mapit = rowNzIndeces.begin();  // first set in the map
//            minRCount = (*mapit).first;         // row count
//            setit = (*mapit).second.begin();    // first item in the set
////            /* Just for log */
////            for (set<unsigned int>::iterator ti = setit; ti != (*mapit).second.end(); ti++) {
////                DEVINFO(D::SBF_SUPER, "Search... minRCount: " << minRCount << ", in row: " << *ti);
////            }

//            minRIdx = (*setit);                 // row index
//        }

//        /* The algorithm can go further */
//        if (minRCount == 1) {
//        	DEVINFO(D::SBF_SUPER, "Selected row: " << minRIdx);

//            /* Find column in the selected row */
//            /* If there is only one non-zero in this row -- obvious */
//           	for (j=0; j < sb_basisHead.size(); j++) {
//           		DEVINFO(D::SBF_SUPER, j << ". columnNzCount: " << columnNzCount.at(j));
//           		 if (columnNzCount.at(j)) {
//					 /* Structural variable */
//					 if (sb_basisHead.at(j) < orig_structVarCount) {
//						 const Vector& v = orig_matrix.column(sb_basisHead.at(j));
//						 if (!Numerical::isZero(v.at(minRIdx)) ) {
//							 minCIdx = j;
//							 break;
//						 }
//					 } else {
//						 if (minRIdx== (sb_basisHead.at(j)-orig_structVarCount)) {
//							 minCIdx = j;
//							 break;
//						 }
//					 }
//           		 }
//           	}

//           	if (minCIdx == -1 ) {
//           		DEVINFO(D::SBF_SUPER, "Column not found..");
//           		break;
//           	}

//			/* recompute the row counts after removing the column */
//			/* Structural variable */
//			if (sb_basisHead.at(minCIdx) < orig_structVarCount) {

//				const Vector & selectedColumn = orig_matrix.column(sb_basisHead.at(minCIdx));
//				for (Vector::NonzeroIterator it = selectedColumn.beginNonzero(); it != selectedColumn.endNonzero(); it++ ) {
//					j = it.getIndex();
//					k = rowNzCount.at(j);
//					if (k > 0) {
//						rowNzCount[j]--;
//						if (k-1 > 0) {
//							rowNzIndeces[k-1].insert(j);
//						}
//						rowNzIndeces.at(k).erase(j);
//					}
//				}
//			} else {
//				/* Logically remove this row */
//				rowNzIndeces.at(1).erase(minRIdx);
//				/* Logical variable has only one nonzero, it should not affect other rows */
//			}

//			/* push to the basis head */
//			pivots.push_back(intpair(minRIdx,minCIdx));
//			/* remove row logically */
//			rowNzCount[minRIdx] = 0;
//			/* remove column logically */
//			columnNzCount[minCIdx] = 0;
//			/* there is a chance to continue the algorithm */
//			change = true;

//			DEVINFO(D::SBF_SUPER, "Pivot: " << minRIdx << " " << minCIdx);
//        }
//    }

//    DEVINFO(D::SBF_SUPER, "Done. Pivot size: " << pivots.size());

//    if (pivots.size() == sb_basisHead.size()) {
//    	return true;
//    }

//    cerr << "ERROR: NonTriangularStartingBasis" << endl;
//	return false;
//}
