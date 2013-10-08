/**
 * @file defaultparameters.cpp
 */

#include <defaultparameters.h>

//Linalg Parameters
const double DefaultParameters::E_ABSOLUTE = 1.0E-14;
const double DefaultParameters::E_RELATIVE = 1.0E-10;
const double DefaultParameters::SPARSITY_RATIO = 0.05;
const double DefaultParameters::ELBOWROOM = 5;

//Simplex Parameters
//Tolerances
const double DefaultParameters::E_PIVOT = 1.0E-06;
const double DefaultParameters::E_HARRIS = 1.0E-06;
const double DefaultParameters::E_FEASIBILITY = 1.0E-08;
const double DefaultParameters::E_OPTIMALITY = 1.0E-06;
//Starting procedures
const double DefaultParameters::PRESOLVE= 0.0;
const double DefaultParameters::SCALING = 0.0;
const double DefaultParameters::STARTING_BASIS = 0.0;
//Basis factorization
const double DefaultParameters::FACTORIZATION_TYPE = 0.0;
const double DefaultParameters::NONTRIANGULAR_METHOD = 2.0;
const double DefaultParameters::NONTRIANGULAR_PIVOT_RULE = 1.0;
const double DefaultParameters::PIVOT_THRESHOLD = 0.01;
//Pricing
const double DefaultParameters::PRICING_TYPE = 0.0;
//Global
const double DefaultParameters::DEBUG_LEVEL = 1.0;
const double DefaultParameters::ITERATION_LIMIT = 200000.0;
const double DefaultParameters::TIME_LIMIT = 3600.0;
const double DefaultParameters::REINVERSION_FREQUENCY = 30.0;

