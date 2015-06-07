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
 * @file entry.h This file describes the Entry type.
 * @author Jozsef Smidla
 */

#ifndef ENTRY_H
#define ENTRY_H

#include <string>

/**
 * This type describes a general type for entries of any type.
 * Entries can be handled together, independently of their stored
 * variable type.
 */
struct Entry {

    Entry(): m_string(nullptr),
             m_integer(0),
             m_double(0.0),
             m_bool(false) {

    }

    /**
     * This type describes a variable type.
     */
    enum ENTRY_TYPE{
        STRING,
        INTEGER,
        DOUBLE,
        BOOL
    };

    /**
     * The entry data as a string.
     */
    std::string* m_string;

    /**
     * The entry data as an integer.
     */
    int m_integer;

    /**
     * The entry data as a double.
     */
    double m_double;

    /**
     * The entry data as a bool.
     */
    bool m_bool;
};

#endif // ENTRY_H
