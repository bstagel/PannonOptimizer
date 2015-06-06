/**
 * @file pfibasis.h
 */

#ifndef PFIBASIS_H
#define PFIBASIS_H

#include <globals.h>
#include <vector>
#include <list>

#include <simplex/basis.h>
#include <simplex/simplexmodel.h>

#include <utils/exceptions.h>

extern double loosedBits;
extern double expDiffSquareSum;
extern double expDiffSum;
extern int expCounter;

extern double aprExpDiffSquareSum;
extern double aprExpDiffSum;
extern int aprExpCounter;

/**
 * This class generates the PFI (Product Form of the Inverse) form of the basis.
 * The algorithm can be parametrized to support different methods of inversion.
 * The parameters are stored in enum values.
 *
 * @class PfiBasis
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
        NONE,
        THRESHOLD
    };

    /**
     * Construct the PfiBasis object with a model assigned to it.
     *
     * @construtor
     * @param model The assigned model.
     */
    PfiBasis();
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
    void invert();

    /**
     * It creates a new inverse after a basis change.
     *
     * @param vector the alpha vector of the incoming variable.
     * @param pivotRow the pivot position (row index) in the vector.
     * @param incoming the index of the incoming variable (to update the basis head).
     *
     * @throws NumericalException if the outgoing variable trys to leave the basis
     * with a value different from LB or UB.
     */
    void append(const SparseVector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState);

    /**
     * Does the FTRAN operation on a vector.
     * The operation B^-1 * v is called FTRAN.
     *
     * @param vector the vector to be transformed.
     */
    virtual void Ftran(DenseVector &vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void Ftran(SparseVector &vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;

    virtual void FtranCheck(DenseVector & vector, FTRAN_MODE mode = DEFAULT_FTRAN) const;
    virtual void BtranCheck(DenseVector &vector) const;

    virtual void analyzeStability() const;

    /**
     * Does the BTRAN operation on a vector.
     * The operation v * B^-1 is called BTRAN.
     *
     * @param vector the vector to be transformed.
     */
    virtual void Btran(DenseVector &vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;
    virtual void Btran(SparseVector &vector, BTRAN_MODE mode = DEFAULT_BTRAN) const;

    /**
     * Thread handling
     */
    void registerThread();
    void releaseThread();
    void clearUpdates();

    /**
     * Static memory allocations
     */
    void prepareForModel(const Model &model);

    /**
     * Static memory release
     */
    void releaseModel();

    /**
     * Dumps the content of the inverse to the stream os.
     * @param os
     */
    void dumbToStream(std::ostream & os) const;

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
     * Stores the basis inverse in the PFI form (as a vector of ETMs).
     */
    static thread_local std::vector<ETM>* m_updates;

    static thread_local IndexedDenseVector* m_updateHelper;

    static std::vector<std::vector<ETM>*> m_updatesManager;

    static std::vector<IndexedDenseVector*> m_updateHelperManager;

    static std::mutex m_updateLock;

    /**
     * Stores the original variable indices for each column in the submatrix.
     */
    std::vector<int> m_basicColumnIndices;

    std::vector<std::list<int> > m_rowNonzeroIndices;

    /**
     * The vector of the column counts (c_i).
     * The column count represents the number of nonzeros in a column. A column count of
     * an inactive column (used) is -1, the column count of a column of the active submatrix
     * is a natural number.
     */
    std::vector<int> m_columnCounts;

    /**
     * The vector of the row counts (r_i).
     * The row count represents the number of nonzeros in a row. A row count of
     * an  inactive row (used or logical) is -1, the row count of a row of the
     * active submatrix is a natural number.
     */
    std::vector<int> m_rowCounts;

    /**
     * Columns for the C part.
     * During the identification of the CC part, the vectors are gathered out
     * from the submatrix, thus pivoting can be done only after the processing
     * of the nontriangular part. The pointers for these sumbatrix columns are
     * collected in this vector.
     */
    std::vector<const SparseVector*>* m_cColumns;

    /**
     * For each C column the corresponding pivot index (row index) is stored here.
     */
    std::vector<int>* m_cPivotIndexes;


    /**
     * Rowwise representation of the MM (non-triangular) part.
     * The MM part is logically reordered after the R and C parts are identified
     * to logically form a standard square matrix.
     */
    std::vector<SparseVector*>* m_mmRows;

    /**
     * The
     */
    std::vector<int>* m_mmRowIndices;

    /**
     * Columnwise representation of the MM (non-triangular) part.
     * The MM part is logically reordered after the R and C parts are identified
     * to logically form a standard square matrix.
     */
    std::vector<SparseVector*>* m_mmColumns;
    std::vector<int>* m_mmColumnIndices;

    std::vector<std::vector<int> >* m_mmGraphOut;
    /**
     * Represents the usage of a node during the tarjan algorithm.
     * -1 means the node is already used
     *  0 means the node is available
     *  1 means the node is in the stack
     */
    std::vector<char>* m_mmGraphUsed;
    std::vector<PathNode>* m_stack;
    std::vector<int>* m_mmBlocks;
    std::vector<int>* m_rowSwapHash;
    std::vector<int>* m_columnSwapHash;
    std::vector<int>* m_columnSwapLog;

    unsigned int m_transformationCount;
    Numerical::Double m_transformationAverage;
    Numerical::Double m_mNumAverage;
    static thread_local int m_inversionCount;

    //Run parameters
    //Non-triangular method parameter
    NONTRIANGULAR_METHOD m_nontriangularMethod;
    //Non-triangular pivot rule parameter
    NONTRIANGULAR_PIVOT_RULE m_nontriangularPivotRule;
    //Threshold value for threshold pivoting
    const double & m_threshold;

    void copyBasis(bool buildIndexLists = true);

    void buildRowCountIndexLists(int size, int maxRowCount);
    void buildColumnCountIndexLists(int size, int maxColumnCount);

    void invertR();
    void findC();
    void invertM();
    void invertC();

    void pivot(const SparseVector &column, int pivotRow, std::vector<ETM>* etaFile);

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
    bool nontriangularCheck(int& rowindex, const SparseVector *currentColumn, int blockNum);

    void checkSingularity();

    void printCounts() const;
    void printMM() const;
    void printSwapHashes() const;
    void printStack() const;
    void printBlocks() const;
    void printStatistics() const;
    void printTransformationStatistics() const;

    static NONTRIANGULAR_METHOD getNontriangularMethod(const std::string & method) {
        if (method == "SIMPLE_KERNEL") {
            return SEARCH;
        }
        if (method == "BLOCK_PIVOT") {
            return BLOCK_TRIANGULAR;
        }
        if (method == "BLOCK_ORDER") {
            return BLOCK_ORDERED_TRIANGULAR;
        }
        // TODO: exception
        return SEARCH;
    }

    static NONTRIANGULAR_PIVOT_RULE getNontriangularPivotRule(const std::string & method) {
        if (method == "SIMPLE_PIVOT") {
            return NONE;
        }
        if (method == "THRESHOLD_PIVOT") {
            return THRESHOLD;
        }
        // TODO: exception
        return NONE;
    }
};

#endif // PFIBASIS_H
