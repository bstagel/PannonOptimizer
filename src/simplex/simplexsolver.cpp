#include <simplex/simplexsolver.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexcontroller.h>
#include <simplex/simplexthread.h>
#include <utils/thread.h>
#include <thread>
#include <vector>

void SimplexSolver::solve(const Model &model)
{
    bool parallel = SimplexParameterHandler::getInstance().getBoolParameterValue("enable_parallelization");
    if (parallel) {
        bool joco = true;
        if(!joco){
            SimplexController simplexController;
            simplexController.parallelSolve(model);
        }else{
//            unsigned int threadIndex;
            std::cout << " ---------------- ";
            std::thread t1( &SimplexSolver::parallelSolve, SimplexSolver(), &model );
            std::thread t2( &SimplexSolver::parallelSolve, SimplexSolver(), &model );
            t1.join();
            t2.join();
        }
    } else {
        SimplexController simplexController;
        simplexController.sequentialSolve(model);
    }
}

void SimplexSolver::parallelSolve(const Model * model)
{
#ifndef CLASSIC_NEW_DELETE
    MemoryManager::startParallel();
#endif
    ThreadSupervisor::registerMyThread();

    SimplexController simplexController;
    simplexController.sequentialSolve(*model);

    ThreadSupervisor::unregisterMyThread();
}
