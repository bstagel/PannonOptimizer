#ifndef DOTPRODUCTBENCHMARK_H
#define DOTPRODUCTBENCHMARK_H

#include <framework/tester.h>
#include <framework/unittest.h>

class DotProductBenchmark : public UnitTest
{
public:
    DotProductBenchmark(const char * name);
    ~DotProductBenchmark();

private:
    struct Times {
        double m_naive;
        double m_stableBranch;
        double m_stableNoBranch;
        double m_naiveSSE2;
        double m_stableSSE2;
        double m_naiveAVX;
        double m_stableAVX;
    };

    std::vector<Times> m_results;
    std::vector<unsigned int> m_lengths;

    double * m_a;
    double * m_b;

    void naiveDotProduct();

    void stableBranchDotProduct();

    void stableNoBranchDotProduct();

    void naiveDotProductSSE2();

    void stableDotProductSSE2();

    void naiveDotProductAVX();

    void stableDotProductAVX();

    ////////////////

    void init();
    void release();
    double getMFLOPS(unsigned int length, unsigned int time);
};

#endif // DOTPRODUCTBENCHMARK_H
