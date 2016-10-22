#include "addvectorbenchmark.h"
#include <globals.h>
#include <utils/arch/x86.h>
#include <utils/primitives.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>
#include <unistd.h>

extern "C" unsigned long long int readTimeStampCounter();

#define N 1000 000 0
int REPEAT = 5000;

const double relTolerance = 0.001;
const double absTolerance = 0.01;
const double lambda = 2.0;

#define CPU_COOLING //if (index % 10 == 0) {std::cout << "cpu cooling\n"; sleep(5);}

AddVectorBenchmark::AddVectorBenchmark(const char *name):
    UnitTest(name)
{
    system("rm *.freq");
    m_a = nullptr;
    m_b = nullptr;
    m_c = nullptr;



    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDense);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDense);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHays);
    //ADD_TEST(AddVectorBenchmark::stableAbsDenseToDense);
    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDenseSSE2);
    //  ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseSSE2);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysSSE2);
    //ADD_TEST(AddVectorBenchmark::stableAbsDenseToDenseSSE2);
    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDenseAVX);
    //  ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseAVX);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysAVX);
    //ADD_TEST(AddVectorBenchmark::stableAbsDenseToDenseAVX);

    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDenseSelf);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseSelf);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseSelfOrchardHays);
   // ADD_TEST(AddVectorBenchmark::stableAbsDenseToDenseSelf);
    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDenseSSE2Self);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseSSE2Self);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysSSE2Self);
    //ADD_TEST(AddVectorBenchmark::stableAbsDenseToDenseSSE2Self);
    //ADD_TEST(AddVectorBenchmark::naiveAddDenseToDenseAVXSelf);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseAVXSelf);
    //ADD_TEST(AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysAVXSelf);
    //ADD_TEST(AddVectorBenchmark::stableAbsDenseToDenseAVXSelf);
    ADD_TEST(AddVectorBenchmark::release);
    init();
}

AddVectorBenchmark::~AddVectorBenchmark()
{
    release();
}

void AddVectorBenchmark::naiveAddDenseToDense()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }
        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        //bool again = false;
        decltype(readTimeStampCounter()) start, end;
        //do {
        //if (again) {
        //    std::cout << "repeat..." << std::endl;
        //}
        start = readTimeStampCounter();
        int r;
        for (r = 0; r < REPEAT; r++) {
            denseToDenseAdd(m_a, m_b, m_c, len, lambda);
        }
        end = readTimeStampCounter();
        //auto end = readTimeStampCounter();
        //again = end < start;
        //} while (again);
        auto elapsed = (end - start);
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        //std::cout << "1  " <<  elapsed << '\n';
        std::cout << "1  " << elapsed << std::endl;
        m_results[index].m_naive = getMFLOPS2(len, elapsed);//getMFLOPS2(len, elapsed);
        index++;
        saveFreq("naive_3vectors.freq");
        CPU_COOLING
    }
    save("naive_3vectors.dat", &Times::m_naive);

}

void AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHays()
{
    std::cout << std::endl;

    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }
        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            denseToDenseAddRelOrchardHays(m_a, m_b, m_c, len, lambda, relTolerance);
            //denseToDenseAddAbsRelOrchardHays(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "2  " << elapsed << '\n';

        m_results[index].m_relAbsOrchard = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_3vectors_or.freq");
        CPU_COOLING
    }

    save("absrel_3vectors_or.dat", &Times::m_relAbsOrchard);
}

void AddVectorBenchmark::stableRelAbsDenseToDense()
{
    std::cout << std::endl;

    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();//std::chrono::system_clock::now();
        int r;
        for (r = 0; r < REPEAT; r++) {
            //denseToDenseAddAbsRel(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            denseToDenseAddRel(m_a, m_b, m_c, len, lambda, relTolerance);
        }
        auto end = readTimeStampCounter();

        auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "3  " << elapsed << '\n';

        m_results[index].m_relAbs = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_3vectors.freq");
        CPU_COOLING
    }
    save("absrel_3vectors.dat", &Times::m_relAbs);
}

