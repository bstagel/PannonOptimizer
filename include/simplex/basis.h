/**
 * @file basis.h
 */

#ifndef BASIS_H
#define BASIS_H

class SimplexModel;

#include <globals.h>
#include <vector>
#include <list>

#include <linalg/vector.h>

#include <simplex/simplex.h>

#include <utils/exceptions.h>
#include <utils/indexlist.h>

/**
 * A struct describing an ETM %Matrix.
 * This structure represents an Elementary Transformation %Matrix (ETM).
 * Az ETM is such a matrix that differs from the unit matrix in only one column.
 * This struct is used throughout the inversion process and the FTRAN and BTRAN
 * operations. Az ETM is a $m \times m$ matrix.
 */
struct ETM
{
    ETM(): eta(NULL), index(0) {}
    ETM(Vector* eta, unsigned int index): eta(eta), index(index){}
    /**
     * Eta vector.
     * The nontrivial column of the ETM is the eta column. This is a pointer of
     * that vector, it is allocated dynamically .
     */
    Vector* eta;

    /**
     * Index of the nontrivial column.
     */
    unsigned int index;

};

/**
 * @brief The Basis class
 */
class Basis {
public:

    enum FTRAN_MODE
    {
        DEFAULT_FTRAN
    };

    enum BTRAN_MODE
    {
        DEFAULT_BTRAN
    };

    Basis(const SimplexModel& model,
          std::vector<int>* basisHead,
          IndexList<const Numerical::Double*>* variableStates,
          const Vector& basicVariableValues);
    virtual ~Basis();

    /**
     * Gives information about the state of the inverse.
     * An inverse is considered fresh, if it is in the state after factorization.
     * (There was no modification since then)
     *
     * @return true if the basis is fresh, otherwise false.
     */
    inline bool isFresh() const {
        return m_isFresh;
    }
    virtual void invert() = 0;
    virtual void append(const Vector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState) = 0;

    virtual void Ftran(Vector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const = 0;
    virtual void Btran(Vector & vector, BTRAN_MODE mode = DEFAULT_BTRAN) const = 0;

    virtual void printTransformationStatistics() const = 0;

    int getSingularityCount() {return m_singularityCounter;}
protected:
    const SimplexModel& m_model;

    std::vector<int>* m_basisHead;
    IndexList<const Numerical::Double*>* m_variableStates;
    const Vector& m_basicVariableValues;

    std::vector<int> m_basisNewHead;

    /**
     * Stores the active submatrix of used for the inversion (columnwise).
     * The dimension of the submatrix is m*k , where k is the number of struxtural variables
     * in the basis head. The rows of logical are also contained here. The logical rows
     * are omitted by setting the appropriate row counts to -1.
     */
    std::vector<const Vector*> m_basicColumns;
    std::vector<Vector*> m_basicColumnCopies;

    /**
     * The vector of linked lists of column indices.
     * Each linked list represents a group of columns that have the same column count.
     * The first list has columns with a column count of 1, the second with 2, etc.
     */
    IndexList<> m_columnCountIndexList;

    /**
     * The vector of linked lists of row indices.
     * Each linked list represents a group of rows that have the same row count.
     * The first list has rows with a row count of 1, the second with 2, etc.
     */
    IndexList<> m_rowCountIndexList;

    bool m_isFresh;
    unsigned int m_basisNonzeros, m_inverseNonzeros;
    int m_singularityCounter;

    const double & m_inversion;

    void setNewHead();
    Vector* createEta(const Vector& vector, int pivotPosition);

    virtual void checkSingularity() = 0;

    virtual void printStatistics() const = 0;
    void printActiveSubmatrix() const;

};

#endif // BASIS_H
