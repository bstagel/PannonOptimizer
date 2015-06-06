/**
 * @file lubasis.h
 */

#ifndef LUBASIS_H
#define LUBASIS_H

#include <globals.h>
#include <vector>
#include <list>

#include <simplex/basis.h>
#include <simplex/simplexmodel.h>

#include <utils/exceptions.h>

struct DoubleETM
{
    DoubleETM(): row(NULL), column(NULL) {}
    DoubleETM(SparseVector* row, SparseVector* column): row(row), column(column){}
    SparseVector* row;
    SparseVector* column;
};

struct PivotPosition
{
    PivotPosition(): rowindex(-1), columnindex(-1) {}
    PivotPosition(int rowindex, int columnindex): rowindex(rowindex), columnindex(columnindex){}
    int rowindex;
    int columnindex;
};

class LuBasis : public Basis{
public:
    LuBasis();
    virtual ~LuBasis();

    void invert();
    void append(const SparseVector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState);

    virtual void Ftran(DenseVector &vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void Ftran(SparseVector &vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void Btran(DenseVector &vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;
    virtual void Btran(SparseVector &vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;

    virtual void FtranCheck(DenseVector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void BtranCheck(DenseVector &vector) const;

    virtual void analyzeStability() const;

    /**
     * Thread handling
     */
    void registerThread();
    void releaseThread();

    /**
     * Static memory allocations
     */
    void prepareForModel(const Model& model);

    /**
     * Static memory release
     */
    void releaseModel();

private:

    std::vector<DoubleETM>* m_lower;
    std::vector<DoubleETM>* m_upper;
    std::vector<ETM>* m_updateETMs;
    std::vector<PivotPosition> m_pivots;
    std::vector<PivotPosition> m_pivotIndices;
    unsigned int m_factorizedPart;

    std::vector<SparseVector*> m_basicRows;

    unsigned int m_transformationCount;
    Numerical::Double m_transformationAverage;
    Numerical::Double m_mNumAverage;
    static thread_local int m_inversionCount;

    //Threshold value for threshold pivoting
    const double & m_threshold;

    void copyBasis();

    void invertC();
    void invertR();
    void invertM();

    void checkIndexListValidities();
    void findPivot(int & rowindex, int & columnindex,
                   const std::vector<int> & rowMarkowitzs, const std::vector<int> & columnMarkowitzs,
                   const std::vector<int> & rowMarkowitzColumnIndices, const std::vector<int> & columnMarkowitzRowIndices);

    void checkSingularity();

    void printStatistics() const;
    void printTransformationStatistics() const;
};

#endif // LUBASIS_H
