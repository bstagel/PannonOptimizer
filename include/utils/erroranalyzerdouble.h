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

