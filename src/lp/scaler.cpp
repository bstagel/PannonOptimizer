#include <lp/scaler.h>
#include <utils/timer.h>
#include <debug.h>
#include <fstream>

#include <cstdio>

const std::vector<Numerical::Double> & Scaler::getRowMultipliers() const {
    return m_rowMultipliers;
}

const std::vector<Numerical::Double> & Scaler::getColumnMultipliers() const {
    return m_columnMultipliers;
}

void Scaler::scale(Matrix *matrix, const char * name) {
    LPINFO("SCALER");

    m_rowMultipliers.resize( matrix->rowCount(), 1.0 );
    m_columnMultipliers.resize( matrix->columnCount(), 1.0 );

    Timer timer;
    timer.start();
    benichou(matrix, name);

    timer.stop();

    //    LPINFO("Variance: " << variance);
    LPINFO("Time: " << timer.getCPULastElapsed());
}

void Scaler::benichou(Matrix * matrix, const char *name) {
    __UNUSED(name);
    const unsigned int nonZeros = matrix->nonZeros();
    const unsigned int rowCount = matrix->rowCount();
    const unsigned int columnCount = matrix->columnCount();
    std::vector<Numerical::Double> rowMultipliers;
    std::vector<Numerical::Double> columnMultipliers;
    rowMultipliers.resize(rowCount, 1);
    columnMultipliers.resize(columnCount, 1);

    unsigned int stepCount = 0;
    Numerical::Double variance;
    variance = getVarianceBenichou(matrix, &rowMultipliers);
    std::vector<Numerical::Double> variances;
    variances.push_back(variance);
    LPINFO("Variance: " << variance);

    //std::ofstream ofs("benichou.csv", std::ios_base::app);
    //ofs << name << ";";

    Timer timer;
    timer.start();


    // matrix->show();
    unsigned int index;
    while (variance > 2 && stepCount < 5) {
        Numerical::Double absSum = 0;
        Numerical::Double squareSum = 0;
        Numerical::Double squareTemp;
        Numerical::Double min;
        Numerical::Double max;

        // rowwise
        for (index = 0; index < rowCount; index++) {
            matrix->scaleRowAndGetResults(index, columnMultipliers,
                                          rowMultipliers[index],
                                          &squareTemp, &min, &max);
            m_rowMultipliers[index] *= rowMultipliers[index];
        }
        for (index = 0; index < columnCount; index++) {
            if (matrix->column(index).nonZeros() == 0) {
                columnMultipliers[index] = 1.0;
                continue;
            }
            matrix->scaleColumnAndGetResults(index, rowMultipliers,
                                             1,
                                             &squareTemp, &min, &max);
            columnMultipliers[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));

        }

        for (index = 0; index < rowCount; index++) {
            rowMultipliers[index] = 1;
        }

        // columnwise
        for (index = 0; index < columnCount; index++) {
            matrix->scaleColumnAndGetResults(index, rowMultipliers,
                                             columnMultipliers[index],
                                             &squareTemp, &min, &max);
            m_columnMultipliers[index] *= columnMultipliers[index];
        }

        for (index = 0; index < rowCount; index++) {
            if (matrix->row(index).nonZeros() == 0) {
                rowMultipliers[index] = 1.0;
                continue;
            }
            absSum += matrix->scaleRowAndGetResults(index, columnMultipliers,
                                                    1,
                                                    &squareTemp, &min, &max);
            squareSum += squareTemp;

            rowMultipliers[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));

        }
        for (index = 0; index < columnCount; index++) {
            columnMultipliers[index] = 1;
        }
        variance = (squareSum - (absSum * absSum) / nonZeros) / nonZeros;
        variances.push_back(variance);
        LPINFO("Variance: " << variance);
        stepCount++;
    }

    for (index = 0; index < rowCount; index++) {
        if (matrix->row(index).nonZeros() == 0) {
            rowMultipliers[index] = 1.0;
            continue;
        }
        Numerical::Double multiplier = roundPowerOf2(1.0 / matrix->row(index).absMaxElement());
        rowMultipliers[index] = multiplier;
        matrix->scaleOnlyRowwise(index, multiplier);
        m_rowMultipliers[index] *= multiplier;
    }
    for (index = 0; index < columnCount; index++) {
        if (matrix->column(index).nonZeros() == 0) {
            columnMultipliers[index] = 1.0;
            continue;
        }
        matrix->scaleOnlyColumnwiseLambdas(index, rowMultipliers);
        Numerical::Double multiplier = roundPowerOf2(1.0 / matrix->column(index).absMaxElement());
        columnMultipliers[index] = multiplier;
        matrix->scaleOnlyColumnwise(index, multiplier);
        m_columnMultipliers[index] *= multiplier;
    }

    for (index = 0; index < rowCount; index++) {
        matrix->scaleOnlyRowwiseLambdas(index, columnMultipliers);
    }

    variance = getVarianceBenichou(matrix, &rowMultipliers);
    variances.push_back(variance);
    LPINFO("Variance: " << variance);

    // ez nem igazan segit, majd ha lesz steepest edge meg devex, akkor ujra
    // meg kell nezni
    /*for (index = 0; index < rowCount; index++) {
        rowMultipliers[index] = roundPowerOf2(1.0 + (rand() % 100000000) /    100000000000.0);
        matrix->scaleOnlyRowwise(index, rowMultipliers[index]);
        m_rowMultipliers[index] *= rowMultipliers[index];
        //LPINFO(rowMultipliers[index]);
    }
    for (index = 0; index < columnCount; index++) {
        matrix->scaleOnlyColumnwiseLambdas(index, rowMultipliers);
        columnMultipliers[index] = roundPowerOf2(1.0 + (rand() % 100000000) / 100000000000.0);
        matrix->scaleOnlyColumnwise(index, columnMultipliers[index]);
        m_columnMultipliers[index] *= columnMultipliers[index];

    }
    for (index = 0; index < rowCount; index++) {
        matrix->scaleOnlyRowwiseLambdas(index, columnMultipliers);
    }*/

    variance = getVarianceBenichou(matrix, &rowMultipliers);
    variances.push_back(variance);
    LPINFO("Variance: " << variance);

    timer.stop();
    /*ofs << timer.getCPULastElapsed() << ";" << variances.size() << ";";
    for (index = 0; index < variances.size(); index++) {
        ofs << variances[index] << ";";
    }
    ofs << std::endl;
    ofs.close();*/
    // matrix->show();
}

