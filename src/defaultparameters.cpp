/**
 * @file defaultparameters.cpp
 */

#include <defaultparameters.h>

//Linalg Parameters
const double DefaultParameters::E_ABSOLUTE = 1.0E-14;
const double DefaultParameters::E_RELATIVE = 1.0E-10;
const double DefaultParameters::SPARSITY_RATIO = 0.05;
const int DefaultParameters::ELBOWROOM = 5;

//Simplex Parameters
//Tolerances
const double DefaultParameters::E_PIVOT = 1.0E-06;
const double DefaultParameters::E_FEASIBILITY = 1.0E-08;
const double DefaultParameters::E_OPTIMALITY = 1.0E-08;
//Starting procedures
const bool DefaultParameters::PRESOLVE = false;
const bool DefaultParameters::SCALING = false;
const int DefaultParameters::STARTING_BASIS = 0;
//Basis factorization
const int DefaultParameters::FACTORIZATION_TYPE = 0;
const int DefaultParameters::NONTRIANGULAR_METHOD = 2;
const int DefaultParameters::NONTRIANGULAR_PIVOT_RULE = 1;
const int DefaultParameters::REINVERSION_FREQUENCY = 30;
const double DefaultParameters::PIVOT_THRESHOLD = 0.1;
//Pricing
const int DefaultParameters::PRICING_TYPE = 0;
//Ratiotest
const int DefaultParameters::NONLINEAR_PRIMAL_PHASEI_FUNCTION = 1;
const int DefaultParameters::NONLINEAR_DUAL_PHASEI_FUNCTION = 2;
const int DefaultParameters::NONLINEAR_DUAL_PHASEII_FUNCTION = 2;
const int DefaultParameters::THRESHOLD_REPORT_LEVEL = 0;
const double DefaultParameters::EXPAND_MULTIPLIER_DPH1 = 0.1;
const int DefaultParameters::EXPAND_DIVIDER_DPH1 = 10000;
const int DefaultParameters::EXPAND_DUAL_PHASEII = 0;
//Global
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

