/**
 * @file numerical.cpp
 */

#include <linalg/linalgparameterhandler.h>

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

const double e_absolute = LinalgParameterHandler::getInstance().getParameterValue("e_absolute");
const double e_relative = LinalgParameterHandler::getInstance().getParameterValue("e_relative");

const Numerical::Double Numerical::Infinity = (std::numeric_limits<double>::infinity());

const Numerical::Double Numerical::AbsoluteTolerance = e_absolute;
const Numerical::Double Numerical::RelativeTolerance = e_relative;

