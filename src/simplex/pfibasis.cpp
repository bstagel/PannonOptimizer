///*
// * pfibasis.cpp
// *
// *  Created on: Feb 9, 2011
// *      Author: Peter Tar
// */

//#include <string>
//#include <stdexcept>

//#include <simplex/pfibasis.h>
//#include <simplex/linalg.h>
//#include <lp/model.h>
//#include <simplex/simplexparameters.h>

//#include <utils/testdata.h>

//const double e_relative = SimplexParameters::getParameterValue("e_relative");

//int PfiBasis::m_inversionCount = 0;

//PfiBasis::PfiBasis(Model * const model) :
//    Basis(model) {
//    m_basis = new std::vector<ETM>();
//    m_etaStorer = new Linalg::EtaStorer(model->getMatrix().rowCount());

//    m_transformationCount = 0;
//    m_transformationAverage = 0;
//    m_mNumAverage = 0;

//    m_cColumns = new std::vector<Vector*>();
//    m_cPivotIndexes = new std::vector<int>();

//    m_mmRows = new std::vector<Vector>();
//    m_mmRowIndices = new std::vector<int>();
//    m_mmColumns = new std::vector<Vector>();
//    m_mmColumnIndices = new std::vector<int>();
//    m_mmGraphOut = new std::vector<std::vector<int> >();
//    m_mmGraphIn = new std::vector<std::vector<int> >();
//    m_stack = new std::vector<PathNode>();
//    m_orderedIndex = new std::vector<bool>();
//    m_mmBlocks = new std::vector<int>();
//    m_rowSwapHash = new std::vector<int>();
//    m_columnSwapHash = new std::vector<int>();
//    m_columnSwapLog = new std::vector<int>();

//    triangularMethod = (TRIANGULAR_METHOD) SimplexParameters::getParameterValue("triangular_method");
//    nontriangularMethod = (NONTRIANGULAR_METHOD) SimplexParameters::getParameterValue("nontriangular_method");
//    triangularPivotRule = (TRIANGULAR_PIVOT_RULE) SimplexParameters::getParameterValue("triangular_pivot_rule");
//    nontriangularPivotRule
//            = (NONTRIANGULAR_PIVOT_RULE) SimplexParameters::getParameterValue("nontriangular_pivot_rule");
//    threshold = SimplexParameters::getParameterValue("pivot_threshold");


//    cl_inversion = 0;
//    cl_copy = 0;
//    cl_invertR = 0;
//    cl_invertM = 0;
//    cl_invertC = 0;
//    cl_transversal = 0;
//    cl_createBlockTriangular = 0;
//    cl_pivot = 0;
//    cl_updateColumns = 0;
//}

//PfiBasis::~PfiBasis() {
//    delete m_mmRows;
//    delete m_mmRowIndices;
//    delete m_mmColumns;
//    delete m_mmColumnIndices;
//    delete m_mmGraphOut;
//    delete m_mmGraphIn;
//    delete m_stack;
//    delete m_orderedIndex;
//    delete m_mmBlocks;
//    delete m_rowSwapHash;
//    delete m_columnSwapHash;
//    delete m_columnSwapLog;
//    delete m_cColumns;
//    delete m_cPivotIndexes;
//    for (std::vector<ETM>::iterator iter = m_basis->begin(); iter < m_basis->end(); iter++) {
//        delete iter->eta;
//    }
//    delete m_basis;
//    delete m_etaStorer;
//}

//void PfiBasis::invert() throw (NumericalException) {
//    clock_t inversionStart = clock();

//    m_transformationCount = 0;
//    m_inversionCount++;

//    //Reset the data structures
//    m_columns->clear();
//    m_columnsHash->clear();
//    m_rows->clear();
//    m_rowCounts->clear();
//    m_columnCounts->clear();
//    m_maxRowCount = 0;
//    m_maxColumnCount = 0;
//    m_cColumns->clear();
//    m_cPivotIndexes->clear();

//    for (std::vector<ETM>::iterator iter = m_basis->begin(); iter < m_basis->end(); iter++) {
//        delete iter->eta;
//    }
//    m_basis->clear();
//    m_etaStorer->clear();
//    m_basisNewHead.clear();

//    m_basisNonzeros = 0;
//    m_inverseNonzeros = 0;
//    m_invertedNonzeros = 0;

//    //Copy the basis for computation and discard logical variable rows and columns
//    clock_t copyStart = clock();
//    copyBasis();
//    clock_t copyStop = clock();

//    DEVINFO(D::PFIMAKER, "Basis copied");

//    printActiveSubmatrix();
//    //Invert the R, M, C parts separately
//    if (triangularMethod == LINKS) {
//        clock_t rowLinkBuildStart = clock();
//        buildRowLinks();
//        clock_t rowLinkBuildStop = clock();
//        cl_linkBuild += rowLinkBuildStop - rowLinkBuildStart;
//    }
//    clock_t invertRStart = clock();
//    invertR();
//    clock_t invertRStop = clock();
//    if (triangularMethod == LINKS) {
//        clock_t columnLinkBuildStart = clock();
//        buildColumnLinks();
//        clock_t columnLinkBuildStop = clock();
//        cl_linkBuild += columnLinkBuildStop - columnLinkBuildStart;
//    }
//    clock_t findCStart = clock();
//    findC();
//    clock_t findCStop = clock();
//    clock_t invertMStart = clock();
//    invertM();
//    clock_t invertMStop = clock();
//    clock_t invertCStart = clock();
//    invertC();
//    clock_t invertCStop = clock();
//    //Verify the result of the inversion
//    checkSingularity();

//    //Update the basis head
//    setNewHead();
//    m_isFresh = true;

//    if (triangularMethod == LINKS) {
//        if (m_rowLinks != NULL) {
//            delete m_rowLinks;
//            m_rowLinks = NULL;
//        }
//        if (m_columnLinks != NULL) {
//            delete m_columnLinks;
//            m_columnLinks = NULL;
//        }
//    }

//    clock_t inversionStop = clock();
//    //Save the inversion times
//    cl_inversion += inversionStop - inversionStart;
//    cl_copy += copyStop - copyStart;
//    cl_invertR += invertRStop - invertRStart;
//    cl_invertM += invertMStop - invertMStart;
//    cl_invertC += findCStop - findCStart;
//    cl_invertC += invertCStop - invertCStart;

//    //printStatistics();
//    m_transformationAverage += (m_transformationCount - m_transformationAverage) / m_inversionCount;
//}

//void PfiBasis::append(const Vector & v, int pivotRow, int incoming) throw (NumericalException) {
//    //If the alpha vector comes in, then ftran is done already
//    Vector incomingVector = v;

//    pivot(&incomingVector, pivotRow);

//    int outgoing = m_basisHead.at(pivotRow);
//    Variable & outgoingVariable = m_model->getVariable(outgoing);
//    if (Numerical::equals(outgoingVariable.getValue(), outgoingVariable.getLowerBound())) {
//        outgoingVariable.setState(Variable::NONBASIC_AT_LB);
//        outgoingVariable.setValue(outgoingVariable.getLowerBound());
//    } else if (Numerical::equals(outgoingVariable.getValue(), outgoingVariable.getUpperBound())) {
//        outgoingVariable.setState(Variable::NONBASIC_AT_UB);
//        outgoingVariable.setValue(outgoingVariable.getUpperBound());
//    } else {
//        LPERROR("Variable leaves the basis at value between lower at upper bound!");
//        LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getValue());
//        LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
//        LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
//        cerr.unsetf(ios_base::floatfield);
//        throw NumericalException("NUMERICAL problem");
//    }
//    m_basisHead.at(pivotRow) = incoming;
//    m_isFresh = false;
//}

