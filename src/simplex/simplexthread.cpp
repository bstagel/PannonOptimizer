//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#include <simplex/simplexthread.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/checker.h>

SimplexThread::SimplexThread(Simplex *currentSimplex)
    : m_result(SimplexThread::FINISHED),
      m_exception(""),
      m_exceptionType(SimplexThread::OPTIMAL),
      m_iterationNumber(0),
      m_currentSimplex(currentSimplex),
      m_saveBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.SaveBasis.basis")),
      m_debugLevel(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.debug_level"))
{}

SimplexThread::~SimplexThread()
{}

void SimplexThread::performIterations(Basis* basis, IterationReport* iterationReport, int mainIterations, int iterationNumber)
{
    ThreadSupervisor::registerMyThread();
    InitPanOpt::threadInit();
    basis->registerThread();
    basis->setSimplexState(m_currentSimplex);

    m_currentSimplex->computeFeasibility();

    m_result = ITERATED;
    for (m_iterationNumber = mainIterations;
         m_iterationNumber < mainIterations + iterationNumber;) {
        try{
            //iterate
            if(m_saveBasis){
                m_currentSimplex->saveBasis(m_iterationNumber, ThreadSupervisor::getThreadId());
            }

            m_currentSimplex->iterate(m_iterationNumber);

            ++m_iterationNumber;

            if(m_debugLevel>1 || (m_debugLevel==1 && m_iterationNumber-1 == mainIterations)){
                iterationReport->writeIterationReport();
            }
//            LPINFO("m_iterationNumber "<<m_iterationNumber<<" - "<<ThreadSupervisor::getThreadId());


        } catch ( const OptimalException & exception ) {
            m_result = FINISHED;
            m_exception = exception;
            m_exceptionType = SimplexThread::OPTIMAL;
//            LPINFO("OPTIMAL "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const PrimalInfeasibleException & exception ) {
            m_result = FINISHED;
            m_exception = exception;
            m_exceptionType = SimplexThread::PRIMAL_INFEASIBLE;
//            LPINFO("PRIMAL_INFEASIBLE "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const DualInfeasibleException & exception ) {
            m_result = FINISHED;
            m_exception = exception;
            m_exceptionType = SimplexThread::DUAL_INFEASIBLE;
//            LPINFO("DUAL_INFEASIBLE "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const PrimalUnboundedException & exception ) {
            m_result = FINISHED;
            m_exception = exception;
            m_exceptionType = SimplexThread::PRIMAL_UNBOUNDED;
//            LPINFO("PRIMAL_UNBOUNDED "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const DualUnboundedException & exception ) {
            m_result = FINISHED;
            m_exception = exception;
            m_exceptionType = SimplexThread::DUAL_UNBOUNDED;
//            LPINFO("DUAL_UNBOUNDED "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const NumericalException & exception ) {
            m_result = TERMINATED;
            m_exception = exception;
            m_exceptionType = SimplexThread::NUMERICAL;
//            LPINFO("NUMERICAL "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const FallbackException& exception) {
            m_result = TERMINATED;
            m_exception = exception;
            m_exceptionType = SimplexThread::FALLBACK;
            LPINFO("FALLBACK "<<ThreadSupervisor::getThreadId());
            break;
        } catch ( const PanOptException& exception) {
            LPINFO("Something wrong at thread: "<<ThreadSupervisor::getThreadId());
            LPINFO(exception.getMessage());LPINFO("Something wrong at thread: "<<ThreadSupervisor::getThreadId());

            break;
        }/* catch (...){
            LPINFO("Something wrong at thread: "<<ThreadSupervisor::getThreadId());
            break;
        }*/
    }
    basis->releaseThread();
    InitPanOpt::threadRelease();
    ThreadSupervisor::unregisterMyThread();
}
