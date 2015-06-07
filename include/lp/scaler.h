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

#ifndef SCALER_H
#define SCALER_H

#include <vector>
#include <linalg/matrix.h>
#include <utils/numerical.h>

class Scaler {
public:
    void scale(Matrix * matrix, const char *name = 0);

    const std::vector<Numerical::Double> & getRowMultipliers() const;

    const std::vector<Numerical::Double> & getColumnMultipliers() const;

private:
    std::vector<Numerical::Double> m_rowMultipliers;
    std::vector<Numerical::Double> m_columnMultipliers;

    void benichou(Matrix *matrix, const char * name = 0);

    Numerical::Double getVarianceBenichou(Matrix * matrix,
                                          std::vector<Numerical::Double> * rowMultpiliersPtr);

    Numerical::Double roundPowerOf2(Numerical::Double value) const;
};

#endif // SCALER_H
