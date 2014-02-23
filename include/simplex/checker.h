#ifndef CHECKER_H
#define CHECKER_H

#include <simplex/simplex.h>

class Checker {
public:
    static bool checkPfiWithFtran(const Simplex& simplex);
    static bool checkPfiWithBtran(const Simplex& simplex);
    static bool checkPfiWithReducedCost(const Simplex& simplex);
    static bool checkAlphaValue(const Simplex& simplex,
                               int rowIndex, int columnIndex, Numerical::Double & columnAlpha, Numerical::Double & rowAlpha);
    static bool checkPrimalTheta(const Simplex& simplex,
                                   int rowIndex, int columnIndex,
                                   Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow);
    static bool checkFeasibilityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);
    static bool checkOptimalityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);
    static bool checkAllConstraints(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);
    static bool checkNonbasicVariableStates(const Simplex& simplex, bool print = true);
    static bool checkBasicVariableStates(const Simplex& simplex, bool print = true);
    static bool checkBasicVariableFeasibilities(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

};

#endif // CHECKER_H