//Vector & PfiBasis::Ftran(Vector & e) const {
//#ifndef NDEBUG
//    //In debug mode the dimensions of the basis and the given vector v are compared.
//    //If the dimension mismatches, then the operation cannot be performed.
//    //This can't happen in the "normal" case, so in release mode this check is unnecessary.
//    if (v.length() != m_basisHead.size()) {
//        LPERROR("FTRAN failed, vector dimension mismatch! ");
//        LPERROR("Dimension of the vector to be transformed: " << v.length());
//        LPERROR("Dimension of the basis: " << m_basisHead.size());
//        return v;
//    }
//#endif //!NDEBUG
//    //The linalg class implements the ftran operation.
//    Linalg::ftran(e, *m_basis);
//    return e;
//}

//Vector & PfiBasis::Btran(Vector & v, int suggestion) const
//{
//#ifndef NDEBUG
//    if (v.length() != m_basisHead.size()) {
//        LPERROR("BTRAN failed, vector dimension mismatch! ");
//        LPERROR("Dimension of the vector to be transformed: " << v.length());
//        LPERROR("Dimension of the basis: " << m_basisHead.size());
//        return v;
//    }
//#endif //!NDEBUG
//    //Vector v2 = v;
////    Vector v3( v.length() );
////    int pos = rand() % v3.length();
////    v3.set(pos, 1);
////    m_etaStorer->show();
//    //clock_t start = clock();
////        timeval start, end;
////        gettimeofday(&start, 0);
//        if (suggestion == 0) {
//                m_etaStorer->btran(v, *m_basis);
//        } else {
//            Linalg::btran(v, *m_basis);
//        }
//   //     Linalg::btran(v, *m_basis);
////    gettimeofday(&end, 0);
//    //clock_t end = clock();
////    LPINFO("---------------------------------------")

////    unsigned long long int delta = end.tv_sec*1000000 + end.tv_usec;
////    delta -= start.tv_sec*1000000 + start.tv_usec;
////    LPINFO("\trowwise btran: "<< delta );
////    std::cin.get();
///*    btranTime += delta;
//    if (actualPhase == 1) {
//        phaseOneBtranTime += delta;
//        if (isUpdatePhase) {
//            phaseOneUpdateBtranTime += delta;
//        }
//    } else {
//        phaseTwoBtranTime += delta;
//        if (isUpdatePhase) {
//            phaseTwoUpdateBtranTime += delta;
//        }
//    }*/
//    //LPINFO("\trowwise btran: "<< start.tv_sec <<" sec "<<start.tv_usec<<" usec ->"<< end.tv_sec <<" sec "<<end.tv_usec<<" usec");
//    //LPINFO("\tdelta: "<<delta<<" usec sum: "<<btranTime);
//    //std::cin.get();
////    double a1 = (end-start);
//////    m_etaStorer->btran(v3, *m_basis);
////    start = clock();
//      //Linalg::btran(v, *m_basis);
////        end = clock();
////    LPINFO("\tcolumnwise btran: "<< (end - start)/(double)CLOCKS_PER_SEC );
////    LPINFO("\tspeedup: "<< (end - start)/a1 );
////    if (v != v2) {
////        LPERROR("ELTERES VAN: " << v << v2);
////        std::cin.get();
////    }
//    return v;
//}

//void PfiBasis::symbolicBtran(std::vector<unsigned int> & v) const {
//    Linalg::symbolicBtran(v, *m_basis);
//}

//void PfiBasis::updateColumns(unsigned int rowindex, unsigned int columnindex) {
//    clock_t updateStart = clock();
//    Vector rowToUpdate = m_rows->at(rowindex);
//    for (Vector::NonzeroIterator it = rowToUpdate.beginNonzero(); it < rowToUpdate.endNonzero(); it++) {
//        if (it.getIndex() != columnindex) {
//            DEVINFO(D::PFIMAKER, "UPDATING column : " << it.getIndex());
//            Vector before = m_columns->at(it.getIndex());
//            m_transformationCount += m_basis->back().eta->nonZeros();
//            m_columns->at(it.getIndex()).elementaryFtran(*(m_basis->back().eta), m_basis->back().index);
//            //Update the corresponding row list values
//            int newColumnCount = 0;
//            for (Vector::NonzeroIterator vectorIt = m_columns->at(it.getIndex()).beginNonzero(); vectorIt
//                    < m_columns->at(it.getIndex()).endNonzero(); vectorIt++) {
//                if (m_rowCounts->at(vectorIt.getIndex()) >= 0) {
//                    if (m_rows->at(vectorIt.getIndex()).at(it.getIndex()) == 0) {
//                        m_rowCounts->at(vectorIt.getIndex())++;
//                    }
//                    m_rows->at(vectorIt.getIndex()).set(it.getIndex(), *vectorIt);
//                    newColumnCount++;
//                }
//            }
//            //Check the generated zeros

//            for (Vector::NonzeroIterator vectorIt = before.beginNonzero(); vectorIt < before.endNonzero(); vectorIt++) {
//                if (m_columns->at(it.getIndex()).at(vectorIt.getIndex()) == 0) {
//                    DEVINFO(D::PFIMAKER, "Zero generated in column " << it.getIndex() << " row index: " << vectorIt.getIndex());
//                    m_rows->at(vectorIt.getIndex()).set(it.getIndex(), 0);
//                    m_rowCounts->at(vectorIt.getIndex())--;
//                }
//            }
//            //Update the column counts
//            m_columnCounts->at(it.getIndex()) = newColumnCount;
//        }
//    }
//    clock_t updateStop = clock();
//    cl_updateColumns += updateStop - updateStart;
//}

//void PfiBasis::pivot(Vector* column, int pivotRow) throw (NumericalException) {
//    clock_t pivotStart = clock();
//    ETM newEtm;
//    m_invertedNonzeros += column->nonZeros();
//    if (column->nonZeros() == 1 && *(column->beginNonzero()) == 1) {
//        m_inverseNonzeros++;
//        return;
//    }
//    newEtm.eta = createEta(column, pivotRow);
//    newEtm.index = pivotRow;
//    m_inverseNonzeros += newEtm.eta->nonZeros();
//    m_basis->push_back(newEtm);
//        m_etaStorer->append( *newEtm.eta, pivotRow );
//    clock_t pivotStop = clock();
//    cl_pivot += pivotStop - pivotStart;
//}

//void PfiBasis::invertR() throw (NumericalException) {

//    //The upper triangular part is called R part
//    DEVINFO(D::PFIMAKER, "Search for the R part and invert it");
//    unsigned int rNum = 0;
//    switch (triangularMethod) {
//    case ITERATIONS:
//        //containsOne represents the exit variable
//        bool containsOne;
//        containsOne = true;
//        int r;
//        r = 0;
//        while (containsOne) {
//            DEVINFO(D::PFIMAKER, "ITERATION " << r);
//            containsOne = false;
//            //This part searches for rows with row count 1 and order them to the upper triangular part
//            for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
//                if (*it == 1) {
//                    int rowindex = it - m_rowCounts->begin();
//                    int columnindex = m_rows->at(rowindex).beginNonzero().getIndex();
//                    Vector *currentColumn = &(m_columns->at(columnindex));
//                    if (triangularCheck(rowindex, currentColumn)) {
//                        //Invert the chosen R column
//                        DEVINFO(D::PFIMAKER, "Inverting R column " << columnindex << " with pivot row " << rowindex);

//                        //                            { ofstream ofs("update.txt", std::ios_base::app);
//                        //                            ofs << "pivot1" << std::endl;
//                        //                            ofs.close(); }

//                        pivot(currentColumn, rowindex);
//                        m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                    } else {
//                        LPWARNING("Triangular pivot position is numerically unstable, ignoring nonzero position:" <<
//                                "(row:" << rowindex << " column:" << columnindex << ")");
//                    }
//                    //Update the row lists and row counts
//                    for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                            < currentColumn->endNonzero(); vectorIt++) {
//                        m_rowCounts->at(vectorIt.getIndex())--;
//                        m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                    }
//                    //Set the column count to zero to represent that which column has been chosen.
//                    m_columnCounts->at(columnindex) = -1;
//                    m_rowCounts->at(rowindex) = -1;

