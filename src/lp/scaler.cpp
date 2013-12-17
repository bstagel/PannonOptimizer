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
    LPINFO("Time: " << timer.getLastElapsed());
}

void Scaler::benichou(Matrix * matrix, const char *name) {
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
    std::vector<double> variances;
    variances.push_back(variance);
    LPINFO("Variance: " << variance);

    std::ofstream ofs("benichou.csv", std::ios_base::app);
    if ( ofs.is_open() ) {
        LPINFO("Megnyitva");
    } else {
        LPERROR("Hiba a megnyitaskor");
    }
    ofs << name << ";";

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

        // sor szerint
        //LPINFO("-------------------------------------------");
        //matrix->show();
        for (index = 0; index < rowCount; index++) {
            matrix->scaleRowAndGetResults(index, columnMultipliers,
                                          rowMultipliers[index],
                                          &squareTemp, &min, &max);
            m_rowMultipliers[index] *= rowMultipliers[index];
        }
       // matrix->show();
       // LPINFO("-------------------------------------------");
        for (index = 0; index < columnCount; index++) {
            matrix->scaleColumnAndGetResults(index, rowMultipliers,
                                             1,
                                             &squareTemp, &min, &max);
            columnMultipliers[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));
          //  LPINFO(m_columnMultipliers[index]);
        }
        //matrix->show();
        //LPINFO("-------------------------------------------");

        for (index = 0; index < rowCount; index++) {
            rowMultipliers[index] = 1;
        }


        // oszlop szerint
        for (index = 0; index < columnCount; index++) {
             matrix->scaleColumnAndGetResults(index, rowMultipliers,
                                             columnMultipliers[index],
                                             &squareTemp, &min, &max);
             m_columnMultipliers[index] *= columnMultipliers[index];
        }

        for (index = 0; index < rowCount; index++) {
            absSum += matrix->scaleRowAndGetResults(index, columnMultipliers,
                                                    1,
                                                    &squareTemp, &min, &max);
            squareSum += squareTemp;
            rowMultipliers[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));

        }
        //matrix->show();
        //exit(1);
        for (index = 0; index < columnCount; index++) {
            columnMultipliers[index] = 1;
        }
        // sor szerinti variancia szamolas
        //variance = getVarianceBenichou(matrix);
        variance = (squareSum - (absSum * absSum) / nonZeros) / nonZeros;
        variances.push_back(variance);
        LPINFO("Variance: " << variance);
        stepCount++;
    }
    timer.stop();
    ofs << timer.getLastElapsed() << ";" << variances.size() << ";";
    for (index = 0; index < variances.size(); index++) {
        ofs << variances[index] << ";";
    }
    ofs << std::endl;
    ofs.close();
  //  matrix->show();

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
        absSum += matrix->row(index).absMaxSumsAndMinMax(&squareTemp, &min, &max);
        (*rowMultpiliersPtr)[index] = roundPowerOf2(1.0 / Numerical::sqrt(min * max));

        squareSum += squareTemp;
    }
    return (squareSum - (absSum * absSum) / nonZeros) / nonZeros;
}

Numerical::Double Scaler::roundPowerOf2(Numerical::Double value) const {
    return value;
    union DoubleBits {
        Numerical::Double m_value;
        unsigned long long int m_bits;
    } temp;
    temp.m_value = value;
    temp.m_bits &= 0xFFF0000000000000ULL;
    Numerical::Double result1 = temp.m_value;
    temp.m_bits += 0x0010000000000000ULL;
    Numerical::Double result2 = temp.m_value;
    return value - result1 > result2 - value ? result2 : result1;
}
