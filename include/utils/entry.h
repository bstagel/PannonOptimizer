#ifndef ENTRY_H
#define ENTRY_H

union Entry {
    std::string * m_string;
    int m_integer;
    double m_float;
    bool m_bool;
};

#endif // ENTRY_H