//                    containsOne = true;
//                    rNum++;
//                }
//            }
//            r++;
//        }
//        break;
//    case LINKS:
//        if (m_rowLinks->getPartitionCount() > 1) {
//            while (m_rowLinks->begin(1) != m_rowLinks->end(1)) {
//                //This part searches for rows with row count 1 and order them to the upper triangular part
//                int rowindex = m_rowLinks->begin(1).getData();
//                m_rowLinks->remove(rowindex);
//                int columnindex = m_rows->at(rowindex).beginNonzero().getIndex();
//                //Column links are built later on
//                //				m_columnLinks->remove(columnindex);
//                Vector *currentColumn = &(m_columns->at(columnindex));
//                if (triangularCheck(rowindex, currentColumn)) {
//                    //Invert the chosen R column
//                    DEVINFO(D::PFIMAKER, "Inverting R column " << columnindex << " with pivot row " << rowindex);
//                    pivot(currentColumn, rowindex);
//                    m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                } else {
//                    LPWARNING("Triangular pivot position is numerically unstable, ignoring nonzero position:" <<
//                            "(row:" << rowindex << " column:" << columnindex << ")");
//                }
//                //Update the row lists and row counts
//                for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                        < currentColumn->endNonzero(); vectorIt++) {
//                    int index = vectorIt.getIndex();
//                    m_rowCounts->at(index)--;
//                    //If the row of the iterated element is still active
//                    if (index != rowindex) {
//                        m_rowLinks->remove(index);
//                        m_rowLinks->insert(m_rowCounts->at(index), index);
//                        m_rows->at(index).set(columnindex, 0);
//                    }
//                }
//                //Set the column count to zero to represent that which column has been chosen.
//                m_columnCounts->at(columnindex) = -1;
//                m_rowCounts->at(rowindex) = -1;
//#ifndef NDEBUG
//                printCounts();
//                printRowLinks();
//#endif //!NDEBUG
//                rNum++;
//            }
//        }
//        break;
//    default:
//        LPWARNING("No triangular method defined!")
//        ;
//        break;
//    }
//    DEVINFO(D::PFIMAKER, "RPART num: " << rNum);
//}

//void PfiBasis::findC() {
//    //The lower triangular part is called C part
//    DEVINFO(D::PFIMAKER, "Search for C part");
//    unsigned int cNum = 0;
//    switch (triangularMethod) {
//    case ITERATIONS:
//        //containsOne represents the exit variable
//        bool containsOne;
//        containsOne = true;
//        while (containsOne) {
//            containsOne = false;
//            //This part searches for columns with column count 1 and order them to the lower triangular part
//            for (std::vector<int>::iterator it = m_columnCounts->begin(); it < m_columnCounts->end(); it++) {
//                if (*it == 1) {
//                    int columnindex = it - m_columnCounts->begin();
//                    int rowindex = m_columns->at(columnindex).beginNonzero().getIndex();
//                    for (Vector::NonzeroIterator vectorIt = m_columns->at(columnindex).beginNonzero(); vectorIt
//                            < m_columns->at(columnindex).endNonzero(); vectorIt++) {
//                        if (m_rowCounts->at(vectorIt.getIndex()) > 0) {
//                            rowindex = vectorIt.getIndex();
//                            break;
//                        }
//                    }
//                    Vector *currentColumn = &(m_columns->at(columnindex));
//                    if (triangularCheck(rowindex, currentColumn)) {
//                        //Store the chosen columns in reverse orders
//                        m_cColumns->push_back(currentColumn);
//                        m_cPivotIndexes->push_back(rowindex);
//                        m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                        DEVINFO(D::PFIMAKER, "Submatrix column " << columnindex << " stored for C part with pivot index " << rowindex);
//                    } else {
//                        LPWARNING("Triangular pivot position is numerically unstable, ignoring nonzero position:" <<
//                                "(row:" << rowindex << " column:" << columnindex << ")");
//                        LPWARNING("norm: " << currentColumn->absMaxElement() << " value: " << currentColumn->at(rowindex));
//                    }
//                    //Update the column counts but the columns remains untouched
//                    for (Vector::NonzeroIterator vectorIt = m_rows->at(rowindex).beginNonzero(); vectorIt < m_rows->at(
//                            rowindex).endNonzero(); vectorIt++) {
//                        m_columnCounts->at(vectorIt.getIndex())--;
//                    }
//                    //Delete the chosen row list
//                    m_rows->at(rowindex).clear();
//                    //Set the row count set to zero to represent that which row has been chosen.
//                    m_rowCounts->at(rowindex) = -1;
//                    m_columnCounts->at(columnindex) = -1;
//#ifndef NDEBUG
//                    printCounts();
//#endif //!NDEBUG
//                    containsOne = true;
//                    cNum++;
//                }
//            }
//        }
//        break;
//    case LINKS:
//        if (m_columnLinks->getPartitionCount() > 0) {
//            while (m_columnLinks->begin(1) != m_columnLinks->end(1)) {
//                //This part searches for rows with row count 1 and order them to the upper triangular part
//                int columnindex = m_columnLinks->begin(1).getData();
//                m_columnLinks->remove(columnindex);
//                int rowindex = m_columns->at(columnindex).beginNonzero().getIndex();
//                for (Vector::NonzeroIterator vectorIt = m_columns->at(columnindex).beginNonzero(); vectorIt
//                        < m_columns->at(columnindex).endNonzero(); vectorIt++) {
//                    if (m_rowCounts->at(vectorIt.getIndex()) > 0) {
//                        rowindex = vectorIt.getIndex();
//                        break;
//                    }
//                }
//                m_rowLinks->remove(rowindex);
//                Vector *currentColumn = &(m_columns->at(columnindex));
//                if (triangularCheck(rowindex, currentColumn)) {
//                    //Store the chosen columns
//                    m_cColumns->push_back(currentColumn);
//                    m_cPivotIndexes->push_back(rowindex);
//                    m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                    DEVINFO(D::PFIMAKER, "Submatrix column " << columnindex << " stored for C part with pivot index " << rowindex);
//                } else {
//                    LPWARNING("Triangular pivot position is numerically unstable, ignoring nonzero position:" <<
//                            "(row:" << rowindex << " column:" << columnindex << ")");
//                }
//                //Update the column counts but the columns remains untouched
//                for (Vector::NonzeroIterator vectorIt = m_rows->at(rowindex).beginNonzero(); vectorIt < m_rows->at(
//                        rowindex).endNonzero(); vectorIt++) {
//                    int index = vectorIt.getIndex();
//                    m_columnCounts->at(index)--;
//                    if (index != columnindex) {
//                        m_columnLinks->remove(index);
//                        m_columnLinks->insert(m_columnCounts->at(index), index);
//                    }
//                }
//                //Delete the chosen row list
//                m_rows->at(rowindex).clear();
//                //Set the row count set to zero to represent that which row has been chosen.
//                m_rowCounts->at(rowindex) = -1;
//                m_columnCounts->at(columnindex) = -1;

//#ifndef NDEBUG
//                printCounts();
//                printColumnLinks();
//#endif //!NDEBUG
//                cNum++;
//            }
//        }
//        break;
//    default:
//        LPWARNING("No triangular method defined!")
//        ;
//        break;
//    }
//    DEVINFO(D::PFIMAKER, "CPART num: " << cNum);
//}