void AddVectorBenchmark::stableAbsDenseToDense()
{
    std::cout << std::endl;

    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            denseToDenseAddAbs(m_a, m_b, m_c, len, lambda, absTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }


        elapsed = min * REPEAT; //end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "4  " << elapsed << '\n';

        m_results[index].m_abs = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("abs_3vectors.freq");
        CPU_COOLING
    }
    save("abs_3vectors.dat", &Times::m_abs);
}

void AddVectorBenchmark::naiveAddDenseToDenseSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();
        int r;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_ADD_SSE2_CACHE(m_a, m_b, m_c, len, lambda);
        }
        auto end = readTimeStampCounter();

        auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "5  " << elapsed << '\n';

        m_results[index].m_naiveSSE2 = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("naive_sse2_3vectors.freq");
        CPU_COOLING
    }

    save("naive_sse2_3vectors.dat", &Times::m_naiveSSE2);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();
        int r;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_ADD_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda);
        }
        auto end = readTimeStampCounter();

        auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "6  " << elapsed << '\n';

        m_results[index].m_naiveSSE2_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("naive_sse2_nocache_3vectors.freq");
        CPU_COOLING
    }
    save("naive_sse2_nocache_3vectors.dat", &Times::m_naiveSSE2_nocache);

}

void AddVectorBenchmark::stableRelAbsDenseToDenseSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_CACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_SSE2_CACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }


        elapsed = min * REPEAT;//end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "7  " << elapsed << '\n';

        m_results[index].m_relAbsSSE2 = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_sse2_3vectors.freq");
        CPU_COOLING
    }
    save("absrel_sse2_3vectors.dat", &Times::m_relAbsSSE2);
    //return;
    index = 0;
    int r;
    for (r = 0; r < REPEAT; r++) {
        //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
        DENSE_TO_DENSE_ADD_REL_SSE2_NOCACHE(m_a, m_b, m_c, 1000, lambda, relTolerance);
    }

    for (auto len: m_lengths) {
        int _counter = 0;
        do {
            if (index < 26) {
                REPEAT = 50000;
            } else if (index < 53) {
                REPEAT = 5000;
            } else {
                REPEAT = 500;
            }

            std::cout << "Length: " << len << std::endl;
            auto start = readTimeStampCounter();
            int r;
            for (r = 0; r < REPEAT; r++) {
                //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
                DENSE_TO_DENSE_ADD_REL_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            }
            auto end = readTimeStampCounter();

            auto elapsed = end - start;
            ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            std::cout << "8  " << elapsed << '\n';

            m_results[index].m_relAbsSSE2_nocache = getMFLOPS2(len, elapsed);
            _counter++;
        } while (_counter < 20 && index > 0 && m_results[index].m_relAbsSSE2_nocache < m_results[index - 1].m_relAbsSSE2_nocache);
        index++;
        saveFreq("absrel_sse2_nocache_3vectors.freq");
        CPU_COOLING
    }
    save("absrel_sse2_nocache_3vectors.dat", &Times::m_relAbsSSE2_nocache);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_SSE2_CACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_SSE2_CACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "9  " << elapsed << '\n';

        m_results[index].m_relAbsOrchardSSE2 = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_sse2_3vectors_or.freq");
        CPU_COOLING
    }

    save("absrel_sse2_3vectors_or.dat", &Times::m_relAbsOrchardSSE2);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            DENSE_TO_DENSE_ADD_REL_ORCHARD_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "10 " << elapsed << '\n';

        m_results[index].m_relAbsOrchardSSE2_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_sse2_nocache_3vectors_or.freq");
        CPU_COOLING
    }
    save("absrel_sse2_nocache_3vectors_or.dat", &Times::m_relAbsOrchardSSE2_nocache);
}

void AddVectorBenchmark::stableAbsDenseToDenseSSE2()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_SSE2_CACHE(m_a, m_b, m_c, len, lambda, absTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "11 " <<elapsed << '\n';

        m_results[index].m_absSSE2 = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("abs_sse2_3vectors.freq");
        CPU_COOLING
    }

    save("abs_sse2_3vectors.dat", &Times::m_absSSE2);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_SSE2_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        ////auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "12 " <<elapsed << '\n';

        m_results[index].m_absSSE2_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("abs_sse2_nocache_3vectors.freq");
        CPU_COOLING
    }

    save("abs_sse2_nocache_3vectors.dat", &Times::m_absSSE2_nocache);
}

