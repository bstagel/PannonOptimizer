#ifndef ERRORANALYZERDOUBLE_H
#define ERRORANALYZERDOUBLE_H

class ErrorAnalyzerDouble {
public:
    ErrorAnalyzerDouble();

    ErrorAnalyzerDouble(double value, double relative = 0, double absolute = 0);

    ErrorAnalyzerDouble operator+(const ErrorAnalyzerDouble & v) const;

    ErrorAnalyzerDouble & operator+=(const ErrorAnalyzerDouble & v);

    ErrorAnalyzerDouble operator-(const ErrorAnalyzerDouble & v) const;

    ErrorAnalyzerDouble & operator-=(const ErrorAnalyzerDouble & v);

    ErrorAnalyzerDouble operator*(const ErrorAnalyzerDouble & v) const;

    ErrorAnalyzerDouble & operator*=(const ErrorAnalyzerDouble & v);

    ErrorAnalyzerDouble operator/(const ErrorAnalyzerDouble & v) const;

    ErrorAnalyzerDouble & operator/=(const ErrorAnalyzerDouble & v);

    double getRelativeError() const;

    double getAbsoluteError() const;

    double getValue() const;

private:
    double m_value;
    double m_relativeError;
    double m_absoluteError;

};

#endif // ERRORANALYZERDOUBLE_H

