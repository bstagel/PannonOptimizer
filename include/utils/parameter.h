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
 * @file parameter.h This file contains the API of the Parameter class
 * @author Peter Tar
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <globals.h>
#include <string>
#include <cstring>
#include <ostream>

#include <utils/exceptions.h>
#include <utils/entry.h>

/**
 * This class represents a parameter of the program.
 * Parameters can be changed in an external file without having to rebuild the program.
 *
 * @class Parameter
 */
class Parameter {
public:

    Parameter(): m_name(""), m_comment(""), m_type(Entry::BOOL) {
        panOptMemcpy(&m_entry, 0, sizeof(m_entry));
    }

    /**
     * Constructor of the Parameter class.
     *
     * @param name The name of the parameter.
     * @param type The type of the parameter.
     */
    Parameter(const std::string& name, const Entry::ENTRY_TYPE type, const std::string & comment) :
        m_name(name), m_comment(comment), m_type(type) {
        memset(&m_entry, 0, sizeof(m_entry));
        if(m_type == Entry::STRING){
            m_entry.m_string = new std::string("");
        }
    }

    /**
     * Copy constructor of the Parameter class.
     *
     * @param other The other Parameter object to be copied.
     */
    Parameter(const Parameter& other) :
        m_name(other.m_name),
        m_comment(other.m_comment),
        m_entry(other.m_entry),
        m_type(other.m_type){
        if(m_type == Entry::STRING){
            m_entry.m_string = new std::string("");
            *(m_entry.m_string) = *(other.m_entry.m_string);
        }
    }

    Parameter & operator=(const Parameter & other) {
        if (this == &other) {
            return *this;
        }
        if(m_type == Entry::STRING){
            delete m_entry.m_string;
            m_entry.m_string = nullptr;
        }
        m_name = other.m_name;
        m_comment = other.m_comment;
        m_entry = other.m_entry;
        m_type = other.m_type;
        if(m_type == Entry::STRING){
            m_entry.m_string = new std::string("");
            *(m_entry.m_string) = *(other.m_entry.m_string);
        }
        return *this;
    }

    /**
     * Destructor of the Parameter class.
     *
     * @destructor
     */
    ~Parameter() {
        if(m_type == Entry::STRING){
            delete m_entry.m_string;
            m_entry.m_string = nullptr;
        }
    }

    /**
     * Returns the name of the parameter.
     *
     * @return The name of the parameter.
     */
    const std::string& getName() const {
        return m_name;
    }

    /**
     * Sets the name of the parameter.
     *
     * @param name The name of the parameter.
     */
    void setName(const std::string & name) {
        m_name = name;
    }

    /**
     * Returns the comment of the parameter.
     *
     * @return The comment of the parameter.
     */
    const std::string & getComment() const {
        return m_comment;
    }

    /**
     * Returns the entry data of the parameter.
     *
     * @see Entry
     * @return The entry data of the parameter.
     */
    const Entry & getEntry() const { return m_entry; }

    /**
     * Returns the type of the parameter.
     *
     * @return The type of the parameter.
     */
    Entry::ENTRY_TYPE getEntryType() const { return m_type; }

    /**
     * Sets the value of the parameter as a string.
     *
     * @throws ParameterException if the type of the parameter is not string.
     * @param value The new value of the parameter.
     */
    void setStringValue(const std::string value){
        /*if(m_type != Entry::STRING){
            throw ParameterException("Invalid parameter value");
        }*/
        m_type = Entry::STRING;
        *(m_entry.m_string) = value;
    }

    /**
     * Sets the value of the parameter as an integer.
     *
     * @throws ParameterException if the type of the parameter is not integer.
     * @param value The new value of the parameter.
     */
    void setIntegerValue(const int value){
        /*if(m_type != Entry::INTEGER){
            throw ParameterException("Invalid parameter value");
        }*/
        m_type = Entry::INTEGER;
        m_entry.m_integer = value;
    }

    /**
     * Sets the value of the parameter as a double.
     *
     * @throws ParameterException if the type of the parameter is not double.
     * @param value The new value of the parameter.
     */
    void setDoubleValue(const double value){
        /*if(m_type != Entry::DOUBLE){
            throw ParameterException("Invalid parameter value");
        }*/
        m_type = Entry::DOUBLE;
        m_entry.m_double = value;
    }

    /**
     * Sets the value of the parameter as a boolean.
     *
     * @throws ParameterException if the type of the parameter is not boolean.
     * @param value The new value of the parameter.
     */
    void setBoolValue(const bool value){
        /*if(m_type != Entry::BOOL){
            throw ParameterException("Invalid parameter value");
        }*/
        m_type = Entry::BOOL;
        m_entry.m_bool = value;
    }

    /**
     * Prints the name and value of the parameter to the output.
     *
     * @param os The output stream to print to.
     * @param param The parameter to be printed.
     * @return The output stream with the parameter printed on it.
     */
    friend std::ostream& operator<<(std::ostream& os, const Parameter& param){
        os << param.m_name << " = ";
        if(param.m_type == Entry::STRING){
            os << param.m_entry.m_string << std::endl;
        } else if (param.m_type == Entry::INTEGER){
            os << param.m_entry.m_integer << std::endl;
        } else if (param.m_type == Entry::DOUBLE){
            os << param.m_entry.m_double << std::endl;
        } else if (param.m_type == Entry::BOOL){
            os << param.m_entry.m_bool << std::endl;
        }
        return os;
    }

private:

    /**
     * The name of the parameter.
     */
    std::string m_name;

    /**
     * The comment, or documentation of the parameter.
     */
    std::string m_comment;

    /**
     * The entry data of the parameter.
     */
    Entry m_entry;

    /**
     * The type of the parameter.
     */
    Entry::ENTRY_TYPE m_type;
};

#endif /* PARAMETER_H_ */