void AddVectorBenchmark::naiveAddDenseToDenseAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();
        int r;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_ADD_AVX_CACHE(m_a, m_b, m_c, len, lambda);
        }
        auto end = readTimeStampCounter();

        auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "13 " << elapsed << '\n';

        m_results[index].m_naiveAVX = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("naive_avx_3vectors.freq");
        CPU_COOLING
    }

    save("naive_avx_3vectors.dat", &Times::m_naiveAVX);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();
        int r;
        for (r = 0; r < REPEAT; r++) {
            DENSE_TO_DENSE_ADD_AVX_NOCACHE(m_a, m_b, m_c, len, lambda);
        }
        auto end = readTimeStampCounter();

        auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "14 " << elapsed << '\n';

        m_results[index].m_naiveAVX_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("naive_avx_nocache_3vectors.freq");
        CPU_COOLING
    }

    save("naive_avx_nocache_3vectors.dat", &Times::m_naiveAVX_nocache);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 8000;
        } else {
            REPEAT = 6000;
        }
        int _counter = 0;
        do {
            std::cout << "Length: " << len << std::endl;
            decltype(readTimeStampCounter()) min = -1, elapsed;
            int r;
            for (r = 0; r < REPEAT; r++) {
                auto start = readTimeStampCounter();
                DENSE_TO_DENSE_ADD_REL_AVX_CACHE(m_a, m_b, m_c, len, lambda, relTolerance);
                //DENSE_TO_DENSE_ADD_ABS_REL_AVX_CACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
                auto end = readTimeStampCounter();
                elapsed = end - start;
                if (elapsed < min) min = elapsed;
            }

            elapsed = elapsed * REPEAT;//end - start;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            std::cout << "15 " << elapsed << '\n';

            m_results[index].m_relAbsAVX = getMFLOPS2(len, elapsed);
            _counter++;
        } while  (index > 0 && _counter < 1 && m_results[index].m_relAbsAVX < m_results[index-1].m_relAbsAVX);

        index++;
        saveFreq("absrel_avx_3vectors.freq");
        CPU_COOLING
    }

    save("absrel_avx_3vectors.dat", &Times::m_relAbsAVX);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 8000;
        } else {
            REPEAT = 1000;
        }

        std::cout << "Length: " << len << std::endl;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_AVX_NOCACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_AVX_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }


        elapsed = min * REPEAT;//end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "16 " <<  elapsed << '\n';

        m_results[index].m_relAbsAVX_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_avx_nocache_3vectors.freq");
        CPU_COOLING
    }

    save("absrel_avx_nocache_3vectors.dat", &Times::m_relAbsAVX_nocache);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_AVX_CACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_AVX_CACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "17 " <<  elapsed << '\n';

        m_results[index].m_relAbsOrchardAVX = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_avx_3vectors_or.freq");
        CPU_COOLING
    }

    save("absrel_avx_3vectors_or.dat", &Times::m_relAbsOrchardAVX);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        auto start = readTimeStampCounter();
        int r;
        decltype(readTimeStampCounter()) min = -1, elapsed;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_AVX_NOCACHE(m_a, m_b, m_c, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_AVX_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance, relTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "18 " <<  elapsed << '\n';

        m_results[index].m_relAbsOrchardAVX_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("absrel_avx_nocache_3vectors_or.freq");
        CPU_COOLING
    }

    save("absrel_avx_nocache_3vectors_or.dat", &Times::m_relAbsOrchardAVX_nocache);
}

