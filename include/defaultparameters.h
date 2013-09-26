#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H


class DefaultParameters {
public:
    //Linalg Parameters
    const static double E_ABSOLUTE;
    const static double E_RELATIVE;
    const static double SPARSITY_RATIO;
    const static double ELBOWROOM;


    //Simplex Parameters
    //Tolerances
    const static double E_PIVOT;
    const static double E_HARRIS;
    const static double E_FEASIBILITY;
    const static double E_OPTIMALITY;
    //Starting procedures
    const static double PRESOLVE;
    const static double SCALING;
    const static double STARTING_BASIS;
    //Basis factorization
    const static double FACTORIZATION_TYPE;
    const static double NONTRIANGULAR_METHOD;
    const static double NONTRIANGULAR_PIVOT_RULE;
    const static double PIVOT_THRESHOLD;
    //Pricing
    const static double PRICING_TYPE;
    //Global
    const static double DEBUG_LEVEL;
    const static double ITERATION_LIMIT;
    const static double TIME_LIMIT;
    const static double REINVERSION_FREQUENCY;

};

#endif // DEFAULTPARAMETERS_H
