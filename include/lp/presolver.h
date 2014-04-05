/**
 * @file presolver.h
 */


#ifndef PRESOLVER_H
#define PRESOLVER_H

class PresolverModule;
#include <lp/model.h>
#include <lp/presolvermodule.h>
#include <utils/indexlist.h>
#include <utils/exceptions.h>

class Presolver {
    friend class PresolverTestSuite;
public:

    enum PRESOLVE_MODE
    {
        DEFAULT, CUSTOM, SINGLE_MODULE, NO_PRESOLVE
    };

    class PresolverException : public PanOptException {
    public:
        PresolverException(const std::string & message) : PanOptException::PanOptException(message) {}
        ~PresolverException();
    };

    Presolver(Model * model);
    ~Presolver();

    //getter functions of the presolver
    inline Model * getModel() { return m_model; }
    inline unsigned int getTimeLimit() { return m_timeLimit; }
    inline unsigned int getIterationLimit() { return m_iterationLimit; }

    inline Vector * getRowNonzeros() { return m_rowNonzeros; }
    inline Vector * getColumnNonzeros() { return m_columnNonzeros; }

    inline std::vector<Variable> * getVariables() { return m_variables; }
    inline std::vector<Constraint> * getConstraints() { return m_constraints; }

    inline std::vector<Vector*> * getSubstituteVectors() { return &m_substituteVectors; }

    //setter functions of the presolver

    inline void setTimeLimit(unsigned int time) { m_timeLimit = time; }
    inline void setIterationLimit(unsigned int maxIterations) { m_iterationLimit = maxIterations; }
    inline void setPresolveMode(PRESOLVE_MODE mode) { m_mode = mode; }

    //functions to handle the nesseccary modules
    inline void addModule(PresolverModule * newModule) { m_modules.push_back(newModule); }
    inline void clearModules() { m_modules.clear(); }

    //this function fixes the variable specified by index to a certain value
    //stores the modifications in the presolver instead of modifying the model itself
    void fixVariable(int index, Numerical::Double value);

    void presolve();

    void printStatistics();

private:
    Model * m_model;
    unsigned int m_timeLimit;
    unsigned int m_iterationLimit;

    PRESOLVE_MODE m_mode;

    std::vector<PresolverModule*> m_modules;

    std::vector<Vector*> m_substituteVectors;

    std::vector<Variable> * m_variables;
    std::vector<Constraint> * m_constraints;

    //Linked lists storing the rows and columns by nonzero count
    Vector * m_rowNonzeros;
    Vector * m_columnNonzeros;
};

#endif // PRESOLVER_H