//void PfiBasis::invertM() throw (NumericalException) {
//    //The middle (non-triangular) part is called M part
//    DEVINFO(D::PFIMAKER, "Organize the M part and invert the columns");
//    unsigned int mNum = 0;
//    switch (nontriangularMethod) {
//    case SEARCH:
//        //containsOne represents the exit variable
//        bool containsOne;
//        containsOne = true;
//        while (containsOne) {
//            containsOne = false;
//            for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
//                if (*it > 0) {
//                    DEVINFO(D::PFIMAKER, "Choosing M column with rowcount " << *it);
//                    int rowindex = it - m_rowCounts->begin();
//                    int columnindex = m_rows->at(rowindex).beginNonzero().getIndex();
//                    Vector *currentColumn = &(m_columns->at(columnindex));
//                    if (nontriangularCheck(rowindex, currentColumn, -1)) {
//                        Vector row = m_rows->at(rowindex);
//                        //Invert the chosen M column
//                        DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
//                        //                            { ofstream ofs("update.txt", std::ios_base::app);
//                        //                            ofs << "pivot3" << std::endl;
//                        //                            ofs.close(); }

//                        pivot(currentColumn, rowindex);
//                        containsOne = true;
//                        m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                        //Update the remaining columns
//                        updateColumns(rowindex, columnindex);
//                        //Update the row lists and row counts
//                        for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                                < currentColumn->endNonzero(); vectorIt++) {
//                            if (m_rowCounts->at(vectorIt.getIndex()) >= 0) {
//                                m_rowCounts->at(vectorIt.getIndex())--;
//                                m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                            }
//                        }
//                        //Update the column counts too
//                        for (Vector::NonzeroIterator vectorIt = m_rows->at(rowindex).beginNonzero(); vectorIt
//                                < m_rows->at(rowindex).endNonzero(); vectorIt++) {
//                            m_columnCounts->at(vectorIt.getIndex())--;
//                        }
//                        //Delete the chosen row list
//                        while (m_rows->at(rowindex).nonZeros() > 0) {
//                            m_rows->at(rowindex).set(m_rows->at(rowindex).beginNonzero().getIndex(), 0);
//                        }
//                        //Set the column and row count to zero to represent that which column and row has been chosen.
//                        m_rowCounts->at(rowindex) = -1;
//                        m_columnCounts->at(columnindex) = -1;
//                    } else {
//                        LPWARNING("Non-triangular pivot position is numerically unstable, ignoring nonzero position:" <<
//                                "(row:" << rowindex << " column:" << columnindex << ")");
//                        m_rowCounts->at(it - m_rowCounts->begin())--;
//                        m_columnCounts->at(columnindex)--;
//                        m_rows->at(it - m_rowCounts->begin()).set(columnindex, 0);
//                    }
//#ifndef NDEBUG
//                    printCounts();
//#endif //!NDEBUG
//                    mNum++;
//                }
//            }
//        }
//        break;
//    case BLOCK_TRIANGULAR:{
//        clock_t createBlockTriangularStart = clock();
//        createBlockTriangular();
//        clock_t createBlockTriangularStop = clock();
//        cl_createBlockTriangular += createBlockTriangularStop - createBlockTriangularStart;
//        if (m_mmBlocks->size() > 0) {
//            int currentBlock = -1;
//            int currentBlockSize = 0;
//            for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {
//                if (currentBlockSize == 0) {
//                    currentBlock++;
//                    currentBlockSize = m_mmBlocks->at(currentBlock);
//                    //LPINFO("Iterating block #"<<currentBlock<<" with block size: "<<currentBlockSize);
//                }
//                currentBlockSize--;
//                int rowindex = m_mmRowIndices->at(*rowIt);
//                int columnindex = m_mmColumnIndices->at(m_columnSwapHash->at(rowIt - m_rowSwapHash->begin()));
//                Vector* currentColumn = &(m_columns->at(columnindex));
//                if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
//                    if (m_columns->at(columnindex).at(rowindex) != 0) {
//                        DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
//                        //Invert the chosen M column
//                        //                            { ofstream ofs("update.txt", std::ios_base::app);
//                        //                            ofs << "pivot4, " << columnindex << "   " << *rowIt << std::endl;
//                        //                            ofs.close(); }

//                        pivot(currentColumn, rowindex);
//                        m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                        //Update the remaining columns
//                        updateColumns(rowindex, columnindex);
//                    } else {
//                        DEVINFO(D::PFIMAKER, "M column " << columnindex << " is singular with pivot row " << rowindex);
//                    }
//                    //Update the row lists and row counts
//                    for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                            < currentColumn->endNonzero(); vectorIt++) {
//                        if (m_rows->at(vectorIt.getIndex()).at(columnindex) != 0) {
//                            m_rowCounts->at(vectorIt.getIndex())--;
//                            m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                        }
//                    }
//                    //Update the column counts too
//                    for (Vector::NonzeroIterator vectorIt = m_rows->at(rowindex).beginNonzero(); vectorIt < m_rows->at(
//                            rowindex).endNonzero(); vectorIt++) {
//                        m_columnCounts->at(vectorIt.getIndex())--;
//                    }
//                    //Delete the chosen row list
//                    while (m_rows->at(rowindex).nonZeros() > 0) {
//                        m_rows->at(rowindex).set(m_rows->at(rowindex).beginNonzero().getIndex(), 0);
//                    }
//                    //Set the column and row count to zero to represent that which column and row has been chosen.
//                    m_rowCounts->at(rowindex) = -1;
//                    m_columnCounts->at(columnindex) = -1;
//                } else {
//                    LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" <<
//                            columnindex << ")");
//                    //Update the row lists and row counts
//                    for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                            < currentColumn->endNonzero(); vectorIt++) {
//                        if (m_rows->at(vectorIt.getIndex()).at(columnindex) != 0) {
//                            m_rowCounts->at(vectorIt.getIndex())--;
//                            m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                        }
//                    }
//                    //Set the column count to zero to represent that which column is unstable.
//                    m_columnCounts->at(columnindex) = -1;
//                }
//                mNum++;
//            }
//        }

//        break;
//    }
//    case BLOCK_ORDERED_TRIANGULAR:{
//        clock_t createBlockTriangularStart = clock();
//        createBlockTriangular();
//        clock_t createBlockTriangularStop = clock();
//        cl_createBlockTriangular += createBlockTriangularStop - createBlockTriangularStart;
//        {
//            int blockStart = 0;
//            for (std::vector<int>::iterator blockIt = m_mmBlocks->begin(); blockIt < m_mmBlocks->end(); blockIt++) {
//                int currentBlock = blockIt - m_mmBlocks->begin();
//                int currentBlockSize = *blockIt;
//                std::multimap<int, int> columnOrders;
//                int i;
//                for (i = blockStart; i < blockStart + currentBlockSize; i++) {
//                    columnOrders.insert(
//                            std::make_pair((int) m_mmColumns->at(i).nonZeros(),
//                                    m_mmColumnIndices->at(m_columnSwapHash->at(i))));
//                }
//                for (std::multimap<int, int>::iterator colIt = columnOrders.begin(); colIt != columnOrders.end(); colIt++) {
//                    blockStart++;
//                    int rowindex = -1;
//                    int columnindex = colIt->second;
//                    Vector* currentColumn = &(m_columns->at(columnindex));
//                    if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
//                        if (m_columns->at(columnindex).at(rowindex) != 0) {
//                            DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
//                            //Invert the chosen M column
//                            pivot(currentColumn, rowindex);
//                            m_basisNewHead.at(rowindex) = m_columnsHash->at(columnindex);
//                            //Update the remaining columns
//                            updateColumns(rowindex, columnindex);
//                        } else {
//                            DEVINFO(D::PFIMAKER, "M column " << columnindex << " is singular with pivot row " << rowindex);
//                        }
//                        //Update the row lists and row counts
//                        for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                                < currentColumn->endNonzero(); vectorIt++) {
//                            if (m_rows->at(vectorIt.getIndex()).at(columnindex) != 0) {
//                                m_rowCounts->at(vectorIt.getIndex())--;
//                                m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                            }
//                        }
//                        //Update the column counts too
//                        for (Vector::NonzeroIterator vectorIt = m_rows->at(rowindex).beginNonzero(); vectorIt
//                                < m_rows->at(rowindex).endNonzero(); vectorIt++) {
//                            m_columnCounts->at(vectorIt.getIndex())--;
//                        }
//                        //Delete the chosen row list
//                        while (m_rows->at(rowindex).nonZeros() > 0) {
//                            m_rows->at(rowindex).set(m_rows->at(rowindex).beginNonzero().getIndex(), 0);
//                        }
//                        //Set the column and row count to zero to represent that which column and row has been chosen.
//                        m_rowCounts->at(rowindex) = -1;
//                        m_columnCounts->at(columnindex) = -1;
//                    } else {
//                        LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" <<
//                                columnindex << ")");
//                        //Update the row lists and row counts
//                        for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                                < currentColumn->endNonzero(); vectorIt++) {
//                            if (m_rows->at(vectorIt.getIndex()).at(columnindex) != 0) {
//                                m_rowCounts->at(vectorIt.getIndex())--;
//                                m_rows->at(vectorIt.getIndex()).set(columnindex, 0);
//                            }
//                        }
//                        //Set the column count to zero to represent that which column is unstable.
//                        m_columnCounts->at(columnindex) = -1;
//                    }
//                    mNum++;
//                }
//            }
//        }

