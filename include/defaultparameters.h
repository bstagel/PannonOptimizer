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
    static const double ELBOWROOM;


    //Simplex Parameters
    //Tolerances
    static const double E_PIVOT;
    static const double E_HARRIS;
    static const double E_FEASIBILITY;
    static const double E_OPTIMALITY;
    //Starting procedures
    static const double PRESOLVE;
    static const double SCALING;
    static const double STARTING_BASIS;
    //Basis factorization
    static const double FACTORIZATION_TYPE;
    static const double NONTRIANGULAR_METHOD;
    static const double NONTRIANGULAR_PIVOT_RULE;
    static const double PIVOT_THRESHOLD;
    //Pricing
    static const double PRICING_TYPE;
    //Ratiotest
    static const double NONLINEAR_DUAL_PHASEI_FUNCTION;
    static const double NONLINEAR_DUAL_PHASEII_FUNCTION;
    //Global
    static const double DEBUG_LEVEL;
    static const double ITERATION_LIMIT;
    static const double TIME_LIMIT;
    static const double REINVERSION_FREQUENCY;

};

#endif // DEFAULTPARAMETERS_H
