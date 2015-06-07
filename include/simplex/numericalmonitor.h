#ifndef NUMERICALMONITOR
#define NUMERICALMONITOR



class NumericalMonitor {
public:
    static void trackNumericalError();
    static bool runPrimaryDetector();
    static void setPrimaryDetector(bool enable);
private:
    static bool m_needPrimaryDetector;
};

#endif // NUMERICALMONITOR