//        break;
//    }
//    default:
//        LPWARNING("No triangulatrization method defined!")
//        ;
//        break;
//    }
//    DEVINFO(D::PFIMAKER, "MPART num: " << mNum);
//}

//void PfiBasis::invertC() throw (NumericalException) {
//    //The lower triangular part is called C part
//    DEVINFO(D::PFIMAKER, "Invert the C part");
//    for (std::vector<Vector*>::reverse_iterator it = m_cColumns->rbegin(); it < m_cColumns->rend(); it++) {
//        DEVINFO(D::PFIMAKER, "Inverting C column " << m_cColumns->rend() - 1 - it << " with pivot row " << m_cPivotIndexes->at(m_cColumns->rend() - 1 - it));
//        //                            { ofstream ofs("update.txt", std::ios_base::app);
//        //                            ofs << "pivot5" << std::endl;
//        //                            ofs.close(); }

//        pivot(*it, m_cPivotIndexes->at(m_cColumns->rend() - 1 - it));
//    }
//    return;
//}

//void PfiBasis::buildMM() {
//    //Reset the data structures used for processing the non-triangular part
//    m_mmRowIndices->clear();
//    m_mmRows->clear();
//    m_mmColumnIndices->clear();

//    m_mmColumns->clear();
//    m_rowSwapHash->clear();
//    m_columnSwapHash->clear();
//    m_columnSwapLog->clear();
//    m_mmGraphOut->clear();
//    m_mmGraphIn->clear();
//    m_stack->clear();
//    m_orderedIndex->clear();
//    m_mmBlocks->clear();

//    //Collect the indices od
//    for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
//        if (*it >= 0) {
//            m_mmRowIndices->push_back(it - m_rowCounts->begin());
//        }
//    }
//    std::vector<int> columnMemory = std::vector<int>(m_columnCounts->size(), -1);
//    for (std::vector<int>::iterator it = m_columnCounts->begin(); it < m_columnCounts->end(); it++) {
//        if (*it >= 0) {
//            m_mmColumnIndices->push_back(it - m_columnCounts->begin());
//            columnMemory.at(it - m_columnCounts->begin()) = m_mmColumnIndices->size() - 1;
//        }
//    }
//    if (m_mmRowIndices->size() != m_mmColumnIndices->size()) {
//        LPERROR("Non-triangular part inversion failed!");
//        LPERROR("The numbers of rows and columns with nonnegative counters are different!");
//        return;
//    }
//    int mmSize = m_mmRowIndices->size();
//    //Compute the average size
//    m_mNumAverage += (mmSize - m_mNumAverage) / m_inversionCount;

//    m_mmRows->resize(mmSize, Vector(mmSize));
//    m_mmColumns->resize(mmSize, Vector(mmSize));
//    m_rowSwapHash->resize(mmSize, -1);
//    m_columnSwapHash->resize(mmSize, -1);
//    m_columnSwapLog->resize(mmSize, -1);
//    m_orderedIndex->resize(mmSize, false);
//    for (int i = 0; i < mmSize; i++) {
//        m_rowSwapHash->at(i) = i;
//        m_columnSwapHash->at(i) = i;
//        m_columnSwapLog->at(i) = i;
//        //     m_mmRows->at(i).setSparsityRatio(0.0);
//        //   m_mmColumns->at(i).setSparsityRatio(0.0);
//    }
//    for (std::vector<int>::iterator it = m_mmRowIndices->begin(); it < m_mmRowIndices->end(); it++) {
//        for (Vector::NonzeroIterator vectorIt = m_rows->at(*it).beginNonzero(); vectorIt < m_rows->at(*it).endNonzero(); vectorIt++) {
//            if (columnMemory.at(vectorIt.getIndex()) == -1) {
//                LPERROR("Error happened during M part extraction!");
//            }
//            int columnIndex = columnMemory.at(vectorIt.getIndex());
//            int rowIndex = it - m_mmRowIndices->begin();
//            m_mmRows->at(rowIndex).set(columnIndex, *vectorIt);
//            m_mmColumns->at(columnIndex).set(rowIndex, *vectorIt);
//        }
//    }
//    DEVINFO(D::PFIMAKER, "Nonzero pattern in MM built");
//#ifndef NDEBUG
//    printMM();
//#endif //!NDEBUG
//}

//void PfiBasis::findTransversal() {
//    DEVINFO(D::PFIMAKER, "Searching for transversal form");
//    for (int i = 0; i < (int) m_rowSwapHash->size(); i++) {
//        if (Numerical::equals(m_mmRows->at(i).at(i), 0)) {
//            //Find some with bigger index
//            bool nextRow = false;
//            DEVINFO(D::PFIMAKER, "Searching for diagonal nonzero at row " << i);
//            for (Vector::NonzeroIterator vectorIt = m_mmColumns->at(i).beginNonzero(); vectorIt
//                    < m_mmColumns->at(i).endNonzero(); vectorIt++) {
//                if ((int) vectorIt.getIndex() > i) {
//                    nextRow = true;
//                    DEVINFO(D::PFIMAKER, "Nonzero found below index " << i);
//                    swapRows(vectorIt.getIndex(), i);
//                    break;
//                } else if (Numerical::equals(m_mmRows->at(vectorIt.getIndex()).at(vectorIt.getIndex()), 0)) {
//                    nextRow = true;
//                    DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << i);
//                    swapRows(vectorIt.getIndex(), i);
//                    break;
//                }
//            }
//            if (!nextRow) {
//                DEVINFO(D::PFIMAKER, "Nonzero not found, search columns");
//                std::vector<int> searchedRows;
//                for (Vector::NonzeroIterator vectorIt = m_mmColumns->at(i).beginNonzero(); vectorIt
//                        < m_mmColumns->at(i).endNonzero(); vectorIt++) {
//                    int searchResult;
//                    searchedRows.push_back(vectorIt.getIndex());
//                    searchResult = searchColumn(vectorIt.getIndex(), i, searchedRows);

//                    DEVINFO(D::PFIMAKER, "search result: " << searchResult);
//                    if (searchResult > -1) {
//                        if (searchResult != i) {
//                            swapRows(searchResult, i);
//                        }
//                        break;
//                    }
//                }
//            }
//        }
//    }
//#ifndef NDEBUG
//    DEVINFO(D::PFIMAKER, "Nonzero pattern with transversal ");
//    printMM();
//#endif //!NDEBUG
//}

//void PfiBasis::swapRows(int rowIndex1, int rowIndex2) {
//    DEVINFO(D::PFIMAKER, "Swapping row " << rowIndex1 << " and " << rowIndex2);

