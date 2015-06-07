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
    InitPanOpt::init();

    SimplexController simplexController;
    simplexController.sequentialSolve(*model);

    ThreadSupervisor::unregisterMyThread();
}