void AddVectorBenchmark::stableAbsDenseToDenseAVX()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_AVX_CACHE(m_a, m_b, m_c, len, lambda, absTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "19 " <<  elapsed << '\n';

        m_results[index].m_absAVX = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("abs_avx_3vectors.freq");
        CPU_COOLING
    }

    save("abs_avx_3vectors.dat", &Times::m_absAVX);

    index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        //auto start = readTimeStampCounter();
        decltype(readTimeStampCounter()) min = -1, elapsed;
        int r;
        for (r = 0; r < REPEAT; r++) {
            auto start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_AVX_NOCACHE(m_a, m_b, m_c, len, lambda, absTolerance);
            auto end = readTimeStampCounter();
            elapsed = end - start;
            if (elapsed < min) min = elapsed;
        }
        elapsed = min * REPEAT; //end - start;
        //auto end = readTimeStampCounter();

        //auto elapsed = end - start;
        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "20 " <<  elapsed << '\n';

        m_results[index].m_absAVX_nocache = getMFLOPS2(len, elapsed);
        index++;
        saveFreq("abs_avx_nocache_3vectors.freq");
        CPU_COOLING
    }

    save("abs_avx_nocache_3vectors.dat", &Times::m_absAVX_nocache);
}

////////////////////////////////////////////////////

void AddVectorBenchmark::naiveAddDenseToDenseSelf()
{
    std::cout << std::endl;
    int index = 0;
    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        unsigned long long int time = 0;

        int r;
        double sum = 0.0;
        int index2;
        unsigned long long int start, end;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            denseToDenseAdd(m_a, m_b, m_a, len, lambda);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;

        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "21 " << time << "   " << sum << '\n';

        m_results[index].m_naive = getMFLOPS2(len, time / 1);
        index++;
        saveFreq("naive_2vectors.freq");
        CPU_COOLING
    }

    save("naive_2vectors.dat", &Times::m_naive);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseSelfOrchardHays()
{
    std::cout << std::endl;

    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            denseToDenseAddRelOrchardHays(m_a, m_b, m_a, len, lambda, relTolerance);
            //denseToDenseAddAbsRelOrchardHays(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;

        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "22 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsOrchard = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_2vectors_or.freq");
        CPU_COOLING

    }
    save("absrel_2vectors_or.dat", &Times::m_relAbsOrchard);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseSelf()
{
    std::cout << std::endl;

    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            denseToDenseAddRel(m_a, m_b, m_a, len, lambda, relTolerance);
            //denseToDenseAddAbsRel(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;

        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "23 " << time << "   " << sum << '\n';

        m_results[index].m_relAbs = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_2vectors.freq");
        CPU_COOLING

    }
    save("absrel_2vectors.dat", &Times::m_relAbs);
}



void AddVectorBenchmark::stableAbsDenseToDenseSelf()
{
    std::cout << std::endl;

    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            denseToDenseAddAbs(m_a, m_b, m_a, len, lambda, absTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
            //
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "24 " << time << "   " << sum << '\n';

        m_results[index].m_abs = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("abs_2vectors.freq");
        CPU_COOLING

    }
    save("abs_2vectors.dat", &Times::m_abs);
}


void AddVectorBenchmark::naiveAddDenseToDenseSSE2Self()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_SSE2_CACHE(m_a, m_b, m_a, len, lambda);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "25 " << time << "   " << sum << '\n';

        m_results[index].m_naiveSSE2 = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("naive_sse2_2vectors.freq");
        CPU_COOLING

    }

    save("naive_sse2_2vectors.dat", &Times::m_naiveSSE2);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "26 " << time << "   " << sum << '\n';

        m_results[index].m_naiveSSE2_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("naive_sse2_nocache_2vectors.freq");
        CPU_COOLING

    }

    save("naive_sse2_nocache_2vectors.dat", &Times::m_naiveSSE2_nocache);

}


void AddVectorBenchmark::stableRelAbsDenseToDenseSSE2Self()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_SSE2_CACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_CACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "27 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsSSE2 = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_sse2_2vectors.freq");
        CPU_COOLING

    }

    save("absrel_sse2_2vectors.dat", &Times::m_relAbsSSE2);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "28 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsSSE2_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_sse2_nocache_2vectors.freq");
        CPU_COOLING

    }

    save("absrel_sse2_nocache_2vectors.dat", &Times::m_relAbsSSE2_nocache);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysSSE2Self()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_SSE2_CACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_SSE2_CACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "29 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsOrchardSSE2 = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_sse2_2vectors_or.freq");
        CPU_COOLING

    }

    save("absrel_sse2_2vectors_or.dat", &Times::m_relAbsOrchardSSE2);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "30 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsOrchardSSE2_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_sse2_nocache_2vectors_or.freq");
        CPU_COOLING

    }

    save("absrel_sse2_nocache_2vectors_or.dat", &Times::m_relAbsOrchardSSE2_nocache);
}



