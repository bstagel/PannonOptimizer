#include <coretest.h>
#include <globals.h>
#include <initpanopt.h>
#include <debug.h>
#include <cmath>
#include <utils/arch/x86.h>
#include <utils/primitives.h>
#include <limits>

#define MEMCPY_TESTBUFFER_MAX_SIZE      1024*10
#define MEMCPY_FIX_BORDER               10
#define MEMCPY_MAX_SMALL_SIZE           250

#define DOT_PRODUCT_MAX_SIZE            1024

enum MEMCPY_SIZE_CASES {
    LARGE,
    SMALL,
    EMPTHY
};

CoreTestSuite::CoreTestSuite(const char *name): UnitTest(name) {
    //ADD_TEST(CoreTestSuite::memcpy);
    //ADD_TEST(CoreTestSuite::memset);
    //ADD_TEST(CoreTestSuite::denseToDenseDotProduct);
    //ADD_TEST(CoreTestSuite::denseToSparseDotProduct);
    ADD_TEST(CoreTestSuite::denseToDenseAdd);
}

void CoreTestSuite::memcpy() {
    unsigned int alignIndex;
    for (alignIndex = 0; alignIndex <= 1; alignIndex++) {
        char * testBufferA;
        char * testBufferB;
        if (alignIndex == 0) {
            testBufferB = new char[MEMCPY_TESTBUFFER_MAX_SIZE];
            testBufferA = new char[MEMCPY_TESTBUFFER_MAX_SIZE];
        } else {
            testBufferA = alloc<char, 16>(MEMCPY_TESTBUFFER_MAX_SIZE);
            testBufferB = alloc<char, 16>(MEMCPY_TESTBUFFER_MAX_SIZE);
        }

        unsigned int index;
        unsigned int startIndex;
        unsigned int endIndex;
        unsigned int sizeIndex;
        unsigned int testCase;

        for (index = 0; index < MEMCPY_TESTBUFFER_MAX_SIZE; index++) {
            testBufferA[index] = (index * index) % CHAR_MAX;
            testBufferB[index] = 0;
        }

        for (testCase = 0; testCase <= 2; testCase++) {
            switch (testCase) {
            case 0:
                LPINFO("Classic memcpy");
                break;
            case 1:
                LPINFO("Copy with SSE2, using cache");
                break;
            case 2:
                LPINFO("Copy with SSE4-1, without cache");
                break;
            }

            // copy from testBufferA to testBufferB
            for (startIndex = 0; startIndex <= 100; startIndex++) {
                for (endIndex = 0; endIndex <= 100; endIndex++) {
                    for (sizeIndex = 0; sizeIndex < 3; sizeIndex++) {

                        for (index = 0; index < MEMCPY_TESTBUFFER_MAX_SIZE; index++) {
                            testBufferB[index] = 0;
                        }


                        unsigned int start = MEMCPY_FIX_BORDER + startIndex;
                        unsigned int end;
                        switch (sizeIndex) {
                        case LARGE:
                            end = MEMCPY_TESTBUFFER_MAX_SIZE - MEMCPY_FIX_BORDER - endIndex;
                            break;
                        case SMALL:
                            end = MEMCPY_MAX_SMALL_SIZE - MEMCPY_FIX_BORDER - endIndex;
                            break;
                        case EMPTHY:
                            end = start;
                            break;
                        }
                        //end = start;
                        unsigned int size = end - start;

                        switch (testCase) {
                        case 0:
                            ::memcpy(testBufferB + start, testBufferA + start, size);
                            break;
                        case 1:
                            if (InitPanOpt::getInstance().getArchitecture().featureExists("SSE2") ) {
                                MEMCPY_CACHE_SSE2(testBufferB + start, testBufferA + start, size);
                            }
                            break;
                        case 2:
                            if (InitPanOpt::getInstance().getArchitecture().featureExists("SSE4-1") ) {
                                MEMCPY_NO_CACHE_SSE4_1(testBufferB + start, testBufferA + start, size);
                            }

                        }

                        // checking
                        for (index = 0; index < start; index++) {
                            if (testBufferB[index] != 0) {
                                TEST_ASSERT(testBufferB[index] == 0);
                                return;
                            }
                        }
                        for (index = start; index < end; index++) {
                            if (testBufferA[index] != testBufferB[index]) {
                                LPERROR( testBufferA[index] << " != " << testBufferB[index] );
                                TEST_ASSERT(testBufferA[index] == testBufferB[index]);
                                return;
                            }
                        }
                        for (index = end; index < MEMCPY_TESTBUFFER_MAX_SIZE; index++) {
                            if (testBufferB[index] != 0) {
                                TEST_ASSERT(testBufferB[index] == 0);
                                return;
                            }
                        }

                    }
                }
            }

        }
        if (alignIndex == 0) {
            delete [] testBufferA;
            delete [] testBufferB;
        } else {
            release(testBufferA);
            release(testBufferB);
        }
    }
}