//    int rowHashIndex1 = m_rowSwapHash->at(rowIndex1);
//    int rowHashIndex2 = m_rowSwapHash->at(rowIndex2);
//    Vector row1 = m_mmRows->at(rowIndex1);
//    Vector row2 = m_mmRows->at(rowIndex2);

//    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
//        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex1, 0);
//    }
//    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
//        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex2, 0);
//    }
//    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
//        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex2, *vectorIt);
//    }
//    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
//        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex1, *vectorIt);
//    }
//    m_mmRows->at(rowIndex1) = row2;
//    m_mmRows->at(rowIndex2) = row1;

//    m_rowSwapHash->at(rowIndex1) = rowHashIndex2;
//    m_rowSwapHash->at(rowIndex2) = rowHashIndex1;
//    //printSwapHashes();
//}

//void PfiBasis::swapColumns(int columnIndex1, int columnIndex2) {
//    DEVINFO(D::PFIMAKER, "Swapping column " << columnIndex1 << " and " << columnIndex2);

//    int columnHashIndex1 = m_columnSwapHash->at(columnIndex1);
//    int columnHashIndex2 = m_columnSwapHash->at(columnIndex2);
//    Vector column1 = m_mmColumns->at(columnIndex1);
//    Vector column2 = m_mmColumns->at(columnIndex2);

//    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
//        m_mmRows->at(vectorIt.getIndex()).set(columnIndex1, 0);
//    }
//    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
//        m_mmRows->at(vectorIt.getIndex()).set(columnIndex2, 0);
//    }
//    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
//        m_mmRows->at(vectorIt.getIndex()).set(columnIndex2, *vectorIt);
//    }
//    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
//        m_mmRows->at(vectorIt.getIndex()).set(columnIndex1, *vectorIt);
//    }
//    m_mmColumns->at(columnIndex1) = column2;
//    m_mmColumns->at(columnIndex2) = column1;

//    m_columnSwapLog->at(columnHashIndex1) = columnIndex2;
//    m_columnSwapLog->at(columnHashIndex2) = columnIndex1;

//    m_columnSwapHash->at(columnIndex1) = columnHashIndex2;
//    m_columnSwapHash->at(columnIndex2) = columnHashIndex1;
//    //printSwapHashes();
//}

//int PfiBasis::searchColumn(int columnIndex, int searchIndex, std::vector<int>& searchedRows) {
//    DEVINFO(D::PFIMAKER, "Searching column " << columnIndex << " for nonzero with searchindex " << searchIndex);
//    for (Vector::NonzeroIterator vectorIt = m_mmColumns->at(columnIndex).beginNonzero(); vectorIt < m_mmColumns->at(
//            columnIndex).endNonzero(); vectorIt++) {
//        if ((int) vectorIt.getIndex() >= searchIndex) {
//            DEVINFO(D::PFIMAKER, "Nonzero found below index " << searchIndex);
//            int searchResult = vectorIt.getIndex();
//            swapRows(columnIndex, searchResult);
//            return searchResult;
//        } else if (Numerical::equals(m_mmRows->at(vectorIt.getIndex()).at(vectorIt.getIndex()), 0)) {
//            DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << searchIndex);
//            int searchResult = vectorIt.getIndex();
//            swapRows(columnIndex, searchResult);
//            return searchResult;
//        }
//    }
//    for (Vector::NonzeroIterator vectorIt = m_mmColumns->at(columnIndex).beginNonzero(); vectorIt < m_mmColumns->at(
//            columnIndex).endNonzero(); vectorIt++) {
//        bool contains = false;
//        for (std::vector<int>::iterator it = searchedRows.begin(); it < searchedRows.end(); it++) {
//            if (*it == (int) vectorIt.getIndex()) {
//                contains = true;
//            }
//        }
//        if (contains) {
//            continue;
//        }
//        searchedRows.push_back(vectorIt.getIndex());
//        int searchResult = searchColumn(vectorIt.getIndex(), searchIndex, searchedRows);
//        if (searchResult > -1) {
//            swapRows(columnIndex, searchResult);
//            return searchResult;
//        }
//    }
//    return -1;
//}

//void PfiBasis::createGraph() {
//    m_mmGraphOut->resize(m_mmRows->size(), std::vector<int>());
//    m_mmGraphIn->resize(m_mmRows->size(), std::vector<int>());
//    for (std::vector<Vector>::iterator it = m_mmRows->begin(); it < m_mmRows->end(); it++) {
//        for (Vector::NonzeroIterator vectorIt = it->beginNonzero(); vectorIt < it->endNonzero(); vectorIt++) {
//            if ((int) vectorIt.getIndex() != it - m_mmRows->begin()) {
//                m_mmGraphOut->at(it - m_mmRows->begin()).push_back(vectorIt.getIndex());
//                m_mmGraphIn->at(vectorIt.getIndex()).push_back(it - m_mmRows->begin());
//            }
//        }
//    }
//#ifndef NDEBUG
//    printGraph();
//#endif //!NDEBUG
//}

//void PfiBasis::tarjan() {
//    DEVINFO(D::PFIMAKER, "Tarjan begin");
//    for (std::vector<bool>::iterator it = m_orderedIndex->begin(); it < m_orderedIndex->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Tarjan step" << it - m_orderedIndex->begin());
//        if (*it == false) {
//            PathNode node;
//            node.index = it - m_orderedIndex->begin();
//            node.lowest = node.index;
//            node.nextEdge = 0;
//            m_stack->push_back(node);
//            searchNode();
//        }
//    }
//}

//int PfiBasis::searchNode() {
//#ifndef NDEBUG
//    printStack();
//#endif //!NDEBUG
//    PathNode currentNode = m_stack->back();
//    int stackPosition = m_stack->size() - 1;
//    int nextLowest = -1;
//    DEVINFO(D::PFIMAKER, "Searching node " << currentNode.index);
//    std::vector<int> outGoing = m_mmGraphOut->at(currentNode.index);
//    while ((int) outGoing.size() > currentNode.nextEdge) {
//        if (m_mmGraphIn->at(outGoing.at(currentNode.nextEdge)).size() > 0) {
//            int next = outGoing.at(currentNode.nextEdge);
//            DEVINFO(D::PFIMAKER, "Searching edge " << next);
//            bool newNode = true;
//            for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
//                if (it->index == next) {
//                    newNode = false;
//                    break;
//                }
//            }
//            if (newNode) {
//                DEVINFO(D::PFIMAKER, "Edge points to a new node, create it on the stack");
//                PathNode node;
//                node.index = next;
//                node.lowest = node.index;
//                node.nextEdge = 0;
//                m_stack->push_back(node);
//                nextLowest = searchNode();
//                if (nextLowest != -1) {
//                    for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
//                        if (it->index == currentNode.lowest) {
//                            break;
//                        }
//                        if (it->index == nextLowest) {
//                            currentNode.lowest = nextLowest;
//                            m_stack->at(stackPosition).lowest = nextLowest;
//                            break;
//                        }
//                    }
//                }
//            } else {
//                for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
//                    if (it->index == next) {
//                        DEVINFO(D::PFIMAKER, "Setting current lowest to next " << next);
//                        currentNode.lowest = it->lowest;
//                        m_stack->at(stackPosition).lowest = it->lowest;
//#ifndef NDEBUG
//                        printStack();
//#endif //!NDEBUG
//                        break;
//                    }
//                    if (it->index == currentNode.lowest) {
//                        break;
//                    }
//                }
//            }
//        }
//        currentNode.nextEdge++;
//        m_stack->at(stackPosition).nextEdge++;
//    }
//    if (currentNode.index == currentNode.lowest) {
//        DEVINFO(D::PFIMAKER, "Creating block #" << m_mmBlocks->size());
//        int allBlocks = 0;
//        for (std::vector<int>::iterator it = m_mmBlocks->begin(); it < m_mmBlocks->end(); it++) {
//            allBlocks += *it;
//        }
//        m_mmBlocks->push_back(0);
//        int lastIndex;
//        do {
//            lastIndex = m_stack->back().index;
//            for (std::vector<int>::iterator nodeIt = m_mmGraphIn->at(lastIndex).begin(); nodeIt < m_mmGraphIn->at(
//                    lastIndex).end(); nodeIt++) {
//                DEVINFO(D::PFIMAKER, "Searching node " << *nodeIt);
//                for (std::vector<int>::iterator searchIt = m_mmGraphOut->at(*nodeIt).begin(); searchIt
//                        < m_mmGraphOut->at(*nodeIt).end(); searchIt++) {
//                    DEVINFO(D::PFIMAKER, "Outgoing edge " << *searchIt);
//                    if (*searchIt == lastIndex) {
//                        m_mmGraphOut->at(*nodeIt).erase(searchIt);
//                        break;
//                    }
//                }
//            }
//            m_mmGraphIn->at(lastIndex).clear();
//            DEVINFO(D::PFIMAKER, "Last index in the stack is " << lastIndex);
//            m_orderedIndex->at(lastIndex) = true;
//            swapRows(m_columnSwapLog->at(lastIndex), allBlocks);
//            swapColumns(m_columnSwapLog->at(lastIndex), allBlocks);
//            m_mmBlocks->back()++;
//            allBlocks++;
//            DEVINFO(D::PFIMAKER, "Block " << m_mmBlocks->size() - 1 << " now contains node " << lastIndex);
//            m_stack->pop_back();
//#ifndef NDEBUG
//            printStack();
//#endif //!NDEBUG
//        } while (lastIndex != currentNode.index);
//    } else {
//        DEVINFO(D::PFIMAKER, "Node #" << currentNode.index << " is returning with " << currentNode.lowest);
//        return currentNode.lowest;
//    }
//    return -1;
//}

