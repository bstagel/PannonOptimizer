/**
 * @file numerical.cpp
 */

#include <linalg/linalgparameterhandler.h>

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

//TODO: INIT PARAMETERS!
const double e_absolute = LinalgParameterHandler::getInstance().getParameterValue("e_absolute");
const double e_relative = LinalgParameterHandler::getInstance().getParameterValue("e_relative");
//const double e_feasibility = 0.0; //SimplexParameters::getParameterValue("e_feasibility");
//const double e_optimality = 0.0; //SimplexParameters::getParameterValue("e_optimality");

const Numerical::Double Numerical::Infinity = (std::numeric_limits<double>::infinity());

const Numerical::Double Numerical::AbsoluteTolerance = e_absolute;
const Numerical::Double Numerical::RelativeTolerance = e_relative;

//const Numerical::Double Numerical::FeasibilityTolerance = e_feasibility;
//const Numerical::Double Numerical::OptimalityTolerance = e_optimality;
