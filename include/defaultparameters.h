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
    class Tolerances {
    public:
        static const double E_PIVOT;
        static const double E_FEASIBILITY;
        static const double E_OPTIMALITY;
    };

    //Starting procedures
    class Starting {
    public:
        class Presolve {
        public:
            static const bool ENABLE;
        };

        class Scaling {
        public:
            static const bool ENABLE;
            static const char * TYPE;
        };

        class Basis {
        public:
            static const char * STARTING_NONBASIC_STATES;
            static const char * STARTING_BASIS_STRATEGY;
        };
    };

    //Basis factorization
    class Factorization {
    public:
        static const char * TYPE;
        static const int REINVERSION_FREQUENCY;
        static const double PIVOT_THRESHOLD;

        class PFI {
        public:
            static const char * NONTRIANGULAR_METHOD;
            static const char * NONTRIANGULAR_PIVOT_RULE;
        };
    };

    //Pricing
    class Pricing {
    public:
        static const char * TYPE;

        class Simpri {
        public:
            static const int PHASEI_CLUSTERS;
            static const int PHASEI_VISIT_CLUSTERS;
            static const int PHASEI_IMPROVING_CANDIDATES;
            static const int PHASEII_CLUSTERS;
            static const int PHASEII_VISIT_CLUSTERS;
            static const int PHASEII_IMPROVING_CANDIDATES;
        };
    };

    //Ratiotest
    class Ratiotest {
    public:
        static const char * NONLINEAR_PRIMAL_PHASEI_FUNCTION;
        static const char * NONLINEAR_DUAL_PHASEI_FUNCTION;
        static const char * NONLINEAR_DUAL_PHASEII_FUNCTION;
        static const bool ENABLE_FAKE_FEASIBILITY;

        class Expand {
        public:
            static const char* TYPE;
            static const double MULTIPLIER;
            static const int DIVIDER;
        };
    };

    //Perturbation
    class Perturbation {
    public:
        static const char* PERTURB_COST_VECTOR;
        static const double E_COST_VECTOR;
        static const bool PERTURB_RHS;
        static const double E_RHS;
        static const bool SHIFT_BOUNDS;
        static const double E_BOUNDS;
    };

    //Global
    class Global {
    public:
        //Switch algorithm
        static const char* STARTING_ALGORITHM;
        static const char* SWITCH_ALGORITHM;

        static const int DEBUG_LEVEL;
        static const int ITERATION_LIMIT;
        static const double TIME_LIMIT;

        class SaveBasis {
        public:
            static const bool BASIS;
            static const char* FILENAME;
            static const char* FORMAT;
            static const bool LAST_BASIS;
            static const int ITERATION;
            static const int PERIODICALLY;
        };

        class LoadBasis {
        public:
            static const bool BASIS;
            static const char* FILENAME;
            static const char* FORMAT;
        };

        class Export {
        public:
            static const bool ENABLE;
            static const char * FILENAME;
            static const char * TYPE;
        };
    };

    //Parallelization
    static const bool ENABLE_PARALLELIZATION;
};

#endif // DEFAULTPARAMETERS_H
