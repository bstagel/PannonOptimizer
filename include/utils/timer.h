/**
 * @file timer.h
 */

#ifndef TIMER_H_
#define TIMER_H_


#ifdef WIN32   // Windows system specific
    #include <windows.h>
#else          // Unix based system specific
    #include <sys/time.h>
#endif


class Timer
{
public:
    Timer();

    void start();
    void stop();

    //Returns seconds with microsec accuracy
    double getTotalElapsed();
    double getLastElapsed();
    double getAverageElapsed();

private:
    bool m_started;
    unsigned int m_startCount;

#ifdef WIN32
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_end;
    LARGE_INTEGER m_lastElapsed;
    LARGE_INTEGER m_totalElapsed;
    LARGE_INTEGER m_averageElapsed;
#else
    timeval m_start;
    timeval m_end;
    timeval m_lastElapsed;
    timeval m_totalElapsed;
    timeval m_averageElapsed;
#endif
};

#endif /* TIMER_H_ */