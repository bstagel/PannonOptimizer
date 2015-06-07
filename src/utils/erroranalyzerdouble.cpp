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

#include <utils/erroranalyzerdouble.h>
#include <cmath>


ErrorAnalyzerDouble::ErrorAnalyzerDouble()
{
    m_value = 0.0;
    m_relativeError = 0.0;
    m_absoluteError = 0.0;
}

ErrorAnalyzerDouble::ErrorAnalyzerDouble(double value, double relative, double absolute)
{
    m_value = value;
    m_relativeError = relative;
    m_absoluteError = absolute;
}

ErrorAnalyzerDouble ErrorAnalyzerDouble::operator+(const ErrorAnalyzerDouble &v) const
{
    if (m_value * v.m_value > 0.0) {
        long double longSum = (long double)m_value + (long double)v.m_value;
        double sum = m_value + v.m_value;
        long double longAbsError = fabs(longSum - sum);
        // TODO: vegiggondolni, lehet hogy az absolut hiba kell majd, es abbol
        // visszavezetni a relativat
        //long double longRelError = longSum != 0.0 ? longAbsError / longSum : 0.0;

        double absoluteError = m_absoluteError + v.m_absoluteError + longAbsError;
        //double relativeError =
        return ErrorAnalyzerDouble(sum, 0.0, absoluteError);
    } else {
        return operator-(v);
    }
}

ErrorAnalyzerDouble &ErrorAnalyzerDouble::operator+=(const ErrorAnalyzerDouble &v)
{
    long double longSum = (long double)m_value + (long double)v.m_value;
    double sum = m_value + v.m_value;
    long double longAbsError = fabs(longSum - sum);
    m_absoluteError += v.m_absoluteError + longAbsError;
    m_value = sum;
    return *this;
}

ErrorAnalyzerDouble ErrorAnalyzerDouble::operator-(const ErrorAnalyzerDouble &v) const
{
    long double longSum = (long double)m_value - (long double)v.m_value;
    double sum = m_value - v.m_value;
    long double longAbsError = fabs(longSum - sum);
    double absoluteError = m_absoluteError + v.m_absoluteError + longAbsError;
    return ErrorAnalyzerDouble(sum, 0.0, absoluteError);
}

ErrorAnalyzerDouble &ErrorAnalyzerDouble::operator-=(const ErrorAnalyzerDouble &v)
{
    long double longSum = (long double)m_value - (long double)v.m_value;
    double sum = m_value - v.m_value;
    long double longAbsError = fabs(longSum - sum);
    m_absoluteError += v.m_absoluteError + longAbsError;
    m_value = sum;
    return *this;
}

ErrorAnalyzerDouble ErrorAnalyzerDouble::operator*(const ErrorAnalyzerDouble &v) const
{
    long double longProd = (long double)m_value * (long double)v.m_value;
    double prod = m_value * v.m_value;
    long double longAbsError = fabs(longProd - prod);
    double absoluteError = m_absoluteError * fabs(v.m_value) + v.m_absoluteError * fabs(m_value) +
            m_absoluteError * v.m_absoluteError + longAbsError;
    return ErrorAnalyzerDouble(prod, 0.0, absoluteError);
}

ErrorAnalyzerDouble &ErrorAnalyzerDouble::operator*=(const ErrorAnalyzerDouble &v)
{
    long double longProd = (long double)m_value * (long double)v.m_value;
    double prod = m_value * v.m_value;
    long double longAbsError = fabs(longProd - prod);
    m_absoluteError += v.m_absoluteError + longAbsError;
    m_value = prod;
    return *this;
}

ErrorAnalyzerDouble ErrorAnalyzerDouble::operator/(const ErrorAnalyzerDouble &v) const
{
    long double longRatio = (long double)m_value / (long double)v.m_value;
    double ratio = m_value / v.m_value;
    long double longAbsError = fabs(longRatio - ratio);
    double absoluteError = (m_absoluteError * fabs(v.m_value) + v.m_absoluteError * fabs(m_value)) /
            (v.m_value * (fabs(v.m_value) - v.m_absoluteError)) + longAbsError;
    return ErrorAnalyzerDouble(ratio, 0.0, absoluteError);
}

ErrorAnalyzerDouble &ErrorAnalyzerDouble::operator/=(const ErrorAnalyzerDouble &v)
{
    long double longRatio = (long double)m_value / (long double)v.m_value;
    double ratio = m_value / v.m_value;
    long double longAbsError = fabs(longRatio / ratio);
    m_absoluteError = (m_absoluteError * fabs(v.m_value) + v.m_absoluteError * fabs(m_value)) /
            (v.m_value * (fabs(v.m_value) - v.m_absoluteError)) + longAbsError;
    m_value = ratio;
    return *this;
}

double ErrorAnalyzerDouble::getRelativeError() const
{
    return m_relativeError;
}

double ErrorAnalyzerDouble::getAbsoluteError() const
{
    return m_absoluteError;
}

double ErrorAnalyzerDouble::getValue() const
{
    return m_value;
}
