#include "dotproductbenchmark.h"
#include <globals.h>
#include <utils/arch/x86.h>
#include <utils/primitives.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>

#define REPEAT 500

extern "C" double _denseToDenseDotProduct_stable_AVX_64_linux(const double * a,
                                                              const double * b,
                                                              unsigned long long int len);

/*extern "C" double _denseToDenseDotProductSSE2_64_linux(const double * a,
                                                       const double * b,
                                                       unsigned long long int len);*/

DotProductBenchmark::DotProductBenchmark(const char *name):
    UnitTest(name)
{
    ADD_TEST(DotProductBenchmark::naiveDotProduct);
    ADD_TEST(DotProductBenchmark::stableBranchDotProduct);
    ADD_TEST(DotProductBenchmark::stableNoBranchDotProduct);
    ADD_TEST(DotProductBenchmark::naiveDotProductSSE2);
    ADD_TEST(DotProductBenchmark::stableDotProductSSE2);
    ADD_TEST(DotProductBenchmark::naiveDotProductAVX);
    ADD_TEST(DotProductBenchmark::stableDotProductAVX);
    ADD_TEST(DotProductBenchmark::release);
    init();
}

DotProductBenchmark::~DotProductBenchmark()
{
    release();
}

void DotProductBenchmark::naiveDotProduct()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        for (r = 0; r < REPEAT; r++) {
            denseToDenseDotProductUnstable(m_a, m_b, len);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_naive = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::stableBranchDotProduct()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        double neg;
        for (r = 0; r < REPEAT; r++) {
            denseToDenseDotProductStableBranch(m_a, m_b, len, &neg);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_stableBranch = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::stableNoBranchDotProduct()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        double neg;
        for (r = 0; r < REPEAT; r++) {
            denseToDenseDotProductStable(m_a, m_b, len, &neg);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_stableNoBranch = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::naiveDotProductSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        double neg;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_DOTPRODUCT_UNSTABLE_SSE2(m_a, m_b, len);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_naiveSSE2 = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::stableDotProductSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        double neg;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_DOTPRODUCT_STABLE_SSE2(m_a, m_b, len, &neg);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_stableSSE2 = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::naiveDotProductAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        double neg;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_DOTPRODUCT_UNSTABLE_AVX(m_a, m_b, len);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_naiveAVX = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::stableDotProductAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        std::cout << "Length: " << len << std::endl;
        auto start = std::chrono::system_clock::now();
        int r;
        //double neg;
        for (r = 0; r < REPEAT; r++) {
            _denseToDenseDotProduct_stable_AVX_64_linux(m_a, m_b, len);
            //DENSE_TO_DENSE_DOTPRODUCT_STABLE_AVX(m_a, m_b, len, &neg);
        }
        auto end = std::chrono::system_clock::now();

        auto elapsed = end - start;
        auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << elapsed.count() << "   " << elapsed2.count() << '\n';

        m_results[index].m_stableAVX = getMFLOPS(len, elapsed2.count());
        index++;
    }
}

void DotProductBenchmark::init()
{
    unsigned int len = 1000;
    double mul = 1.1;
    unsigned int i;
    for (i = 0; i < 100; i++) {
        m_lengths.push_back(len * pow(mul, i));
    }
    m_results.resize(m_lengths.size());
    m_a = alloc<double, 32>( m_lengths[ m_lengths.size() - 1 ] );
    m_b = alloc<double, 32>( m_lengths[ m_lengths.size() - 1 ] );
    for (i = 0; i <  m_lengths[ m_lengths.size() - 1 ] ; i++) {
        double a, b;
        a = rand() % 1000000 / 1000.0;
        b = rand() % 1000000 / 1000.0;

        switch (rand() % 4) {
        case 0:
            break;
        case 1:
            b *= -1;
            break;
        case 2:
            a *= -1;
            break;
        case 3:
            a *= -1;
            b *= -1;
            break;
        }
        m_a[i] = a;
        m_b[i] = b;
    }
    int counters[2] = {0, 0};
    for (i = 0; i <  m_lengths[ m_lengths.size() - 1 ] ; i++) {
        double c = m_a[i] * m_b[i];
        if (c < 0.0) {
            counters[0]++;
        } else {
            counters[1]++;
        }
    }
    std::cout << counters[0] << std::endl;
    std::cout << counters[1] << std::endl;

}

void DotProductBenchmark::release()
{
    ::release(m_a);
    ::release(m_b);

    std::ofstream ofs("dotproduct_benchmark.csv");
    ofs << "size;naive;stable branch;stable no branch;naive SSE2;stable SSE2;naive AVX;stable AVX;" << std::endl;
    unsigned int index;
    for (index = 0; index < m_lengths.size(); index++) {
        ofs << m_lengths[index] << ";";
        ofs << m_results[index].m_naive << ";";
        ofs << m_results[index].m_stableBranch << ";";
        ofs << m_results[index].m_stableNoBranch << ";";
        ofs << m_results[index].m_naiveSSE2 << ";";
        ofs << m_results[index].m_stableSSE2 << ";";
        ofs << m_results[index].m_naiveAVX << ";";
        ofs << m_results[index].m_stableAVX << ";" << std::endl;
    }
    ofs.close();

}

double DotProductBenchmark::getMFLOPS(unsigned int length, unsigned int time)
{
    return ((1000000.0 / (((double)time / REPEAT) / length))) / 1000000.0;
}

