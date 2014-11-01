/**
 * @file numerical.cpp
 */

#include <linalg/linalgparameterhandler.h>

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

const double Numerical::Infinity = (std::numeric_limits<double>::infinity());
//const Numerical::Double Numerical::Infinity = DBL_MAX;

//const Numerical::Double & Numerical::AbsoluteTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
//const Numerical::Double & Numerical::RelativeTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");

double Numerical::AbsoluteTolerance = 0;
double Numerical::RelativeTolerance = 0;

// TODO: lehet hogy pointerekre kell lecserelni oket, es dinamikusan letrehozni
const Numerical::Double ZERO = 0.0;
const Numerical::Double INVALID = -DBL_MAX;

void Numerical::_globalInit() {
    AbsoluteTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
    RelativeTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");

}
