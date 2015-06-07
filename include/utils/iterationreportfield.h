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
 * @file iterationreportfield.h This file contains the API of the IterationReportField class.
 * @author Jozsef Smidla
 */

#ifndef ITERATIONREPORTFIELD_H
#define ITERATIONREPORTFIELD_H

#include <globals.h>
#include <string>

class IterationReportProvider;

/**
 * This class describes a field of the iteration report.
 * This description contains the name of the field and its format to get from the entries.
 *
 * @see IterationReport
 * @class IterationReportField
 */
class IterationReportField {
public:

    /**
     * This type describes an alignment in an output field.
     */
    enum ITERATION_REPORT_FIELD_ALIGNMENT {
        IRF_LEFT,
        IRF_RIGHT,
        IRF_CENTER
    };

    /**
     * This type describes the desired type of a field entry.
     */
    enum ITERATION_REPORT_FIELD_TYPE {
        IRF_INT,
        IRF_FLOAT,
        IRF_STRING,
        IRF_BOOL
    };

    /**
     * This type describes the output format of a floating point number.
     */
    enum ITERATION_REPORT_FLOAT_FORMAT {
        IRF_FIXED,
        IRF_SCIENTIFIC,
        IRF_NONE
    };

    /**
     * Constructor of the IterationReportField class.
     * Needs to specify all the data associated the the field.
     *
     * @param name The name of the iteration report field.
     * @param width The width of the field when printed to the output.
     * @param debugLevel The debug level of the field.
     * @param align The alignment of the entry in the field.
     * @param type The type of the entry in the field.
     * @param provider The iteration report provider of the field.
     * @param precision The floating point number precision of the field.
     * @param floatFormat The floating point number format of the field.
     */
    IterationReportField(const std::string & name,
                         unsigned int width,
                         int debugLevel,
                         ITERATION_REPORT_FIELD_ALIGNMENT align,
                         ITERATION_REPORT_FIELD_TYPE type,
                         const IterationReportProvider & provider,
                         int precision = -1,
                         ITERATION_REPORT_FLOAT_FORMAT floatFormat = IRF_NONE):
        m_name(name),
        m_width(width < name.length() ? name.length() : width),
        m_debugLevel(debugLevel),
        m_align(align),
        m_type(type),
        m_provider(&provider),
        m_precision(precision),
        m_floatFormat(floatFormat)
    {

    }

    /**
     * Returns true if the two report fields are the same.
     *
     * @param other The other iteration report field to be compared.
     * @return True if the two report fields are the same.
     */
    bool operator==(const IterationReportField & other) const {
        return m_name == other.m_name && &m_provider == &other.m_provider;
    }

    /**
     * Returns the name of the field.
     *
     * @return The name of the field.
     */
    const std::string & getName() const {
        return m_name;
    }

    /**
     * Returns the output width of the field.
     *
     * @return The output width of the field.
     */
    unsigned int getWidth() const {
        return m_width;
    }

    /**
     * Returns the debug level of the field.
     *
     * @return The debug level of the field.
     */
    int getDebugLevel() const {
        return m_debugLevel;
    }

    /**
     * Returns the alignment of the entry data in the field.
     *
     * @return The alignment of the entry data in the field.
     */
    ITERATION_REPORT_FIELD_ALIGNMENT getAlignment() const {
        return m_align;
    }

    /**
     * Returns the type of entry data in the field.
     *
     * @return The type of entry data in the field.
     */
    ITERATION_REPORT_FIELD_TYPE getType() const {
        return m_type;
    }

    /**
     * Returns the report provider of the field.
     *
     * @return The report provider of the field.
     */
    const IterationReportProvider & getProvider() const {
        return *m_provider;
    }

    /**
     * Returns the floating point number precision of the field.
     *
     * @return The floating point number precision of the field.
     */
    int getPrecision() const {
        return m_precision;
    }

    /**
     * Returns the floating point number format of the field.
     *
     * @return The floating point number format of the field.
     */
    ITERATION_REPORT_FLOAT_FORMAT getFloatFormat() const {
        return m_floatFormat;
    }

private:

    /**
     * The name of the field.
     */
    std::string m_name;

    /**
     * The output width of the field.
     */
    unsigned int m_width;

    /**
     * The debug level of the field.
     */
    int m_debugLevel;

    /**
     * The alignment of entry data in the field.
     */
    ITERATION_REPORT_FIELD_ALIGNMENT m_align;

    /**
     * The type of entry data in the field.
     */
    ITERATION_REPORT_FIELD_TYPE m_type;

    /**
     * The report provider of the field.
     */
    const IterationReportProvider * m_provider;

    /**
     * The floating point number precision in the field.
     */
    int m_precision;

    /**
     * The floating point number format in the field.
     */
    ITERATION_REPORT_FLOAT_FORMAT m_floatFormat;
};

#endif // ITERATIONREPORTFIELD_H
