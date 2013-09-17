#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H


class DefaultParameters {
public:
    //Linalg Parameters
    const static double E_ABSOLUTE = 1.0E-14;
    const static double E_RELATIVE = 1.0E-10;
    const static double SPARSITY_RATIO = 0.05;
    const static double ELBOWROOM = 5;


    //Simplex Parameters
    //Tolerances
    const static double E_PIVOT = 1.0E-06;
    const static double E_HARRIS = 1.0E-06;
    const static double E_FEASIBILITY = 1.0E-08;
    const static double E_OPTIMALITY = 1.0E-06;
    //Starting procedures
    const static double PRESOLVE= 0.0;
    const static double SCALING = 0.0;
    const static double STARTING_BASIS = 0.0;
    //Basis factorization
    const static double FACTORIZATION_TYPE = 0.0;
    const static double TRIANGULAR_METHOD = 1.0;
    const static double NONTRIANGULAR_METHOD = 2.0;
    const static double TRIANGULAR_PIVOT_RULE = 0.0;
    const static double NONTRIANGULAR_PIVOT_RULE = 1.0;
    const static double PIVOT_THRESHOLD = 0.01;
    //Pricing
    const static double PRICING_TYPE = 0.0;
    //Global
    const static double DEBUG_LEVEL = 1.0;
    const static double ITERATION_LIMIT = 200000.0;
    const static double TIME_LIMIT = 3600.0;
    const static double REINVERSION_FREQUENCY = 30.0;

};

#endif // DEFAULTPARAMETERS_H
