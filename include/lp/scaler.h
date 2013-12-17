#ifndef SCALER_H
#define SCALER_H

#include <vector>
#include <linalg/matrix.h>
#include <utils/numerical.h>

class Scaler {
public:
    void scale(Matrix * matrix, const char *name = 0);

    const std::vector<Numerical::Double> & getRowMultipliers() const;

    const std::vector<Numerical::Double> & getColumnMultipliers() const;

private:
    std::vector<Numerical::Double> m_rowMultipliers;
    std::vector<Numerical::Double> m_columnMultipliers;

    void benichou(Matrix *matrix, const char * name = 0);

    Numerical::Double getVarianceBenichou(Matrix * matrix,
                                          std::vector<Numerical::Double> * rowMultpiliersPtr);

    Numerical::Double roundPowerOf2(Numerical::Double value) const;
};

#endif // SCALER_H