void CoreTestSuite::memset() {

}

void CoreTestSuite::denseToDenseDotProduct() {
    double * arrayA = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayB = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayARef = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayBRef = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);

    unsigned int index;
    unsigned int testCase;
    // generate stable input
    srand(0);
    for (index = 0; index < DOT_PRODUCT_MAX_SIZE; index++) {
        if (index < 100) {
            arrayA[index] = index / 16.0;
            arrayB[index] = (index * 1.25) * pow(-1, index);
        } else {
            arrayA[index] = (rand() % 1000 / 64.0) - 3;
            arrayB[index] = (rand() % 1000 / 16.0) - 6;
        }
        arrayARef[index] = arrayA[index];
        arrayBRef[index] = arrayB[index];
    }

    for (testCase = 0; testCase <= 50; testCase++) {
        unsigned int count;
        if (testCase < 40) {
            count = testCase;
        } else {
            count = testCase * 20;
        }
        double result;
        double resultRef = 0;
        for (index = 0; index < count; index++) {
            resultRef += arrayA[index + 4] * arrayB[index + 4];
        }
        unsigned int function;
        for (function = 0; function <= 3; function++) {
            result = rand() % 100 + 10;
            switch (function) {
            case 0: // default, platform independent implementation
                continue;
                break;
            case 1:
                result = denseToDenseDotProductUnstable(arrayA + 4, arrayB + 4, count);
                /*if (InitPanOpt::getInstance().getArchitecture().featureExists("FPU") &&
                        InitPanOpt::getInstance().getArchitecture().featureExists("CMOV")) {
                    continue;
                } else {

                }*/
                break;
            case 2:
                if (InitPanOpt::getInstance().getArchitecture().featureExists("SSE2") ) {
                    result = DENSE_TO_DENSE_DOTPRODUCT_UNSTABLE_SSE2(arrayA + 4, arrayB + 4, count);
                }
                break;
            case 3:
                if (InitPanOpt::getInstance().getArchitecture().featureExists("AVX") ) {
                    result = DENSE_TO_DENSE_DOTPRODUCT_UNSTABLE_AVX(arrayA + 4, arrayB + 4, count);
                }
                break;
            }
            TEST_ASSERT(result == resultRef);
            if (result != resultRef) {
                LPINFO(count << "   " << result << " != " << resultRef);
                return;
            }
            for (index = 0; index < DOT_PRODUCT_MAX_SIZE; index++) {
                if (arrayA[index] != arrayARef[index]) {
                    TEST_ASSERT(arrayA[index] == arrayARef[index]);
                    return;
                }
                if (arrayB[index] != arrayBRef[index]) {
                    TEST_ASSERT(arrayB[index] == arrayBRef[index]);
                    return;
                }
            }

        }
    }


    release(arrayA);
    release(arrayB);
}

