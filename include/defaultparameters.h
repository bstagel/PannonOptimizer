//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

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

    class NumericalInstability {
    public:
        static const bool ENABLE_NUMERICAL_INSTABILITY_DETECTION;
    };

    //Parallel
    class Parallel {
    public:
        static const bool ENABLE_PARALLELIZATION;
        static const bool ENABLE_THREAD_SYNCHRONIZATION;
        static const int NUMBER_OF_THREADS;
    };

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
        static const int ADAPTIVE_REINVERSION;
        static const int ADAPTIVE_MULTIPLIER;
        static const double E_ACCURACY;

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
        static const bool COMBINED_OBJECTIVE;

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
        static const bool E_PIVOT_GENERATION;
        static const char * NONLINEAR_PRIMAL_PHASEI_FUNCTION;
        static const char * NONLINEAR_DUAL_PHASEI_FUNCTION;
        static const char * NONLINEAR_DUAL_PHASEII_FUNCTION;
        static const bool ENABLE_FAKE_FEASIBILITY;
        static const bool ENABLE_WOLFE_ADHOC;

        class Expand {
        public:
            static const char* TYPE;
            static const double MULTIPLIER;
            static const int DIVIDER;
            static const bool AVOIDTHETAMIN;
            static const bool ADAPTIVE;
        };
    };

    //Perturbation
    class Perturbation {
    public:
        static const char* PERTURB_COST_VECTOR;
        static const char* PERTURB_TARGET;
        static const bool PERTURB_LOGICAL;
        static const double E_COST_VECTOR;
        static const double XI_MULTIPLIER;
        static const char* WEIGHTING;
        static const double PSI;
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
        static const int REPEAT_SOLUTION;

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

        static const bool BATCH_OUTPUT;
        static const int BATCH_SIZE;
    };

    //Parallelization
    static const bool ENABLE_PARALLELIZATION;
};

#endif // DEFAULTPARAMETERS_H
