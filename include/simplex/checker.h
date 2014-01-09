#ifndef CHECKER_H
#define CHECKER_H

#include <simplex/simplex.h>

class Checker {
public:
    static bool checkPfiWithFtran(const Simplex& simplex);
    static bool checkPfiWithBtran(const Simplex& simplex);
    static bool checkPfiWithReducedCost(const Simplex& simplex);
    static bool checkAlphaValue(const Simplex& simplex,
                               int rowIndex, int columnIndex, double& columnAlpha, double& rowAlpha);
    static bool checkPrimalTheta(const Simplex& simplex,
                                   int rowIndex, int columnIndex,
                                   Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow);
};

#endif // CHECKER_H