Numerical::Double Scaler::getVarianceBenichou(Matrix * matrix,
                                              std::vector<Numerical::Double> * rowMultpiliersPtr) {
    unsigned int index;
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    Numerical::Double squareTemp;
    Numerical::Double min;
    Numerical::Double max;

    const unsigned int nonZeros = matrix->nonZeros();
    const unsigned int rowCount = matrix->rowCount();

    for (index = 0; index < rowCount; index++) {
        if (matrix->row(index).nonZeros() == 0) {
            (*rowMultpiliersPtr)[index] = 1.0;
            continue;
        }
        absSum += matrix->row(index).absMaxSumsAndMinMax(&squareTemp, &min, &max);
        (*rowMultpiliersPtr)[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));

        squareSum += squareTemp;
    }
    return (squareSum - (absSum * absSum) / nonZeros) / nonZeros;
}

Numerical::Double Scaler::roundPowerOf2(Numerical::Double value) const {
    union DoubleBits {
        double m_value;
        unsigned long long int m_bits;
    } temp;
#if DOUBLE_TYPE == DOUBLE_GMP
    temp.m_value = value;
#endif
#if DOUBLE_TYPE == DOUBLE_CLASSIC
    temp.m_value = (double)value;
#endif
#if DOUBLE_TYPE == DOUBLE_HISTORY
    temp.m_value = (double)value;
#endif
    temp.m_bits &= 0xFFF0000000000000ULL;
    Numerical::Double result1 = temp.m_value;
    temp.m_bits += 0x0010000000000000ULL;
    Numerical::Double result2 = temp.m_value;
    Numerical::Double result = value - result1 > result2 - value ? result2 : result1;
    return result;
}
