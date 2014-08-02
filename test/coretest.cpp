#include <coretest.h>
#include <globals.h>
#include <initpanopt.h>
#include <debug.h>
#include <cmath>
#include <utils/arch/x86.h>
#include <utils/primitives.h>

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
    ADD_TEST(CoreTestSuite::memset);
    ADD_TEST(CoreTestSuite::denseToDenseDotProduct);
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
                    result = DENSE_TO_DENSE_UNSTABLE_SSE2(arrayA + 4, arrayB + 4, count);
                }
                break;
            case 3:
                if (InitPanOpt::getInstance().getArchitecture().featureExists("AVX") ) {
                    result = DENSE_TO_DENSE_UNSTABLE_AVX(arrayA + 4, arrayB + 4, count);
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