void AddVectorBenchmark::stableAbsDenseToDenseSSE2Self()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_SSE2_CACHE(m_a, m_b, m_a, len, lambda, absTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "31 " << time << "   " << sum << '\n';

        m_results[index].m_absSSE2 = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("abs_sse2_2vectors.freq");
        CPU_COOLING

    }

    save("abs_sse2_2vectors.dat", &Times::m_absSSE2);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_SSE2_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "32 " << time << "   " << sum << '\n';

        m_results[index].m_absSSE2_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("abs_sse2_nocache_2vectors.freq");
        CPU_COOLING
    }

    save("abs_sse2_nocache_2vectors.dat", &Times::m_absSSE2_nocache);
}


void AddVectorBenchmark::naiveAddDenseToDenseAVXSelf()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_AVX_CACHE(m_a, m_b, m_a, len, lambda);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "33 " << time << "   " << sum << '\n';

        m_results[index].m_naiveAVX = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("naive_avx_2vectors.freq");
        CPU_COOLING

    }

    save("naive_avx_2vectors.dat", &Times::m_naiveAVX);


    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_AVX_NOCACHE(m_a, m_b, m_a, len, lambda);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "34 " << time << "   " << sum << '\n';

        m_results[index].m_naiveAVX_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("naive_avx_nocache_2vectors.freq");
        CPU_COOLING

    }

    save("naive_avx_nocache_2vectors.dat", &Times::m_naiveAVX_nocache);
}


void AddVectorBenchmark::stableRelAbsDenseToDenseAVXSelf()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_AVX_CACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_AVX_CACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "35 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsAVX = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_avx_2vectors.freq");
        CPU_COOLING

    }

    save("absrel_avx_2vectors.dat", &Times::m_relAbsAVX);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_AVX_NOCACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_AVX_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "36 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsAVX_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_avx_nocache_2vectors.freq");
        CPU_COOLING

    }
    save("absrel_avx_nocache_2vectors.dat", &Times::m_relAbsAVX_nocache);
}

void AddVectorBenchmark::stableRelAbsDenseToDenseOrchardHaysAVXSelf()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len<< std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_AVX_CACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_AVX_CACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "37 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsOrchardAVX = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_avx_2vectors_or.freq");
        CPU_COOLING

    }

    save("absrel_avx_2vectors_or.dat", &Times::m_relAbsOrchardAVX);


    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_REL_ORCHARD_AVX_NOCACHE(m_a, m_b, m_a, len, lambda, relTolerance);
            //DENSE_TO_DENSE_ADD_ABS_REL_ORCHARD_AVX_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance, relTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "38 " << time << "   " << sum << '\n';

        m_results[index].m_relAbsOrchardAVX_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("absrel_avx_nocache_2vectors_or.freq");
        CPU_COOLING

    }

    save("absrel_avx_nocache_2vectors_or.dat", &Times::m_relAbsOrchardAVX_nocache);
}