//void PfiBasis::createBlockTriangular() {
//    buildMM();

//    clock_t transversalStart = clock();
//    findTransversal();
//    clock_t transversalStop = clock();
//    cl_transversal += transversalStop - transversalStart;
//    createGraph();
//    tarjan();
//#ifndef NDEBUG
//    printMM();
//    printBlocks();
//#endif //!NDEBUG
//}

//bool PfiBasis::triangularCheck(int rowindex, Vector* currentColumn) {

//    switch (triangularPivotRule) {
//    case ABSOLUTE_TOLERANCE:
//        return Numerical::isZero(currentColumn->at(rowindex)) ? false : true;
//        break;
//    case RELATIVE_TOLERANCE:
//        if (currentColumn->at(rowindex) > 0) {
//            return currentColumn->at(rowindex) < currentColumn->absMaxElement() * e_relative ? false : true;
//        } else {
//            return -(currentColumn->at(rowindex)) < currentColumn->absMaxElement() * e_relative ? false : true;
//        }
//        break;
//    default:
//        return true;
//    }

//    return true;
//}

//bool PfiBasis::nontriangularCheck(int& rowindex, Vector* currentColumn, int blockNum) {
//    std::vector<int> activeRows;
//    std::vector<int> goodRows;
//    Numerical::Double nontriangularMax = 0;
//    int previousBlocks = 0;
//    switch (nontriangularPivotRule) {
//    case NONE:
//        if (Numerical::isZero(currentColumn->at(rowindex))) {
//            rowindex = -1;
//            return false;
//        } else {
//            return true;
//        }
//        break;
//    case THRESHOLD:
//        if (blockNum != -1) {
//            for (int i = 0; i < blockNum; i++) {
//                previousBlocks += m_mmBlocks->at(i);
//            }
//            for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                    < currentColumn->endNonzero(); vectorIt++) {
//                if (m_rowCounts->at(vectorIt.getIndex()) > -1) {
//                    for (int i = previousBlocks; i < previousBlocks + m_mmBlocks->at(blockNum); i++) {
//                        if ((int) vectorIt.getIndex() == m_mmRowIndices->at(m_rowSwapHash->at(i))) {
//                            activeRows.push_back(vectorIt.getIndex());
//                            break;
//                        }
//                    }
//                }
//            }
//        } else {
//            for (Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero(); vectorIt
//                    < currentColumn->endNonzero(); vectorIt++) {
//                if (m_rowCounts->at(vectorIt.getIndex()) > -1) {
//                    activeRows.push_back(vectorIt.getIndex());
//                }
//            }
//        }
//        for (std::vector<int>::iterator it = activeRows.begin(); it < activeRows.end(); it++) {
//            if (currentColumn->at(*it) > 0) {
//                if (currentColumn->at(*it) > nontriangularMax) {
//                    nontriangularMax = currentColumn->at(*it);
//                }
//            } else {
//                if (-(currentColumn->at(*it)) > nontriangularMax) {
//                    nontriangularMax = -(currentColumn->at(*it));
//                }
//            }
//        }
//        //				LPWARNING("nontriangularmax:"<<nontriangularMax);
//        for (std::vector<int>::iterator it = activeRows.begin(); it < activeRows.end(); it++) {
//            //			LPWARNING("ACTIVE: " << currentColumn->at(*it));
//            if (currentColumn->at(*it) > 0) {
//                if (currentColumn->at(*it) > threshold * nontriangularMax) {
//                    goodRows.push_back(*it);
//                }
//            } else {
//                if (-(currentColumn->at(*it)) > threshold * nontriangularMax) {
//                    goodRows.push_back(*it);
//                }
//            }
//        }
//        //		LPWARNING("GOOD size: " << goodRows.size());
//        if (goodRows.size() > 0) {
//            rowindex = goodRows[0];
//            for (std::vector<int>::iterator it = goodRows.begin(); it < goodRows.end(); it++) {
//                if (m_rowCounts->at(*it) < m_rowCounts->at(rowindex)) {
//                    rowindex = *it;
//                }
//            }
//            //			LPINFO("chosen row: "<<rowindex<<" with value: "<<currentColumn->at(rowindex));
//            return true;
//        } else {
//            //			LPWARNING("ERROR -1");
//            rowindex = -1;
//            return false;
//        }
//        break;
//    default:
//        return true;
//    }
//    return true;
//}

//void PfiBasis::printCounts() const {
//#ifndef NDEBUG
//    DEVINFO(D::PFIMAKER, "Row counts: ");
//    for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Row " << it - m_rowCounts->begin() << " has " << *it << " nonzeros");
//    }
//    DEVINFO(D::PFIMAKER, "Column counts: ");
//    for (std::vector<int>::iterator it = m_columnCounts->begin(); it < m_columnCounts->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Column " << it - m_columnCounts->begin() << " has " << *it << " nonzeros");
//    }
//#endif //!NDEBUG
//    return;
//}

//void PfiBasis::printMM() const {
//#ifndef NDEBUG
//    DEVINFO(D::PFIMAKER, "MM pattern by rows");
//    for (std::vector<Vector>::iterator it = m_mmRows->begin(); it < m_mmRows->end(); it++) {
//        std::string s;
//        for (int i = 0; i < (int) it->length(); i++) {
//            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
//        }
//        DEVINFO(D::PFIMAKER, s);
//    }
//    DEVINFO(D::PFIMAKER, "MM pattern by columns");
//    for (int i = 0; i < (int) m_mmColumns->size(); i++) {
//        std::string s;
//        for (std::vector<Vector>::iterator it = m_mmColumns->begin(); it < m_mmColumns->end(); it++) {
//            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
//        }
//        DEVINFO(D::PFIMAKER, s);
//    }
//    DEVINFO(D::PFIMAKER, "MM pattern with values");
//    for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {
//        int rowindex = m_mmRowIndices->at(*rowIt);
//        std::string s;
//        for (std::vector<int>::iterator columnIt = m_columnSwapHash->begin(); columnIt < m_columnSwapHash->end(); columnIt++) {
//            int columnindex = m_mmColumnIndices->at(*columnIt);
//            s += Numerical::equals(m_columns->at(columnindex).at(rowindex), 0) ? "-" : "X";
//        }
//        DEVINFO(D::PFIMAKER, s);
//    }
//    DEVINFO(D::PFIMAKER, "MM pattern with values without equals");
//    for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {
//        int rowindex = m_mmRowIndices->at(*rowIt);
//        std::string s;
//        for (std::vector<int>::iterator columnIt = m_columnSwapHash->begin(); columnIt < m_columnSwapHash->end(); columnIt++) {
//            int columnindex = m_mmColumnIndices->at(*columnIt);
//            s += m_columns->at(columnindex).at(rowindex) == 0 ? "-" : "X";
//        }
//        DEVINFO(D::PFIMAKER, s);
//    }

