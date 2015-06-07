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
 * @file iterationreportprovider.h This file contains the IterationReportProvider class.
 */
#ifndef ITERATIONREPORTPROVIDER_H
#define ITERATIONREPORTPROVIDER_H

#include <globals.h>
#include <utils/iterationreportfield.h>
#include <utils/entry.h>
#include <vector>
#include <string>

/**
 * Describes a general object that provides information, that the iterationreporter should write.
 * Those classes that have any information the iteration reporter should write should be derived from this class.
 * In the derived classes the pure virtual functions should be implemented.
 *
 * @class IterationReportProvider
 */
class IterationReportProvider {
public:

    /**
     * Describes the possible types of one iteration report field.
     */
    enum ITERATION_REPORT_FIELD_TYPE {
        IRF_START,
        IRF_ITERATION,
        IRF_SOLUTION,
        IRF_EXPORT
    };

    /**
     * Function for returning the iteration report fields of one derived class.
     * This function should be implemented in order to be able to provide information
     * to the iteration reporter. The implementation should be done for every
     * IterationReportProvider::ITERATION_REPORT_FIELD_TYPE. The fields will be stored
     * in a vector that shall be returned.
     * @param type holds the type of the iteration report field
     * @return vector containing the iteration report fields of the derived class
     * @throws ParameterException if the export type in the parameter file is invalid
     */
    virtual std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

    /**
     * This function is needed to be able to get an iteration entry in run-time.
     * This function should be implemented in order to be able to provide information
     * to the iteration reporter. The implementation should be done for every
     * IterationReportProvider::ITERATION_REPORT_FIELD_TYPE. The information will be stored
     * in a reply Entry, which is returned.
     * @param name holds the name of the iteration report field
     * @param type holds the type of the iteration report field
     * @return reply Entry containing information for the reporter.
     */
    virtual Entry getIterationEntry(const std::string & name, enum ITERATION_REPORT_FIELD_TYPE & type) const = 0;

};

#endif // ITERATIONREPORTPROVIDER_H