void AddVectorBenchmark::stableAbsDenseToDenseAVXSelf()
{
    std::cout << std::endl;
    int index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_AVX_CACHE(m_a, m_b, m_a, len, lambda, absTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "39 " << time << "   " << sum << '\n';

        m_results[index].m_absAVX = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("abs_avx_2vectors.freq");
        CPU_COOLING

    }
    save("abs_avx_2vectors.dat", &Times::m_absAVX);

    index = 0;

    for (auto len: m_lengths) {
        if (index < 26) {
            REPEAT = 50000;
        } else if (index < 53) {
            REPEAT = 5000;
        } else {
            REPEAT = 500;
        }

        std::cout << "Length: " << len << std::endl;
        int r;
        double sum = 0.0;
        int index2;

        unsigned long long int time = 0;
        unsigned long long int start, end;
        decltype(readTimeStampCounter()) min = -1;
        for (r = 0; r < REPEAT; r++) {
            memcpy(m_a, m_a2, len * sizeof(double));
            // preparing the cache
            for (index2 = 0; index2 < len; index2++) {
                sum += m_a[index2] + m_b[index2];
            }
            start = readTimeStampCounter();
            DENSE_TO_DENSE_ADD_ABS_AVX_NOCACHE(m_a, m_b, m_a, len, lambda, absTolerance);
            end = readTimeStampCounter();
            auto elapsed = end - start;
            if (elapsed < min) min = elapsed;
            //auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
            time += elapsed;
        }

        //        //auto elapsed2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        std::cout << "40 " << time << "   " << sum << '\n';

        m_results[index].m_absAVX_nocache = getMFLOPS2(len, min * REPEAT);
        index++;
        saveFreq("abs_avx_nocache_2vectors.freq");
        CPU_COOLING

    }
    save("abs_avx_nocache_2vectors.dat", &Times::m_absAVX_nocache);
}


void AddVectorBenchmark::init()
{
    std::cout << "allitsd be amit be akarsz allitani chrt -r -p [1..99] {pid} " << std::endl;
    std::cout << "PID: " << getpid() << std::endl;
    std::cin.get();
    //std::cout << "hali" << std::endl;
    unsigned int len = 1000;
    double mul = 1.1;
    unsigned int i;
    bool first = true;
    if (m_lengths.size() == 0) {
        for (i = 0; i < 80; i++) {
            m_lengths.push_back(len * pow(mul, i));
        }
        m_results.resize(m_lengths.size());


        m_savedAbsTolerance = Numerical::AbsoluteTolerance;
        m_savedRelTolerance = Numerical::RelativeTolerance;
        Numerical::AbsoluteTolerance = absTolerance;
        Numerical::RelativeTolerance = relTolerance;

        m_a = alloc<double, 64>( m_lengths[ m_lengths.size() - 1 ] );
        m_b = alloc<double, 64>( m_lengths[ m_lengths.size() - 1 ] );
        m_c = alloc<double, 64>( m_lengths[ m_lengths.size() - 1 ] );
        m_a2 = alloc<double, 64>( m_lengths[ m_lengths.size() - 1 ] );
        m_b2 = alloc<double, 64>( m_lengths[ m_lengths.size() - 1 ] );

    } else {
        first = false;
    }
    for (i = 0; i <  m_lengths[ m_lengths.size() - 1 ] ; i++) {
        double a = 0, b = 0;
        switch (rand() % 2) {
        case 0:
            a = rand() % 10000 / 10.0;
            b = rand() % 10000 / 10.0;
            break;
        case 2:
            a = rand() % 10000000 / 10.0;
            b = -(a / lambda) * (1.0 + relTolerance / 10.0);
            break;
        case 1:
            a = rand() % 100;
            b = -a * (1.0 + absTolerance);
            break;
        }

        /*case 1:
            a = rand() % 10000000 / 10.0;
            b = -(a / lambda) * (1.0 + relTolerance / 10.0);
            break;
        case 2:
            a = rand() % 100;
            b = -a * (1.0 + absTolerance);
            break;
        }*/
        m_a[i] = a;
        m_b[i] = b;
        m_a2[i] = a;
        m_b2[i] = b;
    }

    if (first) {
        int counters[3] = {0, 0, 0};
        for (i = 0; i <  m_lengths[ m_lengths.size() - 1 ] ; i++) {
            double c = m_a[i] + lambda * m_b[i];
            //if ((fabs(m_a[i]) + fabs(lambda*m_b[i]))*relTolerance > fabs(c) ) {
            if ((fabs(m_a[i]) )*relTolerance > fabs(c) ) {
                c = 0.0;
                counters[1]++;
            } else {
                if (c < absTolerance) {
                    c = 0;
                    counters[2]++;
                } else {
                    counters[0]++;
                }
            }
        }
        std::cout << "stable: " << counters[0] << std::endl;
        std::cout << "rel: " << counters[1] << std::endl;
        std::cout << "abs: " << counters[2] << std::endl;
    }
    std::cin.get();
}