//#endif //!NDEBUG
//}

//void PfiBasis::printSwapHashes() const {
//#ifndef NDEBUG
//    DEVINFO(D::PFIMAKER, "Row swap hash:");
//    for (std::vector<int>::iterator it = m_rowSwapHash->begin(); it < m_rowSwapHash->end(); it++) {
//        DEVINFO(D::PFIMAKER, "    " << *it);
//    }
//    DEVINFO(D::PFIMAKER, "Column swap hash:");
//    for (std::vector<int>::iterator it = m_columnSwapHash->begin(); it < m_columnSwapHash->end(); it++) {
//        DEVINFO(D::PFIMAKER, "    " << *it);
//    }
//#endif //!NDEBUG
//}

//void PfiBasis::printGraph() const {
//#ifndef NDEBUG
//    //Print the outgoing edges
//    DEVINFO(D::PFIMAKER, "Outgoing graph edges");
//    for (std::vector<std::vector<int> >::iterator it = m_mmGraphOut->begin(); it < m_mmGraphOut->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Node " << it - m_mmGraphOut->begin() << ":");
//        for (std::vector<int>::iterator nodeIt = it->begin(); nodeIt < it->end(); nodeIt++) {
//            DEVINFO(D::PFIMAKER, "    " << *nodeIt);
//        }
//    }
//    //Print the incoming edges
//    DEVINFO(D::PFIMAKER, "Incoming graph edges");
//    for (std::vector<std::vector<int> >::iterator it = m_mmGraphIn->begin(); it < m_mmGraphIn->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Node " << it - m_mmGraphIn->begin() << ":");
//        for (std::vector<int>::iterator nodeIt = it->begin(); nodeIt < it->end(); nodeIt++) {
//            DEVINFO(D::PFIMAKER, "    " << *nodeIt);
//        }
//    }
//#endif //!NDEBUG
//}

//void PfiBasis::printStack() const {
//#ifndef NDEBUG
//    //Print the stack
//    DEVINFO(D::PFIMAKER, "Tarjan algorithm stack (Top --> Bottom):");
//    DEVINFO(D::PFIMAKER, "index \tlowest\t nextEdge");
//    for (std::vector<PathNode>::reverse_iterator it = m_stack->rbegin(); it < m_stack->rend(); it++) {
//        DEVINFO(D::PFIMAKER, it->index << " \t " << it->lowest << " \t " << it->nextEdge);
//    }
//#endif //!NDEBUG
//}

//void PfiBasis::printBlocks() const {
//#ifndef NDEBUG
//    //Print the outgoing edges
//    DEVINFO(D::PFIMAKER, "Block sizes:");
//    for (std::vector<int>::iterator it = m_mmBlocks->begin(); it < m_mmBlocks->end(); it++) {
//        DEVINFO(D::PFIMAKER, "Block #" << it - m_mmBlocks->begin() << ": " << *it);
//    }
//    //printMM();
//#endif //!NDEBUG
//}

//void PfiBasis::printStatistics() const {
//    LPINFO("INVERSION FINISHED");
//    LPINFO("Run parameters during the inversion:");
//    switch (triangularMethod) {
//    case ITERATIONS:
//        LPINFO("Used triangular method: ITERATIONS")
//        ;
//        break;
//    case LINKS:
//        LPINFO("Used triangular method: LINKS")
//        ;
//        break;
//    default:
//        LPWARNING("No triangular method defined!")
//        ;
//        break;
//    }
//    switch (nontriangularMethod) {
//    case SEARCH:
//        LPINFO("Used non-triangular method: SEARCH")
//        ;
//        break;
//    case BLOCK_TRIANGULAR:
//        LPINFO("Used non-triangular method: BLOCK_TRIANGULAR")
//        ;
//        break;
//    case BLOCK_ORDERED_TRIANGULAR:
//        LPINFO("Used non-triangular method: BLOCK_ORDERED_TRIANGULAR")
//        ;
//        break;
//    default:
//        LPWARNING("No non-triangular method defined!")
//        ;
//        break;
//    }
//    switch (triangularPivotRule) {
//    case ABSOLUTE_TOLERANCE:
//        LPINFO("Used triangular pivot rule: ABSOLUTE_TOLERANCE")
//        ;
//        break;
//    case RELATIVE_TOLERANCE:
//        LPINFO("Used triangular pivot rule: RELATIVE_TOLERANCE")
//        ;
//        break;
//    default:
//        LPWARNING("No triangular pivot rule defined!")
//        ;
//        break;
//    }
//    switch (nontriangularPivotRule) {
//    case NONE:
//        LPINFO("Used non-triangular pivot rule: NONE")
//        ;
//        break;
//    case THRESHOLD:
//        LPINFO("Used non-triangular pivot rule: THRESHOLD")
//        ;
//        break;
//    default:
//        LPWARNING("No non-triangular pivot rule defined!")
//        ;
//        break;
//    }
//    LPINFO("Nonzero statistics: ");
//    LPINFO("Nonzeros in the basis: " << m_basisNonzeros);
//    LPINFO("Nonzeros in the inverted part of the basis: " << m_invertedNonzeros);
//    LPINFO("Nonzeros in the inverse: " << m_inverseNonzeros);
//    LPINFO("Fill in amount: " << ((Numerical::Double)m_inverseNonzeros / (Numerical::Double)m_invertedNonzeros)*100 - 100 << "% )");
//    LPINFO("Inversion time statistics: ");
//    LPINFO("Total inversion time: " << ((Numerical::Double) (cl_inversion) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
//    LPINFO("Active submatrix copy: " << ((Numerical::Double) (cl_copy) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");

//    LPINFO("Find transversal time:"<<(Numerical::Double) (cl_transversal) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
//    LPINFO("Create block triangular time:"<<(Numerical::Double) (cl_createBlockTriangular) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
//    LPINFO("Pivot time:"<<(Numerical::Double) (cl_pivot) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
//    LPINFO("Update columns time:"<<(Numerical::Double) (cl_updateColumns) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");

//    if (triangularMethod == LINKS) {
//        LPINFO("Link building: " << ((Numerical::Double) (cl_linkBuild) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
//    }
//    LPINFO("R part inversion: " << ((Numerical::Double) (cl_invertR) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
//    LPINFO("M part inversion: " << ((Numerical::Double) (cl_invertM) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
//    LPINFO("C part inversion: " << ((Numerical::Double) (cl_invertC) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
//}

//void PfiBasis::printTransformationStatistics() const {
//    if (SimplexParameters::getInstance()->getParameterValue("debug_level") == -1) {
//        cout << m_inversionCount << ", " << m_transformationAverage << ", " << m_mNumAverage << ", ";
//    } else {
//        LPINFO("Inversion count: "<<m_inversionCount);
//        //		LPINFO("Transformation count in the last inversion: "<<m_transformationCount);
//        LPINFO("Transformation count average: "<<m_transformationAverage);
//        LPINFO("Average size of the non-triangular part:"<<m_mNumAverage);
//    }
//    printStatistics();
//}
