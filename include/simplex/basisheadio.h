/**
 * @file basisheadio.h This file contains the BasisHeadIO class.
 */
#ifndef BASISHEADIO_H
#define BASISHEADIO_H

#include <globals.h>
#include <simplex/simplex.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <lp/panoptstate.h>
#include <utils/indexlist.h>
#include <vector>

/**
 * This abstract class describes a general basishead input output unit.
 * It contains many abstract members that should be implemented in the derived classes.
 * With this we can write (and read) the basic and nonbasic variables into a file.
 *
 * @class BasisHeadIO
 */
class BasisHeadIO: public PanOptState {
public:

    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    BasisHeadIO():m_basisHeadPtr(0), m_variableStatesPtr(0) {}

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~BasisHeadIO() {}

    /**
     * Pure virtual function, adding a new basicvariable to a vector.
     * @param variable pointer to the variable
     * @param basisPosition basis index of the variable
     * @param variableIndex variable index of the choosen variable
     * @param value value of the selected variable
     */
    virtual void addBasicVariable(const Variable * variable,
                                  unsigned int basisPosition,
                                  unsigned int variableIndex,
                                  Numerical::Double value) = 0;

    /**
     * Pure virtual function, adding a new nonbasic variable to a vector.
     * @param variable pointer to the variable to be added
     * @param variableIndex variable index of the chosen variable
     * @param state state of the nonbasic variable (lower/upper bound)
     * @param value value of the nonbasic variable
     */
    virtual void addNonbasicVariable(const Variable * variable,
                                     unsigned int variableIndex,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value) = 0;

    /**
     * Pure virtual function, adding a new variable to a vector.
     * @param variable variable to be added
     */
    virtual void addVariable(const Variable & variable) = 0;

    /**
     * Setter for the basishead.
     * @param basisHead pointer to the basishead to be set.
     */
    void setBasisHead(std::vector<int> * basisHead) {
        m_basisHeadPtr = basisHead;
    }

    /**
     * Setter for the list containing the variable states.
     * @param variableStates pointer of the inexlist to be set.
     */
    void setVariableStateList(IndexList<const Numerical::Double*> * variableStates) {
        m_variableStatesPtr = variableStates;
    }

protected:

    /**
     * Pointer of the basishead (containing variable indices of basicvariables)
     */
    std::vector<int> * m_basisHeadPtr;

    /**
     * Pointer of indexlist holding the variable states.
     */
    IndexList<const Numerical::Double*> * m_variableStatesPtr;
};

#endif // BASISHEADIO_H
