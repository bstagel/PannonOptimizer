/**
 * @file timer.cpp
 */

#include <stdlib.h>

#include <utils/timer.h>

#include <debug.h>

Timer::Timer():
    m_started(false),
    m_startCount(0),
    m_clockStart(0),
    m_clockEnd(0),
    m_clockLastElapsed(0),
    m_clockTotalElapsed(0),
    m_clockAverageElapsed(0)
{
#ifdef WIN32
    QueryPerformanceFrequency(&m_frequency);
    m_start.QuadPart = 0;
    m_end.QuadPart = 0;
    m_lastElapsed.QuadPart = 0;
    m_totalElapsed.QuadPart = 0;
    m_averageElapsed.QuadPart = 0;
#else
    m_start.tv_sec = m_start.tv_usec = 0;
    m_end.tv_sec = m_end.tv_usec = 0;
    m_lastElapsed.tv_sec = m_lastElapsed.tv_usec = 0;
    m_totalElapsed.tv_sec = m_totalElapsed.tv_usec = 0;
    m_averageElapsed.tv_sec = m_averageElapsed.tv_usec = 0;
#endif
}

void Timer::start()
{
    if(m_started){
        return;
    }
    m_started = true;
    m_startCount++;

    m_clockStart = clock();
#ifdef WIN32
    QueryPerformanceCounter(&m_start);
#else
    gettimeofday(&m_start, NULL);
#endif
}

void Timer::stop()
{

    if(!m_started){
        return;
    }

    m_started = false;

    m_clockEnd = clock();
    m_clockLastElapsed = m_clockEnd - m_clockStart;
    m_clockTotalElapsed += m_clockLastElapsed;
    m_clockAverageElapsed = m_clockTotalElapsed / m_startCount;

#ifdef WIN32
    QueryPerformanceCounter(&m_end);
    m_lastElapsed.QuadPart = m_end.QuadPart-m_start.QuadPart;
    m_totalElapsed.QuadPart += m_lastElapsed.QuadPart;
    m_averageElapsed.QuadPart = m_totalElapsed.QuadPart / m_startCount;
#else
    gettimeofday(&m_end, NULL);

    unsigned long long int startTime = m_start.tv_sec * 1000000 + m_start.tv_usec;
    unsigned long long int endTime = m_end.tv_sec * 1000000 + m_end.tv_usec;
    unsigned long long int diffTime = endTime - startTime;
    m_lastElapsed.tv_sec = diffTime / 1000000;
    m_lastElapsed.tv_usec = diffTime % 1000000;
    unsigned long long int totalElapsed = m_totalElapsed.tv_sec * 1000000 + m_totalElapsed.tv_usec;
    totalElapsed += diffTime;
    m_totalElapsed.tv_sec = totalElapsed / 1000000;
    m_totalElapsed.tv_usec = totalElapsed % 1000000;
    totalElapsed /= m_startCount;
    m_averageElapsed.tv_sec = totalElapsed / 1000000;
    m_averageElapsed.tv_usec = totalElapsed % 1000000;
#endif
}

double Timer::getLastElapsed() const
{
#ifdef WIN32
    return (m_lastElapsed.QuadPart * (1000000.0 / m_frequency.QuadPart));
#else
    return ((m_lastElapsed.tv_sec * 1000000) + m_lastElapsed.tv_usec) / 1000000.0;
#endif
}

double Timer::getTotalElapsed() const
{
#ifdef WIN32
    return (m_totalElapsed.QuadPart * (1000000.0 / m_frequency.QuadPart));
#else
    return ((m_totalElapsed.tv_sec * 1000000.0) + m_totalElapsed.tv_usec) * 0.000001;
#endif
}

double Timer::getAverageElapsed() const
{
#ifdef WIN32
    return (m_averageElapsed.QuadPart * (1000000.0 / m_frequency.QuadPart));
#else
    return ((m_averageElapsed.tv_sec * 1000000.0) + m_averageElapsed.tv_usec) * 0.000001;
#endif
}

double Timer::getRunningTime() const
{
    if(m_started){
#ifdef WIN32
    LARGE_INTEGER actual;
    QueryPerformanceCounter(&actual);
    return (actual.QuadPart-m_start.QuadPart) * (1000000.0 / m_frequency.QuadPart);
#else
    timeval actual;
    gettimeofday(&actual, NULL);

    unsigned long long int startTime = m_start.tv_sec * 1000000 + m_start.tv_usec;
    unsigned long long int endTime = actual.tv_sec * 1000000 + actual.tv_usec;
    unsigned long long int diffTime = endTime - startTime;
    return diffTime/1000000;
#endif
    } else {
        return 0;
    }
}

double Timer::getCPULastElapsed() const
{
    return m_clockLastElapsed / (double) CLOCKS_PER_SEC;
}

double Timer::getCPUTotalElapsed() const
{
    return m_clockTotalElapsed / (double) CLOCKS_PER_SEC;
}

double Timer::getCPUAverageElapsed() const
{
    return m_clockAverageElapsed / (double) CLOCKS_PER_SEC;
}

double Timer::getCPURunningTime() const
{
    if(m_started){
        clock_t actual = clock();
        return (actual-m_clockStart) / (double) CLOCKS_PER_SEC;
    } else {
        return 0;
    }
}
