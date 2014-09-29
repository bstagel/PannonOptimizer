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
const double DefaultParameters::Tolerances::E_PIVOT = 1.0E-06;
const double DefaultParameters::Tolerances::E_FEASIBILITY = 1.0E-08;
const double DefaultParameters::Tolerances::E_OPTIMALITY = 1.0E-08;
//Starting procedures
const bool DefaultParameters::Starting::Presolve::ENABLE = false;
const bool DefaultParameters::Starting::Scaling::ENABLE = false;
const char * DefaultParameters::Starting::Scaling::TYPE = "BENICHOU";
const char * DefaultParameters::Starting::Basis::STARTING_NONBASIC_STATES = "NONBASIC_TO_LOWER";
const char * DefaultParameters::Starting::Basis::STARTING_BASIS_STRATEGY = "LOGICAL";
//Basis factorization
const char * DefaultParameters::Factorization::TYPE = "PFI";
const int DefaultParameters::Factorization::REINVERSION_FREQUENCY = 30;
const double DefaultParameters::Factorization::PIVOT_THRESHOLD = 0.1;
const char * DefaultParameters::Factorization::PFI::NONTRIANGULAR_METHOD = "BLOCK_ORDER";
const char * DefaultParameters::Factorization::PFI::NONTRIANGULAR_PIVOT_RULE = "THRESHOLD_PIVOT";

//Pricing
const char * DefaultParameters::Pricing::TYPE = "DANTZIG";
const int DefaultParameters::Pricing::Simpri::PHASEI_CLUSTERS = 1;
const int DefaultParameters::Pricing::Simpri::PHASEI_VISIT_CLUSTERS = 1;
const int DefaultParameters::Pricing::Simpri::PHASEI_IMPROVING_CANDIDATES = 0;
const int DefaultParameters::Pricing::Simpri::PHASEII_CLUSTERS = 1;
const int DefaultParameters::Pricing::Simpri::PHASEII_VISIT_CLUSTERS = 1;
const int DefaultParameters::Pricing::Simpri::PHASEII_IMPROVING_CANDIDATES = 0;
//Ratiotest
const char * DefaultParameters::Ratiotest::NONLINEAR_PRIMAL_PHASEI_FUNCTION = "PIECEWISE";
const char * DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEI_FUNCTION = "PIECEWISE";
const char * DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEII_FUNCTION = "PIECEWISE";
const bool DefaultParameters::Ratiotest::ENABLE_FAKE_FEASIBILITY = false;
const double DefaultParameters::Ratiotest::Expand::MULTIPLIER = 0.01;
const int DefaultParameters::Ratiotest::Expand::DIVIDER = 10000;
const char * DefaultParameters::Ratiotest::Expand::TYPE = "EXPANDING";
//Perturbation
const char * DefaultParameters::Perturbation::PERTURB_COST_VECTOR = "INACTIVE";
const char * DefaultParameters::Perturbation::PERTURB_TARGET = "ALL";
const bool DefaultParameters::Perturbation::PERTURB_LOGICAL = false;
const double DefaultParameters::Perturbation::E_COST_VECTOR = 1.e-6;
const bool DefaultParameters::Perturbation::PERTURB_RHS = false;
const double DefaultParameters::Perturbation::E_RHS = 1.e-6;
const bool DefaultParameters::Perturbation::SHIFT_BOUNDS = false;
const double DefaultParameters::Perturbation::E_BOUNDS = 1.e-6;
//Global
const char * DefaultParameters::Global::STARTING_ALGORITHM = "DUAL";
const char * DefaultParameters::Global::SWITCH_ALGORITHM = "INACTIVE";
const int DefaultParameters::Global::DEBUG_LEVEL = 1;
const int DefaultParameters::Global::ITERATION_LIMIT = 200000;
const double DefaultParameters::Global::TIME_LIMIT = 3600.0;

const bool DefaultParameters::Global::SaveBasis::BASIS = false;
const char* DefaultParameters::Global::SaveBasis::FILENAME = "basis";
const char* DefaultParameters::Global::SaveBasis::FORMAT = "PBF";
const bool DefaultParameters::Global::SaveBasis::LAST_BASIS = false;
const int DefaultParameters::Global::SaveBasis::ITERATION = 0;
const int DefaultParameters::Global::SaveBasis::PERIODICALLY = 0;

const bool DefaultParameters::Global::LoadBasis::BASIS = false;
const char* DefaultParameters::Global::LoadBasis::FILENAME = "basis";
const char* DefaultParameters::Global::LoadBasis::FORMAT = "PBF";

const bool DefaultParameters::Global::Export::ENABLE = false;
const char * DefaultParameters::Global::Export::FILENAME = "exported_result.txt";
const char * DefaultParameters::Global::Export::TYPE = "PARAMETER_STUDY";

//Parallelization
const bool DefaultParameters::ENABLE_PARALLELIZATION = false;
