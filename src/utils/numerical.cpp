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
 * @file numerical.cpp
 */

#include <linalg/linalgparameterhandler.h>

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

const double Numerical::Infinity = (std::numeric_limits<double>::infinity());
//const double Numerical::Infinity = DBL_MAX;

//const double & Numerical::AbsoluteTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
//const double & Numerical::RelativeTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");

double Numerical::AbsoluteTolerance = 0;
double Numerical::RelativeTolerance = 0;

// TODO: lehet hogy pointerekre kell lecserelni oket, es dinamikusan letrehozni
const Numerical::Double ZERO = 0.0;
const Numerical::Double INVALID = -DBL_MAX;

void Numerical::_globalInit() {
    AbsoluteTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
    RelativeTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");

}
