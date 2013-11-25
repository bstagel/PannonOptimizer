#ifndef REPORTENTRY_H
#define REPORTENTRY_H

union ReportEntry {
    std::string * m_string;
    int m_integer;
    double m_float;
    bool m_bool;
};

#endif // REPORTENTRY_H
