#ifndef ADDVECTORBENCHMARK_H
#define ADDVECTORBENCHMARK_H

#include <framework/tester.h>
#include <framework/unittest.h>

class AddVectorBenchmark : public UnitTest
{
public:
    AddVectorBenchmark(const char * name);

    ~AddVectorBenchmark();
private:

    struct Times {
        double m_naive;
        double m_relAbs;
        double m_abs;
        double m_relAbsOrchard;

        double m_naiveSSE2;
        double m_naiveSSE2_nocache;
        double m_relAbsSSE2;
        double m_relAbsSSE2_nocache;
        double m_relAbsOrchardSSE2;
        double m_relAbsOrchardSSE2_nocache;
        double m_absSSE2;
        double m_absSSE2_nocache;

        double m_naiveAVX;
        double m_naiveAVX_nocache;
        double m_relAbsAVX;
        double m_relAbsAVX_nocache;
        double m_relAbsOrchardAVX;
        double m_relAbsOrchardAVX_nocache;
        double m_absAVX;
        double m_absAVX_nocache;
    };

    std::vector<Times> m_results;
    std::vector<unsigned int> m_lengths;

    double * m_a;
    double * m_b;
    double * m_a2;
    double * m_b2;
    double * m_c;
    double m_savedAbsTolerance;
    double m_savedRelTolerance;

    void naiveAddDenseToDense();

    void stableRelAbsDenseToDenseOrchardHays();

    void stableRelAbsDenseToDense();

    void stableAbsDenseToDense();

    void naiveAddDenseToDenseSSE2();

    void stableRelAbsDenseToDenseSSE2();

    void stableRelAbsDenseToDenseOrchardHaysSSE2();

    void stableAbsDenseToDenseSSE2();

    void naiveAddDenseToDenseAVX();

    void stableRelAbsDenseToDenseAVX();

    void stableRelAbsDenseToDenseOrchardHaysAVX();

    void stableAbsDenseToDenseAVX();

    /////////////////////////////////

    void naiveAddDenseToDenseSelf();

    void stableRelAbsDenseToDenseSelfOrchardHays();

    void stableRelAbsDenseToDenseSelf();

    void stableAbsDenseToDenseSelf();

    void naiveAddDenseToDenseSSE2Self();

    void stableRelAbsDenseToDenseSSE2Self();

    void stableRelAbsDenseToDenseOrchardHaysSSE2Self();

    void stableAbsDenseToDenseSSE2Self();

    void naiveAddDenseToDenseAVXSelf();

    void stableRelAbsDenseToDenseAVXSelf();

    void stableRelAbsDenseToDenseOrchardHaysAVXSelf();

    void stableAbsDenseToDenseAVXSelf();

    ////////////////

    void init();
    void release();
    double getMFLOPS(unsigned int length, unsigned int time);
    double getMFLOPS2(unsigned int length, unsigned long long int time);
    void save(const char * fileName, double Times::*pTime) const;
    void saveFreq(const char * fileName) const;
};

#endif // ADDVECTORBENCHMARK_H
