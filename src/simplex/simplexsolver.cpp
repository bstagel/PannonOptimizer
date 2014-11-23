#include <simplex/simplexsolver.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/simplexcontroller.h>
#include <utils/thread.h>
#include <thread>
#include <vector>

void SimplexSolver::solve(const Model &model)
{
    bool parallel = SimplexParameterHandler::getInstance().getBoolParameterValue("enable_parallelization");
    if (parallel) {
        std::cout << " ---------------- ";
//        unsigned int threadIndex;
        std::thread t1( &SimplexSolver::parallelSolve, SimplexSolver(), &model );
        std::thread t2( &SimplexSolver::parallelSolve, SimplexSolver(), &model );
        t1.join();
        t2.join();
    } else {
        SimplexController simplexController;
        simplexController.solve(model);
    }
}

void SimplexSolver::parallelSolve(const Model * model)
{
    ThreadSupervisor::registerMyThread();

    SimplexController simplexController;
    simplexController.solve(*model);

    ThreadSupervisor::unregisterMyThread();
}
