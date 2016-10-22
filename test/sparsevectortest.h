#ifndef SPARSEVECTORTEST_H
#define SPARSEVECTORTEST_H

#include <framework/tester.h>
#include <framework/unittest.h>

class SparseVectorTestSuite : public UnitTest {
public:
    SparseVectorTestSuite(const char * name);

private:
    void init();
    void copy();
    void move();
    void assign();
    void assignDense();
    void moveAssign();
    void createUnitVector();
    void createVectorFromDenseArray();
    void createVectorFromSparseArray();
    void createEtaVector();
    void at();
    void arrayOperator();
    void set();
    void setNewNonzero();
    void length();
    void nonZeros();
    void euclidNorm();
    void l1Norm();
    void elementaryFtran();
    void addSparseVector();
    void addDenseVector();
    void addIndexedDenseVector();
    void scale();
    void sortIndices();
    void clear();
    void setAddMode();
    void setDotProductMode();
    void beginNonzero();
    void endNonzero();
    void iterator();


};

#endif // SPARSEVECTORTEST_H