void AddVectorBenchmark::release()
{
    std::cout << "RELEASE" << std::endl;
    ::release(m_a);
    ::release(m_b);
    ::release(m_c);

    Numerical::AbsoluteTolerance = m_savedAbsTolerance;
    Numerical::RelativeTolerance = m_savedRelTolerance;

    std::ofstream ofs("add_benchmark0.csv");
    ofs << "size;"
           "naive;"
           "abs rel;"
           "abs rel orchard;"
           "abs;"
           "naive SSE2;"
           "naive SSE no cache;"
           "abs rel SSE2;"
           "abs rel SSE2 no cache;"
           "abs rel Orchard SSE2;"
           "abs rel Orchard SSE2 no cache;"
           "abs SSE2;"
           "abs SSE2 no cache;"
           "naive AVX;"
           "naive AVX no cache;"
           "abs rel AVX;"
           "abs rel AVX no cache;"
           "abs rel Orchard AVX;"
           "abs rel Orchard AVX no cache;"
           "abs AVX;"
           "abs AVX no cache;" << std::endl;
    unsigned int index;
    for (index = 0; index < m_lengths.size(); index++) {
        ofs << m_lengths[index] << ";";
        ofs << m_results[index].m_naive << ";";
        ofs << m_results[index].m_relAbs << ";";
        ofs << m_results[index].m_relAbsOrchard << ";";
        ofs << m_results[index].m_abs << ";";
        ofs << m_results[index].m_naiveSSE2 << ";";
        ofs << m_results[index].m_naiveSSE2_nocache << ";";
        ofs << m_results[index].m_relAbsSSE2 << ";";
        ofs << m_results[index].m_relAbsSSE2_nocache << ";";
        ofs << m_results[index].m_relAbsOrchardSSE2 << ";";
        ofs << m_results[index].m_relAbsOrchardSSE2_nocache << ";";
        ofs << m_results[index].m_absSSE2 << ";";
        ofs << m_results[index].m_absSSE2_nocache << ";";
        ofs << m_results[index].m_naiveAVX << ";";
        ofs << m_results[index].m_naiveAVX_nocache << ";";
        ofs << m_results[index].m_relAbsAVX << ";";
        ofs << m_results[index].m_relAbsAVX_nocache << ";";
        ofs << m_results[index].m_relAbsOrchardAVX << ";";
        ofs << m_results[index].m_relAbsOrchardAVX_nocache << ";";
        ofs << m_results[index].m_absAVX << ";";
        ofs << m_results[index].m_absAVX_nocache << ";" << std::endl;
    }
    ofs.close();
}

double AddVectorBenchmark::getMFLOPS(unsigned int length, unsigned int time)
{
    return (((1000000.0 / (((double)time / REPEAT) / length))) / 1000000.0) * 2;
}

double AddVectorBenchmark::getMFLOPS2(unsigned int length, unsigned long long time)
{
    double t = time / 25000.0; // microsec
    return ((((1000000.0 / ((t / REPEAT) / length))) / 1000000.0) * 2) / 10.0;
}

void AddVectorBenchmark::save(const char *fileName, double Times::*pTime) const
{
    std::ofstream ofs(fileName);
    ofs << "x_0     f(x)" << std::endl;


    unsigned int i;
    for (i = 0; i < m_results.size(); i++) {
        ofs << m_lengths[i] << "  ";
        std::ostringstream str;
        str << m_results[i].*pTime;
        std::string str2 = str.str();
        unsigned int j;
        for (j = 0; j < str2.length(); j++) {
            if (str2[j] == ',') {
                str2[j] = '.';
            }
        }
        ofs << str2 << std::endl;
    }


    ofs.close();

    // cpu cooling
    system("sleep 5");
}

void AddVectorBenchmark::saveFreq(const char * fileName) const {
    std::string cmd = std::string("lscpu | grep \"^CPU MHz:\" | sed 's,^.* \([0-9].*\)$,\1,' >> ") + fileName;
    system(cmd.c_str());
}
