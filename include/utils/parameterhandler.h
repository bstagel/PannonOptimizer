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
 * @file parameterhandler.h This file contains the API of the ParameterHandler class.
 * @author Peter Tar
 */

#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <globals.h>

#include <map>
#include <vector>
#include <utils/parameter.h>
#include <utils/entry.h>
#include <utils/nodefile.h>

/**
 * This class is able to handle the parameters of the program.
 *
 * @see Parameter
 * @class ParameterHandler
 */
class ParameterHandler{
    friend class LinalgParameterHandlerTestSuite;
    friend class SimplexParameterHandlerTestSuite;

public:

    /**
     * Destructor of the ParameterHandler class.
     *
     * @destructor
     */
    virtual ~ParameterHandler();

    /**
     * Returns true if the parameter with the given name is already specified.
     *
     * @param name The name of the parameter to be queried.
     * @return True if the parameter is already set.
     */
    inline bool hasParameter(const std::string& name) const {return m_values.count(name)>0;}

    /**
     * Creates a new parameter.
     *
     * @param name The name of the new parameter.
     * @param type The type of the new parameter.
     */
    inline void createParameter(const std::string& name,
                                Entry::ENTRY_TYPE type,
                                const std::string & comment){
        if (m_values.find(name) == m_values.end()) {
            m_valueNames.push_back(name);
        }
        m_values.insert(std::pair<std::string, Parameter>(name, Parameter(name,type,comment)));
    }

    virtual bool validateParameterValue(const std::string& parameter,
                                        const std::string& value) = 0;

    /**
     * Returns the type of the given parameter.
     *
     * @param name The name of the parameter.
     * @return The type of the parameter.
     */
    inline Entry::ENTRY_TYPE getParameterType(const std::string& name) const {
        return m_values.at(name).getEntryType();}

    /**
     * Returns the value of the given parameter as a string.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual inline const std::string & getStringParameterValue(const std::string& name) const {
        if (m_values.find(name) == m_values.end()) {
            LPERROR(name);
        }
        if (m_values.at(name).getEntryType() != Entry::STRING) {
            LPERROR("not string: " << name);
        }
        return *(m_values.at(name).getEntry().m_string);
    }

    /**
     * Returns the value of the given parameter as an integer.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual inline const int & getIntegerParameterValue(const std::string& name) const {
        if (m_values.find(name) == m_values.end()) {
            LPERROR(name);
        }
        if (m_values.at(name).getEntryType() != Entry::INTEGER) {
            LPERROR("not integer: " << name);
        }
        return m_values.at(name).getEntry().m_integer;
    }

    /**
     * Returns the value of the given parameter as a double.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual inline const double & getDoubleParameterValue(const std::string& name) const {
        if (m_values.find(name) == m_values.end()) {
            LPERROR(name);
        }
        if (m_values.at(name).getEntryType() != Entry::DOUBLE) {
            LPERROR("not double: " << name);
        }
        return m_values.at(name).getEntry().m_double;
    }

    /**
     * Returns the value of the given parameter as a boolean.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual inline const bool & getBoolParameterValue(const std::string& name) const {
        if (m_values.find(name) == m_values.end()) {
            LPERROR(name);
        }
        if (m_values.at(name).getEntryType() != Entry::BOOL) {
            LPERROR("not bool: " << name);
        }
        return m_values.at(name).getEntry().m_bool;
    }

    /**
     * Sets the given parameter's value to a double.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual inline void setParameterValue(const std::string name, const double value){
        m_values.at(name).setDoubleValue(value);
    }

    /**
     * Sets the given parameter's value to an integer.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual inline void setParameterValue(const std::string name, const int value){
        m_values.at(name).setIntegerValue(value);
    }

    /**
     * Sets the given parameter's value to a string.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual inline void setParameterValue(const std::string name, const std::string value){
        m_values.at(name).setStringValue(value);
    }

    /**
     * Sets the given parameter's value to a string.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual inline void setParameterValue(const std::string name, const char * value){
        m_values.at(name).setStringValue(std::string(value));
    }

    /**
     * Sets the given parameter's value to a boolean.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual inline void setParameterValue(const std::string name, const bool value){
        m_values.at(name).setBoolValue(value);
    }

    /**
     * Reads in a given file and stores its content as parameters.
     *
     * @param filename The name of the file.
     */
    virtual void readParameterFile(const char * filename);

    /**
     * Returns the given parameter's value formatted as a string.
     *
     * @param name The name of the parameter.
     * @return The parameter's value formatted as a string.
     */
    const std::string writeParameter(std::string name);

    /**
     * Exports the current parameters with their values to an output file.
     */
    virtual void writeParameterFile() = 0;

    /**
     * Initializes the parameters to their default values.
     */
    virtual void initParameters() = 0;

    /**
     * Returns the name of the parameter file associated with the handler.
     *
     * @return The name of the parameter file associated with the handler.
     */
    inline std::string getFileName() const {return m_filename;}

    /**
     * Sets the name of the parameter file associated with the handler to a new value.
     *
     * @return The name of the new parameter file.
     */
    inline void setFileName(std::string filename) {m_filename = filename;}

    /**
     * Prints the parameters to the output.
     */
    void showParameters() const;

protected:

    /**
     * The name of the parameter file associated with the handler.
     */
    std::string m_filename;

    /**
     * The parameters with name and values.
     */
    std::map<std::string, Parameter> m_values;

    /**
     * This vector stores the value names in the original ordering.
     */
    std::vector<std::string> m_valueNames;

    /**
     * The original node file.
     */
    NodeFile m_nodeFile;

    /**
     * Loads the parameters from an external file.
     *
     * @param in The input file stream of the file.
     */
    virtual void loadValuesFromFile(std::ifstream& in);

    void processNode(const NodeFile::Node &node, const std::string &name = "");

};

#endif // PARAMETERHANDLER_H
