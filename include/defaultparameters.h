#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H


class DefaultParameters {
public:
    //Linalg Parameters
    //Tolerances
    const static double M_E_ABSOLUTE = 1.0E-14;
    const static double M_E_RELATIVE = 1.0E-10;
    //Global
    const static double M_SPARSITY_RATIO = 0.05;
    const static double M_ELBOWROOM = 5;


    //Simplex Parameters
    //Tolerances
    const static double M_E_PIVOT = 1.0E-06;
    const static double M_E_HARRIS = 1.0E-06;
    const static double M_E_FEASIBILITY = 1.0E-08;
    const static double M_E_OPTIMALITY = 1.0E-06;
    //Preprocessing
    const static double M_PRESOLVE= 0.0;
    const static double M_SCALING = 1.0;
    //Starting procedures
    const static double M_STARTING_BASIS = 4.0;
    //Basis factorization
    const static double M_FACTORIZATION_TYPE = 0.0;
    const static double M_TRIANGULAR_METHOD = 1.0;
    const static double M_NONTRIANGULAR_METHOD = 2.0;
    const static double M_TRIANGULAR_PIVOT_RULE = 0.0;
    const static double M_NONTRIANGULAR_PIVOT_RULE = 1.0;
    const static double M_PIVOT_THRESHOLD = 0.01;
    //Pricing
    const static double M_PRICING_TYPE = 0.0;
    const static double M_PRICING_CANDIDATE_LIST = 0.0;
    const static double M_PRCING_CLUSTER_COUNT = 1.0;
    const static double M_INVESTIGATED_CLUSTERS = 1.0;
    const static double M_IMPROVING_VARIABLES_IN_CLUSTER = 0.0;
    //Ratio test
    const static double M_RATIO_TEST_PRIMAL_PH1 = 0.0;
    const static double M_RATIO_TEST_ANTI_DEGENERACY = 0.0;
    const static double M_HARRIS_RATIO_TEST = 1.0;
    //Global
    const static double M_DEBUG_LEVEL = 1.0;
    const static double M_ITERATION_LIMIT = 200000.0;
    const static double M_TIME_LIMIT = 3600.0;
    const static double M_CLOCK_RATIO = 0.0;
    const static double M_REINVERSION_FREQUENCY = 30.0;

};

#endif // DEFAULTPARAMETERS_H
