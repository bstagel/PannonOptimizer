/**
 * @file defaultparameters.h
 */

#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H

#include <string>

class DefaultParameters {
public:
    //Linalg Parameters
    static const double E_ABSOLUTE;
    static const double E_RELATIVE;
    static const double SPARSITY_RATIO;
    static const int ELBOWROOM;

    //Simplex Parameters
    //Tolerances
    static const double E_PIVOT;
    static const double E_FEASIBILITY;
    static const double E_OPTIMALITY;
    //Starting procedures
    static const bool PRESOLVE;
    static const int SCALING;
    static const int STARTING_NONBASIC_STATES;
    static const int STARTING_BASIS_STRATEGY;
    //Basis factorization
    static const int FACTORIZATION_TYPE;
    static const int NONTRIANGULAR_METHOD;
    static const int NONTRIANGULAR_PIVOT_RULE;
    static const int REINVERSION_FREQUENCY;
    static const double PIVOT_THRESHOLD;
    //Pricing
    static const int PRICING_TYPE;
    static const int SIMPRI_PHASEI_CLUSTERS;
    static const int SIMPRI_PHASEI_VISIT_CLUSTERS;
    static const int SIMPRI_PHASEI_IMPROVING_CANDIDATES;
    static const int SIMPRI_PHASEII_CLUSTERS;
    static const int SIMPRI_PHASEII_VISIT_CLUSTERS;
    static const int SIMPRI_PHASEII_IMPROVING_CANDIDATES;
    //Ratiotest
    static const int NONLINEAR_PRIMAL_PHASEI_FUNCTION;
    static const int NONLINEAR_DUAL_PHASEI_FUNCTION;
    static const int NONLINEAR_DUAL_PHASEII_FUNCTION;
    static const int ENABLE_FAKE_FEASIBILITY;
    static const double EXPAND_MULTIPLIER;
    static const int EXPAND_DIVIDER;
    static const int EXPAND_DUAL_PHASEI;
    static const int EXPAND_DUAL_PHASEII;
    //Perturbation
    static const int PERTURB_COST_VECTOR;
    static const double EPSILON_COST_VECTOR;
    static const int PERTURB_RHS;
    static const double EPSILON_RHS;
    static const int SHIFT_BOUNDS;
    static const double EPSILON_BOUNDS;
    //Switch algorithm
    static const int STARTING_ALGORITHM;
    static const int SWITCH_ALGORITHM;

    //Global
    static const int DEBUG_LEVEL;
    static const int ITERATION_LIMIT;
    static const double TIME_LIMIT;

    static const bool SAVE_BASIS;
    static const char* SAVE_FILENAME;
    static const char* SAVE_FORMAT;
    static const bool SAVE_LAST_BASIS;
    static const int SAVE_ITERATION;
    static const int SAVE_PERIODICALLY;

    static const bool LOAD_BASIS;
    static const char* LOAD_FILENAME;
    static const char* LOAD_FORMAT;

    static const bool ENABLE_EXPORT;
    static const char* EXPORT_FILENAME;
    static const int EXPORT_TYPE;
};

#endif // DEFAULTPARAMETERS_H
