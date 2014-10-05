/**
 * @file basis.h This file contains the Basis class.
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

extern int etaExpSum;
extern double etaExpSquareSum;
extern int etaExpCount;

/**
 * A struct describing an ETM %Matrix.
 * This structure represents an Elementary Transformation %Matrix (ETM).
 * Az ETM is such a matrix that differs from the unit matrix in only one column.
 * This struct is used throughout the inversion process and the FTRAN and BTRAN
 * operations. Az ETM is a $m \times m$ matrix.
 *
 *@struct ETM
 */
struct ETM
{
    /**
     * Default constructor, creates an empty ETM.
     *
     * @constructor
     */
    ETM(): eta(NULL), index(0) {}

    /**
     * Initializing constructor.
     *
     * @constructor
     * @param eta The nontrivial column of the ETM.
     * @param index The index of the nontrivial column.
     */
    ETM(Vector* eta, unsigned int index): eta(eta), index(index){}

    /**
     * Pointer to the eta vector.
     * The eta vector is the nontrivial column of the ETM.
     */
    Vector* eta;

    /**
     * Index of the nontrivial column.
     */
    unsigned int index;
};

/**
 * This class describes a general basis.
 * A basis is a nonsingular matrix.
 *
 * @class Basis
 */
class Basis {
public:

    /**
     * This indicates the possible FTRAN modes for further development.
     * Description of the FTRAN operation can be found at Basis::Ftran().
     */
    enum FTRAN_MODE
    {
        DEFAULT_FTRAN
    };

    /**
     * This indicates the possible BTRAN modes for further development.
     ** Describtion of the FTRAN operation can be found at Basis::Btran().
     */
    enum BTRAN_MODE
    {
        DEFAULT_BTRAN
    };

    /**
     * Initializing constructor.
     *
     * @constructor
     * @param model The model of the LP problem.
     * @param basisHead The indices of the basic variables.
     * @param variableStates Index list containing variable states (being at lower or upper bound)
     * @param basicVariableValues The X_B vector.
     */
    Basis(const SimplexModel& model,
          std::vector<int>* basisHead,
          IndexList<const Numerical::Double*>* variableStates,
          const Vector& basicVariableValues);

    /**
     * Default destructor.
     *
     * @destructor
     */
    virtual ~Basis();

    /**
     * Gives information about the state of the inverse.
     * An inverse is considered fresh while it is unmodified after factorization.
     *
     * @return true if the basis is fresh, otherwise false.
     */
    inline bool isFresh() const {return m_isFresh;}

    /**
     * Pure virtual function for performing a basis inversion.
     * Different basis types may require different inversion techniques.
     */
    virtual void invert() = 0;

    /**
     * Pure virtual function performing the basis change.
     *
     * @param vector The alpha vector of the incoming variable.
     * @param pivotRow Index of the basis leaving variable.
     * @param incoming Index of the incoming variable.
     * @param outgoingState Determines the bound the outgoing variable will be set.
     *
     * @throws NumericalException if the outgoing variable tries to leave the basis
     * with a value different from its bounds.
     */
    virtual void append(const Vector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState) = 0;

    /**
     * The basic FTRAN operation.
     * The FTRAN operation is B^-1 * v.
     * @param The v vector of the operation.
     * @param mode Optional parameter for the different FTRAN modes, see Basis::FTRAN_MODE.
     */
    virtual void Ftran(Vector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const = 0;

    /**
     * The basic BTRAN operation.
     * The operation v * B^-1 is called BTRAN.
     * @param vector on which the operation shall be done
     * @param mode optional parameter for the different BTRAN modes, see Basis::BTRAN_MODE
     */
    virtual void Btran(Vector & vector, BTRAN_MODE mode = DEFAULT_BTRAN) const = 0;

    virtual void FtranCheck(Vector & vector, Vector & checkVector, FTRAN_MODE mode = DEFAULT_FTRAN) const = 0;

    /**
     * Pure virtual function to print general information about the basis transformation to the output.
     */
    virtual void printTransformationStatistics() const = 0;

    /**
     * Returns the singularity count of the basis.
     * @return Basis::m_singularityCounter
     */
    int getSingularityCount() {return m_singularityCounter;}
protected:
    /**
     * The model if the LP problem.
     */
    const SimplexModel& m_model;

    /**
     * Vector containing the basic variable indices.
     */
    std::vector<int>* m_basisHead;

    /**
     * Index list containing the variable states (being at lower upper bound).
     */
    IndexList<const Numerical::Double*>* m_variableStates;

    /**
     * The X_B vector.
     */
    const Vector& m_basicVariableValues;

    /**
     * The new basis head needed in the Basis::setNewHead() function.
     */
    std::vector<int> m_basisNewHead;

    /**
     * Stores the active submatrix used for the inversion (columnwise).
     * The dimension of the submatrix is m*k , where k is the number of structural variables
     * in the basis head. The rows of logical are also contained here. The logical rows
     * are omitted by setting the appropriate row counts to -1.
     */
    std::vector<const Vector*> m_basicColumns;

    /**
     * Contains copies of the basic columns. It's needed in the inversion process.
     */
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

    /**
     * Describes whether the basis is fresh or has been changed.
     */
    bool m_isFresh;

    /**
     * Counters for the nonzero element in the basis and basis inverse.
     */
    unsigned int m_basisNonzeros, m_inverseNonzeros;

    /**
     * The measure of singularity of the basis.
     */
    int m_singularityCounter;

    /**
     * Parameter reference for the run-time parameter "e_pivot".
     * For details see SimplexParameterHandler.
     */
    const double & m_inversion;

    /**
     * This function sets the new basis head, after inversion it updates the variable states too.
     */
    void setNewHead();

    /**
     * This function creates an Eta vector using the pivot element. The vector is needed to determine the basis inverse.
     * @param vector is the alpha vector containing the pivot element.
     * @param pivotPosition points to the pivot element in the vector
     * @return the computed Eta vector.
     *
     * @throws NumericalException if the pivot element is zero.
     */
    Vector* createEta(const Vector& vector, int pivotPosition);

    /**
     * Checker for the numerical singularity.
     */
    virtual void checkSingularity() = 0;

    /**
     * Pure virtual function printing the statistics to the output.
     */
    virtual void printStatistics() const = 0;

    /**
     * Prints the active submatrix to the output.
     */
    void printActiveSubmatrix() const;
};

#endif // BASIS_H
