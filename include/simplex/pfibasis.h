/**
 * @file pfibasis.h
 */

#ifndef PFIBASIS_H
#define PFIBASIS_H

#include <globals.h>
#include <vector>
#include <list>

#include <linalg/vector.h>

#include <simplex/basis.h>
#include <simplex/simplexmodel.h>

#include <utils/exceptions.h>

/**
 * This class generates the PFI (Product Form of the Inverse) form of the basis.
 * The algorithm can be parametrized to support different methods of inversion.
 * The parameters are stored in enum values.
 */
class PfiBasis : public Basis{
public:
    /**
     * Used to distinguish two methods processing the non-triangular part of the basis.
     * - SEARCH method iterates through the nonzeros searching for a potential pivot.
     * Fill-in should be really large with this, not applicable for large-scale problems.
     * - BLOCK_TRIANGULAR method rearranges the kernel into a block triangular form,
     * using the Tarjan algoritm. This leads to a low fill-in and gives a chance to use
     * threshold pivoting within each nontriangular block.
     * - BLOCK_ORDERED_TRIANGULAR method rearranges the kernel into a block triangular form,
     * using the Tarjan algoritm. This leads to a low fill-in and gives a chance to use
     * threshold pivoting within each nontriangular block. The processing of each block
     * in the block triangular form is done in an ordered way. In each step a column
     * with the least column count is chosen. This improves numerical stability.
     */
    enum NONTRIANGULAR_METHOD{
        SEARCH,
        BLOCK_TRIANGULAR,
        BLOCK_ORDERED_TRIANGULAR
    };

    /**
     * Used to determine how to pivot the non-triangular part.
     * - NONE uses nothing but an absolute tolerance check on the pivot element to determine
     * if it is zero.
     * - THRESHOLD uses the threshold pivoting logic. This is done by selecting elements form
     * the column of the active submatrix that are "big enough" (determined by a multiplier and
     * the abolute maximal element) to be eligible pivot elements. After these are selected, the
     * one with the least row count (to minimize column updates) are choosen to be the pivot.
     */
    enum NONTRIANGULAR_PIVOT_RULE{
        NONE, THRESHOLD
    };

    /**
     * Construct the PfiBasis object with a model assigned to it.
     *
     * @construtor
     * @param model The assigned model.
     */
    PfiBasis(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             const Vector& basicVariableValues);
    /**
     * Destruct the PfiBasis object and frees up the allocated memory.
     *
     * @destructor
     */
    virtual ~PfiBasis();

    /**
     * Generate the inverse of the given basis.
     * The inverse consists of ETM matrices.
     *
     * @throws NumericalException occures if a pivot element is zero.
     */
    void invert() throw (NumericalException);

    /**
     * Returns the last pivot index from the basis inverse.
     *
     * @return the row of the last pivot.
     */
    inline int lastPivotIndex() const {return m_basis->back().index;}

    /**
     * It creates a new inverse after a basis change.
     *
     * @param v the alpha vector of the incoming variable.
     * @param pivotRow the pivot position (row index) in the vector.
     * @param incoming the index of the incoming variable (to update the basis head).
     *
     * @throws NumericalException if the outgoing variable try to leaves the basis
     * with a value different from LB and UB.
     */
    void append(const Vector & vector, int pivotRow, int incoming) throw (NumericalException);