void CoreTestSuite::denseToSparseDotProduct() {
    double * arrayA = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayB = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayARef = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    double * arrayBRef = alloc<double, 32>(DOT_PRODUCT_MAX_SIZE);
    unsigned int * indicesB = alloc<unsigned int, 32>(3);
    indicesB[0] = 1000;
    indicesB[1] = 1020;
    indicesB[2] = 1120;
    indicesB[3] = 1156;
    indicesB[4] = 1220;
    indicesB[5] = 1320;
    indicesB[6] = 1420;
    indicesB[7] = 1520;
    indicesB[8] = 1620;
    indicesB[9] = 2156;
    unsigned int index;
    for (index = 0; index < DOT_PRODUCT_MAX_SIZE; index++) {
        arrayA[index] = 0;
        arrayB[index] = 0;
    }
    arrayB[0] = -1;
    arrayB[1] = -2;
    arrayB[2] = -3;
    arrayB[3] = -4;
    arrayB[4] = -5;
    arrayB[5] = -6;
    arrayB[6] = -7;
    arrayB[7] = -8;
    arrayB[8] = -9;
    arrayB[9] = -10;
    arrayA[1000] = 1;
    arrayA[1020] = 2;
    arrayA[1120] = 3;
    arrayA[1156] = 4;
    arrayA[1220] = 5;
    arrayA[1320] = 6;
    arrayA[1420] = 7;
    arrayA[1520] = 8;
    arrayA[1620] = 9;
    arrayA[2156] = 10;

    LPINFO(arrayA);
    LPINFO(arrayB);
    LPINFO(indicesB);
    double res = DENSE_TO_SPARSE_DOTPRODUCT_UNSTABLE_AVX(arrayA,
                                                         arrayB,
                                                         indicesB,
                                                         10);
    LPINFO("res = " << res);

    unsigned int aSize = 1000000;
    unsigned int bSize = 10;
    unsigned int indexSize = bSize;
    unsigned int repeat = 10000000;
    unsigned int repIndex;

    double * a = alloc<double, 32>(aSize);
    double * b = alloc<double, 32>(bSize);
    unsigned int * indices = alloc<unsigned int, 32>(indexSize);
    for (index = 0; index < aSize; index++) {
        a[index] = rand() % 10000 / 1000.0 - 6;
    }
    for (index = 0; index < bSize; index++) {
        b[index] = rand() % 10000 / 1000.0;
    }
    // fill indices with random indices
    for (index = 0; index < indexSize; index++) {
        indices[index] = index;
    }
    for (index = 0; index < indexSize * 100; index++) {
        int i = rand() % indexSize;
        int j = rand() % indexSize;
        unsigned int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
    clock_t start, end;
    res = 0;
    start = clock();
    for (repIndex = 0; repIndex < repeat; repIndex++) {
        double neg = 0.0;
        res = DENSE_TO_SPARSE_DOTPRODUCT_STABLE_AVX(a, b, indices, indexSize, &neg);
        res += neg;
        //res = DENSE_TO_SPARSE_DOTPRODUCT_UNSTABLE_SSE2(a, b, indices, indexSize);
    }
    end = clock();
    LPINFO("time = " << ( end - start ) / (double)CLOCKS_PER_SEC );
    LPINFO("res = " << res);
    //return;

    start = clock();
    for (repIndex = 0; repIndex < repeat; repIndex++) {
        res = DENSE_TO_SPARSE_DOTPRODUCT_UNSTABLE_AVX(a, b, indices, indexSize);
    }
    end = clock();
    LPINFO("time = " << ( end - start ) / (double)CLOCKS_PER_SEC );
    LPINFO("res = " << res);

}

extern "C" void _denseToDenseAddAbsRelAVX_cache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda,
        double absTolerance,
        double relTolerance);

extern "C" void _denseToDenseAddAbsRelAVX_nocache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda,
        double absTolerance,
        double relTolerance);

extern "C" void _denseToDenseAddAbsAVX_cache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda,
        double absTolerance);

extern "C" void _denseToDenseAddAbsAVX_nocache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda,
        double absTolerance);

extern "C" void _denseToDenseAddAVX_cache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda);

extern "C" void _denseToDenseAddAVX_nocache_64_linux(
        const double * a,
        const double * b,
        const double * c,
        unsigned int count,
        double lambda);

