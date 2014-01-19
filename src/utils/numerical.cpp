/**
 * @file numerical.cpp
 */

#include <linalg/linalgparameterhandler.h>

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

const Numerical::Double Numerical::Infinity = (std::numeric_limits<double>::infinity());

const Numerical::Double Numerical::AbsoluteTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
const Numerical::Double Numerical::RelativeTolerance = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");

const Numerical::Double ZERO = 0.0;
const Numerical::Double INVALID = -DBL_MAX;
