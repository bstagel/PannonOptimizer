//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

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
 * With this we can write and read the basis into / from a file.
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
    * This function opens the output file, and initializes the input/output unit with the model.
    *
    * @param fileName The name of the output file.
    * @param model The model of the LP problem.
    */
    void startWrting(const char * fileName,
                     const Model & model);

    /**
     * This function writes the PanOpt basis information to the output file.
     * After the writing is done, it releases the dynamically allocated data members and closes the used streams.
     */
    void finishWriting();

    /**
     * Initializes the m_info member with the given parameter.
     *
     * @param info The information to be added.
     */
    void addInfo(const char * info);

    /**
     * This function opens the input file, and sets up the model to be loaded.
     *
     * @param fileName The name of the input file.
     * @param model The model of the LP problem.
     */
    void startReading(const char * fileName,
                      const Model & model);

    /**
     * This function reads the basic and nonbasic variables with the help of a hash table.
     * After the reading is done, it releases the dynamically allocated data members and closes the used streams.
     *
     * @throw PanOptException if model is missing, or invalid variable / constraint count is present.
     */
    void finishReading();

    /**
     * Adds a basic variable to the m_basicVariables vector.
     *
     * @param variable Pointer to the variable to be added.
     * @param basisPosition The basis position on which the variable will be added.
     * @param variableIndex The index of the variable to be added.
     * @param value The current value of the variable to be added.
     */
    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);

    /**
     * Adds a nonbasic variable to the m_nonBasicVariables vector.
     *
     * @param variable Pointer to the variable to be added.
     * @param variableIndex The index of the variable to be added.
     * @param state Describes whether the nonbasic variable is on its lower or upper bound.
     * @param value The current value of the variable to be added.
     */
    void addNonbasicVariable(const Variable * variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);

    /**
     * Adds a specific variable to the m_variables vector.
     *
     * @param variable The variable to be added.
     */
    void addVariable(const Variable & variable);

private:

    /**
     * Maximal row length of the output used in writeBasisHeadInfo().
     */
    static const int sm_maxRowLength = 80;

    /**
     * This struct describes a basic variable with all connecting information.
     *
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
     *
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
     * The output file stream.
     */
    std::ofstream m_outputFile;

    /**
     * The input file stream.
     */
    std::ifstream m_inputFile;

    /**
     * Pointer to the model of the LP problem.
     */
    const Model * m_modelPtr;

    /**
     * Vector containing the basic variables.
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
     * Writes all information about the basic variables to the output file.
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
     * @param str The string to be written.
     */
    void writeString(const std::string & str);

    /**
     * Writes a double value into the output file.
     *
     * @param value The double value to be written.
     */
    void writeDouble(Numerical::Double value);

    /**
     * Releases the data members of the input/output unit.
     * It closes the input, output files, and clears the vector members.
     */
    void release();
};

#endif // BASISHEADPANOPT_H