void CoreTestSuite::denseToDenseAdd()
{
    unsigned int i;
    unsigned int size;

    double * a = alloc<double, 32>(11);
    double * b = alloc<double, 32>(11);
    double * c = alloc<double, 32>(11);

    a[0] = 1;   b[0] = -1.1;
    a[1] = 0.001;   b[1] = -0.0011;
    a[2] = 10000;   b[2] = -10000.1;
    a[3] = 4;   b[3] = 4;
    a[4] = 5;   b[4] = 5;
    a[5] = 6;   b[5] = 6;
    a[6] = 7;   b[6] = -7.5;
    a[7] = 8;   b[7] = 8;
    a[8] = 9;   b[8] = 9;
    a[9] = 10;  b[9] = 10;
    a[10] = 11; b[10] = 11;

    Numerical::Double oldAbsTolerance = Numerical::AbsoluteTolerance;
    Numerical::Double oldRelTolerance = Numerical::RelativeTolerance;

    double lambda = 1.0;
    double absTolerance = 0.01;
    double relTolerance = 0.04;

    Numerical::AbsoluteTolerance = absTolerance;
    Numerical::RelativeTolerance = relTolerance;

    double nan = std::numeric_limits<double>::quiet_NaN();

    for (size = 0; size <= 11; size++) {
        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        _denseToDenseAddAbsRelAVX_nocache_64_linux(a, b, c, size, lambda, absTolerance, relTolerance);
        //DENSE_TO_DENSE_ADD_ABS_REL_SSE2_CACHE(a, b, c, size, lambda, absTolerance, relTolerance);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == Numerical::stableAdd(a[i], b[i] * lambda));
            if (c[i] != Numerical::stableAdd(a[i], b[i] * lambda)) {
                LPERROR(c[i] << " != " << Numerical::stableAdd(a[i], b[i] * lambda));
            }
        }

        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        DENSE_TO_DENSE_ADD_ABS_REL_SSE2_NOCACHE(a, b, c, size, lambda, absTolerance, relTolerance);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == Numerical::stableAdd(a[i], b[i] * lambda));
            if (c[i] != Numerical::stableAdd(a[i], b[i] * lambda)) {
                LPERROR(c[i] << " != " << Numerical::stableAdd(a[i], b[i] * lambda));
            }
        }

        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        _denseToDenseAddAbsAVX_nocache_64_linux(a, b, c, size, lambda, absTolerance);
        //DENSE_TO_DENSE_ADD_ABS_SSE2_CACHE(a, b, c, size, lambda, absTolerance);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == Numerical::stableAddAbs(a[i], b[i] * lambda));
            if (c[i] != Numerical::stableAddAbs(a[i], b[i] * lambda)) {
                LPERROR(c[i] << " != " << Numerical::stableAddAbs(a[i], b[i] * lambda));
            }
        }

        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        DENSE_TO_DENSE_ADD_ABS_SSE2_NOCACHE(a, b, c, size, lambda, absTolerance);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == Numerical::stableAddAbs(a[i], b[i] * lambda));
            if (c[i] != Numerical::stableAddAbs(a[i], b[i] * lambda)) {
                LPERROR(c[i] << " != " << Numerical::stableAddAbs(a[i], b[i] * lambda));
            }
        }

        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        _denseToDenseAddAVX_nocache_64_linux(a, b, c, size, lambda);
        //DENSE_TO_DENSE_ADD_SSE2_CACHE(a, b, c, size, lambda);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == a[i] + b[i] * lambda);
            if (c[i] != a[i] + b[i] * lambda) {
                LPERROR(c[i] << " != " << a[i] + b[i] * lambda);
            }
        }

        for (i = 0; i < 11; i++) {
            c[i] = nan;
        }

        DENSE_TO_DENSE_ADD_SSE2_NOCACHE(a, b, c, size, lambda);
        for (i = 0; i < size; i++) {
            TEST_ASSERT(c[i] == a[i] + b[i] * lambda);
            if (c[i] != a[i] + b[i] * lambda) {
                LPERROR(c[i] << " != " << a[i] + b[i] * lambda);
            }
        }

    }

    Numerical::AbsoluteTolerance = oldAbsTolerance;
    Numerical::RelativeTolerance = oldRelTolerance;

}
