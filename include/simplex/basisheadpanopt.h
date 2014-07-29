/**
 * @file basisheadpanopt.h This file contains the BasisHeadPanOpt class.
 */

#ifndef BASISHEADPANOPT_H
#define BASISHEADPANOPT_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <fstream>
#include <vector>

/**
 * This class describes a unique basis format, the PBF (PanOpt Basis Format).
 * With this we can write, and read the basis into / from a file.
 *
 * @class BasisHeadPanOpt
 */
class BasisHeadPanOpt: public BasisHeadIO {
public:

    /**
     * Dedfault constructor of the class.
     *
     * @constructor
     */
    BasisHeadPanOpt();

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~BasisHeadPanOpt() {}

    /**
     * This function opens the output file, and sets the model.
     * @param fileName name of the output file
     * @param model the model of the LP problem
     */
    void startWrting(const char * fileName,
                     const Model & model);

    /**
     * This function performs the actual writing.
     * It calls the writeGeneralInfo(), writeBasisHeadInfo(), writeNonBasisInfo() functions.
     * After writing is done, it calles the release() function.
     */
    void finishWriting();

    /**
     * Initializes the m_info member with the given parameter.
     * @param info information that will be added
     */
    void addInfo(const char * info);

    /**
     * This function opens the input file, and sets the model.
     * @param fileName name of the input file
     * @param model the model of the LP problem
     */
    void startReading(const char * fileName,
                      const Model & model);

    /**
     * This function performs the actual reading.
     * It uses a hash table and reads the basic and nonbasic variables.
     * After reading is done it calles the release() function.
     *
     * @throw PanOptException if model is missing, or invalid variable / constraint count is present
     */
    void finishReading();

    /**
     * With this function we can add a new basicvariable to the m_basicVariables vector.
     * @param variable pointer of the variable to be added
     * @param basisPosition the basis index of the selected variable
     * @param variableIndex variable index of the selected variable
     * @param value actual value of the variable
     */
    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);

    /**
     * With this function we can add a new nonbasic variable to the m_nonBasicVariables vector.
     * @param variable pointer of the variable to be added
     * @param variableIndex variable index of the selected variable
     * @param state the state (LB,UB) of the choosen variable
     * @param value the actual value of the variable
     */
    void addNonbasicVariable(const Variable * variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);

    /**
     * With this function we can add a new variable to the m_variables vector.
     * @param variable variable to be added
     */
    void addVariable(const Variable & variable);

private:

    /**
     * Maximal row length of the output used in writeBasisHeadInfo().
     */
    static const int sm_maxRowLength = 80;

    /**
     * This struct describes a basicvariable with all connecting information.
     * @struct BasicVariableInfo
     */
    struct BasicVariableInfo {
        unsigned int m_basisIndex;
        unsigned int m_variableIndex;
        Numerical::Double m_value;
        const Variable * m_variablePtr;
        bool m_isLogical;
    };

    /**
     * Struct describing a nonbasic variable with all connecting information.
     * @struct NonBasicVariableInfo
     */
    struct NonBasicVariableInfo {
        Simplex::VARIABLE_STATE m_state;
        unsigned int m_variableIndex;
        Numerical::Double m_value;
        const Variable * m_variablePtr;
        bool m_isLogical;
    };

    /**
     * The output file.
     */
    std::ofstream m_outputFile;

    /**
     * The input file.
     */
    std::ifstream m_inputFile;

    /**
     * Pointer to the model of the LP problem.
     */
    const Model * m_modelPtr;

    /**
     * Vector containing the basicvariables.
     */
    std::vector<BasicVariableInfo> m_basicVariables;

    /**
     * Vector containing the nonbasic variables.
     */
    std::vector<NonBasicVariableInfo> m_nonBasicVariables;

    /**
     * Vector that contains pointers to the variables.
     */
    std::vector<const Variable *> m_variables;

    /**
     * Additional information.
     */
    std::string m_info;

    /**
     * Writes general information to the output file.
     * It contains the name of the LP problem, the user, current time etc.
     */
    void writeGeneralInfo();

    /**
     * Writes all information about the basicvariables to the output file.
     * It contains variable name, type, bounds, logical/structural etc.
     */
    void writeBasisHeadInfo();

    /**
     * Writes all information about the nonbasic variables to the output file.
     * It contains variable name, value, type, bounds etc.
     */
    void writeNonBasisInfo();

    /**
     * Writes the string given in parameter to the output file.
     * @param str the string to be written
     */
    void writeString(const std::string & str);

    /**
     * Writes a double value into the output file.
     * If the value is minus infinity, then it writes "minus_inf".
     * @param value the double value to be written
     */
    void writeDouble(Numerical::Double value);

    /**
     * Function releasing members.
     * It closes the input, output files, and clears the vector members.
     */
    void release();
};

#endif // BASISHEADPANOPT_H
