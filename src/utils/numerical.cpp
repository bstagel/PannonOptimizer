/**
 * @file numerical.cpp
 * @author Mark Korondi <korondi@dcs.uni-pannon.hu>
 * @date 01/03/2010
 */

#include <utils/numerical.h>
//#include <simplex/simplexparameters.h>

bool _debug_flag = false;

const double e_absolute = 0.0; // SimplexParameters::getParameterValue("e_absolute");
const double e_relative = 0.0; // SimplexParameters::getParameterValue("e_relative");
const double e_feasibility = 0.0; //SimplexParameters::getParameterValue("e_feasibility");
const double e_optimality = 0.0; //SimplexParameters::getParameterValue("e_optimality");

const Numerical::Double Numerical::AbsoluteTolerance = e_absolute;
const Numerical::Double Numerical::RelativeTolerance = e_relative;

#ifndef USE_MULTIPLE_PRECISION
const Numerical::Double Numerical::feasibilityTolerance = e_feasibility;
const Numerical::Double Numerical::optimalityTolerance = e_optimality;
#else
const Numerical::Double Numerical::optimalityTolerance = 0.0;
const Numerical::Double Numerical::feasibilityTolerance = 0.0;
#endif

//double Numerical::SuperAccumlator::m_maxValues[ Numerical::SuperAccumlator::sm_size ];
//Numerical::SuperAccumlator ** Numerical::sm_superAccumlators = 0;
//int Numerical::sm_superAccumlatorCount = 0;

#ifdef USE_MULTIPLE_PRECISION
unsigned long long int Numerical::Double::sm_addCount = 0;
unsigned long long int Numerical::Double::sm_subCount = 0;
unsigned long long int Numerical::Double::sm_mulCount = 0;
unsigned long long int Numerical::Double::sm_divCount = 0;
#endif
