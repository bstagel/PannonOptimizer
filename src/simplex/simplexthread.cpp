#include <simplex/simplexthread.h>
#include <simplex/simplexparameterhandler.h>

SimplexThread::SimplexThread(Simplex *currentSimplex)
    : m_exception(""),
      m_iterationNumber(0),
      m_objectiveValue(0),
      m_currentSimplex(currentSimplex),
      m_debugLevel(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.debug_level")),
      m_enableExport(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.Export.enable"))
{
//    m_iterationReport = new IterationReport();

//    m_iterationReport->addProviderForStart(*this);
//    m_iterationReport->addProviderForIteration(*this);
//    m_iterationReport->addProviderForSolution(*this);

//    if(m_enableExport){
//        m_iterationReport->addProviderForExport(*this);
//    }
}

SimplexThread::~SimplexThread()
{}

std::vector<IterationReportField> SimplexThread::getIterationReportFields(IterationReportProvider::ITERATION_REPORT_FIELD_TYPE &type) const
{
    std::vector<IterationReportField> result;
    __UNUSED(type);
    return result;
}

Entry SimplexThread::getIterationEntry(const string &name, IterationReportProvider::ITERATION_REPORT_FIELD_TYPE &type) const
{
    Entry reply;
    __UNUSED(type);
    __UNUSED(name);
    return reply;
}

void SimplexThread::performIterations(int iterationNumber)
{
    ThreadSupervisor::registerMyThread();
    m_result = ITERATE;
    int iterations = 0;
    for (; iterations < iterationNumber; iterations++) {
        try{
            //iterate
            m_currentSimplex->iterate();
            if(!m_currentSimplex->m_feasible){
                m_objectiveValue = m_currentSimplex->getPhaseIObjectiveValue();
            }else{
                m_objectiveValue = m_currentSimplex->getObjectiveValue();
            }

//            if(m_debugLevel > 1){
//                m_iterationReport->writeIterationReport();
//            }
        } catch ( const OptimalException & exception ) {
            m_result = COMPLETE;
            m_exception = OptimalException("Optimal solution");
        } catch ( const PrimalInfeasibleException & exception ) {
            m_result = COMPLETE;
            m_exception = PrimalInfeasibleException("Primal infeasible");
        } catch ( const DualInfeasibleException & exception ) {
            m_result = COMPLETE;
            m_exception = DualInfeasibleException("Dual infeasible");
        } catch ( const PrimalUnboundedException & exception ) {
            m_result = COMPLETE;
            m_exception = PrimalUnboundedException("Primal unbounded");
        } catch ( const DualUnboundedException & exception ) {
            m_result = COMPLETE;
            m_exception = DualUnboundedException("Dual unbounded");
        } catch ( const NumericalException & exception ) {
            m_result = TERMINATE;
            m_exception = NumericalException("Numerical problem");
            --iterations;
            break;
        } catch ( const FallbackException& exception) {
            m_result = TERMINATE;
            m_exception = FallbackException("Fallback detected");
            --iterations;
            break;
        }
    }
    m_iterationNumber = iterations;
    ThreadSupervisor::unregisterMyThread();
}
