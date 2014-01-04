#ifndef ENTRY_H
#define ENTRY_H

#include <string>


union Entry {
    enum ENTRY_TYPE{
        STRING,
        INTEGER,
        DOUBLE,
        BOOL
    };

    std::string* m_string;
    int m_integer;
    double m_double;
    bool m_bool;
};

#endif // ENTRY_H
