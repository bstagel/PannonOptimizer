/**
 * @file defaultparameters.cpp
 */

#include <defaultparameters.h>

//Linalg Parameters
const double DefaultParameters::E_ABSOLUTE = 1.0E-14;
const double DefaultParameters::E_RELATIVE = 1.0E-10;
const double DefaultParameters::SPARSITY_RATIO = 0.45;
const int DefaultParameters::ELBOWROOM = 5;

//Simplex Parameters
//Tolerances
const double DefaultParameters::E_PIVOT = 1.0E-06;
const double DefaultParameters::E_FEASIBILITY = 1.0E-08;
const double DefaultParameters::E_OPTIMALITY = 1.0E-08;
//Starting procedures
const bool DefaultParameters::PRESOLVE = false;
const int DefaultParameters::SCALING = 0;
const int DefaultParameters::STARTING_NONBASIC_STATES = 0;
const int DefaultParameters::STARTING_BASIS_STRATEGY = 0;
//Basis factorization
const int DefaultParameters::FACTORIZATION_TYPE = 0;
const int DefaultParameters::NONTRIANGULAR_METHOD = 2;
const int DefaultParameters::NONTRIANGULAR_PIVOT_RULE = 1;
const int DefaultParameters::REINVERSION_FREQUENCY = 30;
const double DefaultParameters::PIVOT_THRESHOLD = 0.1;
//Pricing
const int DefaultParameters::PRICING_TYPE = 0;
const int DefaultParameters::SIMPRI_PHASEI_CLUSTERS = 1;
const int DefaultParameters::SIMPRI_PHASEI_VISIT_CLUSTERS = 1;
const int DefaultParameters::SIMPRI_PHASEI_IMPROVING_CANDIDATES = 0;
const int DefaultParameters::SIMPRI_PHASEII_CLUSTERS = 1;
const int DefaultParameters::SIMPRI_PHASEII_VISIT_CLUSTERS = 1;
const int DefaultParameters::SIMPRI_PHASEII_IMPROVING_CANDIDATES = 0;
//Ratiotest
const int DefaultParameters::NONLINEAR_PRIMAL_PHASEI_FUNCTION = 1;
const int DefaultParameters::NONLINEAR_DUAL_PHASEI_FUNCTION = 1;
const int DefaultParameters::NONLINEAR_DUAL_PHASEII_FUNCTION = 1;
const int DefaultParameters::ENABLE_FAKE_FEASIBILITY = 0;
const double DefaultParameters::EXPAND_MULTIPLIER = 0.5;
const int DefaultParameters::EXPAND_DIVIDER = 10000;
const int DefaultParameters::EXPAND_DUAL_PHASEI = 0;
const int DefaultParameters::EXPAND_DUAL_PHASEII = 2;
//Perturbation
const int DefaultParameters::PERTURB_COST_VECTOR = 0;
const double DefaultParameters::EPSILON_COST_VECTOR = 1.e-6;
const int DefaultParameters::PERTURB_RHS = 0;
const double DefaultParameters::EPSILON_RHS = 1.e-6;
const int DefaultParameters::SHIFT_BOUNDS = 0;
const double DefaultParameters::EPSILON_BOUNDS = 1.e-6;
//Global
const int DefaultParameters::STARTING_ALGORITHM = 1;
const int DefaultParameters::SWITCH_ALGORITHM = 0;
const int DefaultParameters::DEBUG_LEVEL = 1;
const int DefaultParameters::ITERATION_LIMIT = 200000;
const double DefaultParameters::TIME_LIMIT = 3600.0;

const bool DefaultParameters::SAVE_BASIS = false;
const char* DefaultParameters::SAVE_FILENAME = "basis";
const char* DefaultParameters::SAVE_FORMAT = "PBF";
const bool DefaultParameters::SAVE_LAST_BASIS = false;
const int DefaultParameters::SAVE_ITERATION = 0;
const int DefaultParameters::SAVE_PERIODICALLY = 0;

const bool DefaultParameters::LOAD_BASIS = false;
const char* DefaultParameters::LOAD_FILENAME = "basis";
const char* DefaultParameters::LOAD_FORMAT = "PBF";

const bool DefaultParameters::ENABLE_EXPORT = false;
const char* DefaultParameters::EXPORT_FILENAME = "exported_result.txt";
const int DefaultParameters::EXPORT_TYPE = 0;

