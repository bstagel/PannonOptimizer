/**
 * @file timer.h This file contains the API of the Timer class.
 * @author tar
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <globals.h>
#include <time.h>

#ifdef WIN32   // Windows system specific
    #include <windows.h>
#else          // Unix based system specific
    #include <sys/time.h>
#endif

/**
 * This class is used to measure elapsed time during various operations the program implements.
 *
 * @class Timer
 */
class Timer
{
public:
    /**
     * Default constructor of the Timer class.
     *
     * @constructor
     */
    Timer();

    /**
     * Starts measuring the elapsed time.
     * The timer can be started multiple times with pauses between them.
     */
    void start();

    void reset();

    /**
     * Stops the measurement of elapsed time.
     */
    void stop();

    //Returns seconds with microsec accuracy, the timer must be stopped before
    /**
     * Returns the total elapsed time.
     * The total elapsed time is the sum of part times between the multiple starts and stops of the timer.
     *
     * @return The elapsed time in seconds with microsec accuracy.
     */
    double getTotalElapsed() const;

    /**
     * Returns the last elapsed time.
     * The last elapsed time is the time between the last start and stop of the timer.
     *
     * @return The elapsed time in seconds with microsec accuracy.
     */
    double getLastElapsed() const;

    /**
     * Returns the average elapsed time.
     * The average elapsed time is the average time between the multiple starts and stops of the timer.
     *
     * @return The elapsed time in seconds with microsec accuracy.
     */
    double getAverageElapsed() const;

    //Return the actual time if the timer is running
    /**
     * Returns the actual time if the timer is running.
     *
     * @return The elapsed time in seconds with microsec accuracy.
     */
    double getRunningTime() const;

    //Returns CPU time in seconds with clock (millisec) accuracy, the timer must be stopped before
    /**
     * Returns the total elapsed time with clock accuracy.
     * The total elapsed time is the sum of part times between the multiple starts and stops of the timer.
     *
     * @return The elapsed time in seconds with clock accuracy.
     */
    double getCPUTotalElapsed() const;

    /**
     * Returns the last elapsed time with clock accuracy.
     * The last elapsed time is the time between the last start and stop of the timer.
     *
     * @return The elapsed time in seconds with clock accuracy.
     */
    double getCPULastElapsed() const;

    /**
     * Returns the average elapsed time with clock accuracy.
     * The average elapsed time is the average time between the multiple starts and stops of the timer.
     *
     * @return The elapsed time in seconds with clock accuracy.
     */
    double getCPUAverageElapsed() const;

    //Return the actual CPU time if the timer is running
    /**
     * Returns the elapsed time from the last start of the timer.
     *
     * @return The elapsed time in seconds with clock accuracy.
     */
    double getCPURunningTime() const;

private:

    /**
     * Indicates whether the time is running.
     */
    bool m_started;

    /**
     * The times the timer have been started.
     */
    unsigned int m_startCount;

    /**
     * The time when the last timer session started.
     */
    clock_t m_clockStart;

    /**
     * The time when the last timer session ended.
     */
    clock_t m_clockEnd;

    /**
     * The time between the last start and stop of the timer.
     */
    clock_t m_clockLastElapsed;

    /**
     * The total time between the starts and stops of the timer.
     */
    clock_t m_clockTotalElapsed;

    /**
     * The average time between the starts and stops of the timer.
     */
    clock_t m_clockAverageElapsed;

#ifdef WIN32
    /**
     * The frequency of the CPU ticks.
     */
    LARGE_INTEGER m_frequency;

    /**
     * The CPU tick when the last timer session started.
     */
    LARGE_INTEGER m_start;

    /**
     * The CPU tick when the last timer session ended.
     */
    LARGE_INTEGER m_end;

    /**
     * The CPU ticks between the last start and stop of the timer.
     */
    LARGE_INTEGER m_lastElapsed;

    /**
     * The total CPU ticks between the starts and stops of the timer.
     */
    LARGE_INTEGER m_totalElapsed;

    /**
     * The average CPU ticks between the starts and stops of the timer.
     */
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