    /**
     * Does the FTRAN operation on a vector.
     * The operation B^-1 * v is called FTRAN.
     *
     * @param v the vector to be transformed.
     */
    virtual void Ftran(Vector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;


    /**
     * Does the FTRAN operation on a vector.
     * The operation B^-1 * v is called FTRAN.
     *
     * @param v the vector to be transformed.
     */
    virtual void Btran(Vector & vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;



    //Parameters of the inversion algorithm

    /**
     * Setter to choose among the algorithms used to process the non-triangulat part.
     *
     * @param method the method to be used.
     */
    inline void setNontriangularMethod(const NONTRIANGULAR_METHOD method){nontriangularMethod=method;}

    /**
     * Setter to choose among the pivoting rules during the processing of the non-triangular part.
     *
     * @param method the method to be used.
     */
    inline void setNontriangularPivotRule(const NONTRIANGULAR_PIVOT_RULE rule){nontriangularPivotRule=rule;}

    /**
     * Setter to determine the threshold used when the non-triangular part uses threshold pivoting.
     * The threshold multiplied by the absolute largest element of the active submatrix gives a
     * lower bound on the pivot candidates.
     *
     * @param value the value of the threshold.
     */
    inline void setThreshold(const Numerical::Double value){threshold=value;}

private:

    /**
     * Used for the Tarjan algorithm
     */
    struct PathNode{
        int index;
        int lowest;
        int nextEdge;
    };

    /**
     * Stores the basis inverse in the PFI form (as a vector of ETMs).
     */
    std::vector<ETM>* m_basis;

    /**
     * Columns for the C part.
     * During the identification of the CC part, the vectors are gathered out
     * from the submatrix, thus pivoting can be done only after the processing
     * of the nontriangular part. The pointers for these sumbatrix columns are
     * collected in this vector.
     */
    std::vector<const Vector*>* m_cColumns;

    /**
     * For each C column the corresponding pivot index (row index) is stored here.
     */
    std::vector<int>* m_cPivotIndexes;


    /**
     * Rowwise representation of the MM (non-triangular) part.
     * The MM part is logically reordered after the R and C parts are identified
     * to logically form a standard square matrix.
     */
    std::vector<Vector>* m_mmRows;

    /**
     * The
     */
    std::vector<int>* m_mmRowIndices;

    /**
     * Columnwise representation of the MM (non-triangular) part.
     * The MM part is logically reordered after the R and C parts are identified
     * to logically form a standard square matrix.
     */
    std::vector<Vector>* m_mmColumns;
    std::vector<int>* m_mmColumnIndices;
    std::vector<std::vector<int> >* m_mmGraphOut;
    std::vector<std::vector<int> >* m_mmGraphIn;
    std::vector<PathNode>* m_stack;
    std::vector<char>* m_orderedIndex;
    std::vector<int>* m_mmBlocks;
    std::vector<int>* m_rowSwapHash;
    std::vector<int>* m_columnSwapHash;
    std::vector<int>* m_columnSwapLog;

    unsigned int m_transformationCount;
    double m_transformationAverage;
    double m_mNumAverage;
    THREAD_STATIC_DECL int m_inversionCount;

    //Run parameters
    //Non-triangular method parameter
    NONTRIANGULAR_METHOD nontriangularMethod;
    //Non-triangular pivot rule parameter
    NONTRIANGULAR_PIVOT_RULE nontriangularPivotRule;
    //Threshold value for threshold pivoting
    Numerical::Double threshold;

    //Clocks for time measurement of the functional units
    clock_t cl_copy, cl_linkBuild;
    //Profiling
    clock_t cl_transversal, cl_createBlockTriangular, cl_pivot, cl_updateColumns;
    clock_t cl_invertR, cl_invertM, cl_invertC;

    void copyBasis(bool buildIndexLists = true);

    void buildRowCountIndexLists(int size, int maxRowCount);
    void buildColumnCountIndexLists(int size, int maxColumnCount);

    void invertR() throw (NumericalException);
    void findC();
    void invertM() throw (NumericalException);
    void invertC() throw (NumericalException);

    void pivot(const Vector& column, int pivotRow) throw (NumericalException);

    void buildMM();
    void findTransversal();
    void swapRows(int rowIndex1, int rowIndex2);
    void swapColumns(int columnIndex1, int columnIndex2);
    int searchColumn(int columnIndex,int searchIndex,std::vector<int>& searchedRows);
    void createGraph();
    void tarjan();
    int searchNode();
    void createBlockTriangular();
    void updateColumns(unsigned int rowindex,unsigned int columnindex);
    bool nontriangularCheck(int& rowindex, const Vector *currentColumn, int blockNum);

    void printCounts() const;
    void printMM() const;
    void printSwapHashes() const;
    void printGraph() const;
    void printStack() const;
    void printBlocks() const;
    void printStatistics() const;
    void printTransformationStatistics() const;
};

#endif // PFIBASIS_H
