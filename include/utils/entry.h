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
