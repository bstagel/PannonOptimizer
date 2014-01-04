/**
 * @file defaultparameters.h
 */

#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H


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
    static const bool SCALING;
    static const int STARTING_BASIS;
    //Basis factorization
    static const int FACTORIZATION_TYPE;
    static const int NONTRIANGULAR_METHOD;
    static const int NONTRIANGULAR_PIVOT_RULE;
    static const int REINVERSION_FREQUENCY;
    static const double PIVOT_THRESHOLD;
    //Pricing
    static const int PRICING_TYPE;
    //Ratiotest
    static const int NONLINEAR_PRIMAL_PHASEI_FUNCTION;
    static const int NONLINEAR_DUAL_PHASEI_FUNCTION;
    static const int NONLINEAR_DUAL_PHASEII_FUNCTION;
    //Global
    static const int DEBUG_LEVEL;
    static const int ITERATION_LIMIT;
    static const double TIME_LIMIT;

};

#endif // DEFAULTPARAMETERS_H
