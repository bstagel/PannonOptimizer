#ifndef LPPROBLEM_H
#define LPPROBLEM_H

#include <globals.h>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <utils/file.h>
#include <utils/hashtable.h>
#include <utils/list.h>
#include <linalg/densevector.h>
#include <lp/modelbuilder.h>
#include <lp/variable.h>
#include <lp/constraint.h>

#define CREATE_STRING( result, expr ) { \
    std::ostringstream stream; \
    stream << expr; \
    result = stream.str(); \
}

class LpModelBuilder: public ModelBuilder {
public:

    struct LpError {
        int m_errorType;
        int m_errorLine;
        std::string m_errorStr;
    };

    LpModelBuilder();

    virtual ~LpModelBuilder();

    void loadFromFile(const std::string & fileName);

    LpError getError();

    void clear();

    // ********************************************************
    // Inherited from MatrixBuilder
    // ********************************************************

    /**
     *
     * @return
     */
    unsigned int getColumnCount() const;

    /**
     *
     * @return
     */
    unsigned int getRowCount() const;

    /**
     *
     * @param index
     * @param rowVector
     */
    void buildRow(unsigned int index, SparseVector * rowVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     *
     * @param index
     * @param columnVector
     */
    void buildColumn(unsigned int index, SparseVector * columnVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     *
     * @return
     */
    bool hasRowwiseRepresentation() const;

    /**
     *
     * @return
     */
    bool hasColumnwiseRepresentation() const;

    // ********************************************************
    // Inherited from ModelBuilder
    // ********************************************************

    /**
     *
     * @param index
     * @return
     */
    const Variable & getVariable(unsigned int index) const;

    /**
     *
     * @param index
     * @return
     */
    const Constraint & getConstraint(unsigned int index) const ;

    /**
     *
     * @param costVector
     */
    void buildCostVector(DenseVector * costVector) const;

    /**
     *
     * @return
     */
    double getObjectiveConstant() const;

    /**
     *
     * @return
     */
    std::string getName() const;

    /**
     */
    OBJECTIVE_TYPE getObjectiveType() const;


private:

    enum LP_SECTION {
        LP_START,
        LP_OBJECTIVE,
        LP_ST,
        LP_BOUNDS,
        LP_END,
        LP_UNKNOWN
    } m_section;

    enum LP_EQ_STATE {
        LP_EQ_NAME,
        LP_EQ_SIGN,
        LP_EQ_COEFF,
        LP_EQ_VAR,
        LP_EQ_TYPE,
        LP_EQ_RHS
    } m_eqState;

    struct LpRow {
        std::string m_rowName;
        SparseVector m_vector;
        Numerical::Double m_lowerBound;
        Numerical::Double m_upperBound;
    };

    struct LpVar {
        std::string m_varName;
        Numerical::Double m_lowerBound;
        Numerical::Double m_upperBound;
    };

    std::ifstream* m_inputFile;
    int m_currentLine;
    Numerical::Double m_currentCoeff;
    LpError m_error;

    std::list<LpVar> m_lpVars;
    LpVar* m_curVar;
    std::list<LpRow> m_lpRows;
    LpRow m_row;
    SparseVector m_costVector;
    Numerical::Double m_costConstant;
    std::string m_problemName;
    std::string m_buffer;
    std::string m_word;
    int m_type;
    bool m_maximize;
    bool m_terminated;
    std::vector<LpRow*> m_rows;
    std::vector<LpVar*> m_vars;

    std::string upper(const std::string& str);
    void toupper(std::string& str);
    std::string getNextWord();
    bool checkSection(const std::string& word);
    void getSection();
    void processObj();
    void processSt();
    void processBounds();
    void indexEntities();
    void parseRow(SparseVector& target);
    int getVar(const std::string& name);
    LpVar* getVarPtr(const std::string& name);

};

#endif // LPPROBLEM_H
