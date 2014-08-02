/**
 * @file lubasis.h
 */

#ifndef LUBASIS_H
#define LUBASIS_H

#include <globals.h>
#include <vector>
#include <list>

#include <linalg/vector.h>

#include <simplex/basis.h>
#include <simplex/simplexmodel.h>

#include <utils/exceptions.h>

struct DoubleETM
{
    DoubleETM(): row(NULL), column(NULL) {}
    DoubleETM(Vector* row, Vector* column): row(row), column(column){}
    Vector* row;
    Vector* column;
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
    LuBasis(const SimplexModel& model,
            std::vector<int>* basisHead,
            IndexList<const Numerical::Double*>* variableStates,
            const Vector& basicVariableValues);
    virtual ~LuBasis();

    void invert();
    void append(const Vector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState);

    virtual void Ftran(Vector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void Btran(Vector & vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;

    virtual void FtranCheck(Vector & vector, Vector & checkVector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
private:

    std::vector<DoubleETM>* m_lower;
    std::vector<DoubleETM>* m_upper;
    std::vector<ETM>* m_updateETMs;
    std::vector<PivotPosition> m_pivots;
    std::vector<PivotPosition> m_pivotIndices;
    unsigned int m_factorizedPart;

    std::vector<Vector*> m_basicRows;

    unsigned int m_transformationCount;
    double m_transformationAverage;
    double m_mNumAverage;
    THREAD_STATIC_DECL int m_inversionCount;

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
