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

#ifndef PNSINTERFACE_H
#define PNSINTERFACE_H

#include <string>

class SimplexController;
class ManualModelBuilder;
class Model;

class PnsInterface {
public:
    PnsInterface();
    ~PnsInterface();
    /** Just like the other loadProblem() method except that the matrix is
      given in a standard column major ordered format (without gaps). */
    void loadProblem (  const int numcols, const int numrows,
                        const int* columnStartIndex, const int* index,
                        const double* value,
                        const double* collb, const double* colub,
                        const double* obj,
                        const double* rowlb, const double* rowub);
    /**
     * Amount of print out
     */
     void setLogLevel(int value);
    /**
     * Set name of row
     */
    void setRowName(int rowIndex, std::string & name) ;
    /**
     * Set name of col
     */
    void setColumnName(int colIndex, std::string & name) ;
    /**
     * Change row lower bounds
     */
    void chgRowLower(const double * rowLower);
    /**
     * Change row upper bounds
     */
    void chgRowUpper(const double * rowUpper);
    /**
     * Change column lower bounds
     */
    void chgColumnLower(const double * columnLower);
    /**
     * Change column upper bounds
     */
    void chgColumnUpper(const double * columnUpper);
    /**
     * Change objective coefficients
     */
    void chgObjCoefficients(const double * objIn);
    /**
     * Write the problem in MPS format to the specified file.
     *
     * Row and column names may be null.
     * formatType is
     * <ul>
     *   <li> 0 - normal
     *   <li> 1 - extra accuracy
     *   <li> 2 - IEEE hex
     * </ul>
     * Returns non-zero on I/O error
     */
    int writeMps(const char *filename,
                 int formatType = 0, int numberAcross = 2,
                 double objSense = 0.0) const ;
    /**
     * Dual solve
     */
    int dual();
    /**
     * Is optimality proven?
     */
    bool isProvenOptimal() const;

    /**
     * Primal column solution
     */
    const double * primalColumnSolution() const;

    /**
     * Gives the reduced costs
     */
    const double * getReducedCost() const;

private:
    SimplexController* m_simplex;
    ManualModelBuilder* m_builder;
    Model* m_model;
};

#endif // PNSINTERFACE_H
