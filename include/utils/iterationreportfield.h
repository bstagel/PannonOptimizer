#ifndef ITERATIONREPORTFIELD_H
#define ITERATIONREPORTFIELD_H

#include <globals.h>
#include <string>

class IterationReportProvider;

class IterationReportField {
public:
    enum ITERATION_REPORT_FIELD_ALIGNMENT {
        IRF_LEFT,
        IRF_RIGHT,
        IRF_CENTER
    };

    enum ITERATION_REPORT_FIELD_TYPE {
        IRF_INT,
        IRF_FLOAT,
        IRF_STRING,
        IRF_BOOL
    };

    enum ITERATION_REPORT_FLOAT_FORMAT {
        IRF_FIXED,
        IRF_SCIENTIFIC,
        IRF_NONE
    };

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

    bool operator==(const IterationReportField & other) const {
        return m_name == other.m_name && &m_provider == &other.m_provider;
    }

    const std::string & getName() const {
        return m_name;
    }

    unsigned int getWidth() const {
        return m_width;
    }

    int getDebugLevel() const {
        return m_debugLevel;
    }

    ITERATION_REPORT_FIELD_ALIGNMENT getAlignment() const {
        return m_align;
    }

    ITERATION_REPORT_FIELD_TYPE getType() const {
        return m_type;
    }

    const IterationReportProvider & getProvider() const {
        return *m_provider;
    }

    int getPrecision() const {
        return m_precision;
    }

    ITERATION_REPORT_FLOAT_FORMAT getFloatFormat() const {
        return m_floatFormat;
    }

private:
    std::string m_name;
    unsigned int m_width;

    int m_debugLevel;
    ITERATION_REPORT_FIELD_ALIGNMENT m_align;
    ITERATION_REPORT_FIELD_TYPE m_type;
    const IterationReportProvider * m_provider;
    int m_precision;
    ITERATION_REPORT_FLOAT_FORMAT m_floatFormat;
};

#endif // ITERATIONREPORTFIELD_H
