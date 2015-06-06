#include <simplex/numericalmonitor.h>

bool NumericalMonitor::m_needPrimaryDetector = false;

void NumericalMonitor::trackNumericalError()
{
    m_needPrimaryDetector = true;
}

bool NumericalMonitor::runPrimaryDetector()
{
    return m_needPrimaryDetector;
}

void NumericalMonitor::setPrimaryDetector(bool enable)
{
    m_needPrimaryDetector = enable;
}
