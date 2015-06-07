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

#include <utils/thread.h>

std::mutex ThreadSupervisor::sm_mutex;

std::set<unsigned int> * ThreadSupervisor::sm_threadIds = nullptr;

thread_local unsigned int ThreadSupervisor::sm_myThreadId = 0;

unsigned int ThreadSupervisor::registerMyThread() {
    sm_mutex.lock();
    sm_myThreadId = 0;
    while (sm_threadIds->find(sm_myThreadId) != sm_threadIds->end()) {
        sm_myThreadId++;
    }
    sm_threadIds->insert(sm_myThreadId);
    sm_mutex.unlock();
    return sm_myThreadId;
}

void ThreadSupervisor::unregisterMyThread() {
    sm_mutex.lock();
    sm_threadIds->erase( sm_threadIds->find(sm_myThreadId) );
    sm_mutex.unlock();
}

void ThreadSupervisor::_globalInit()
{
    sm_threadIds = new std::set<unsigned int>;
}

void ThreadSupervisor::_globalRelease()
{
    delete sm_threadIds;
    sm_threadIds = nullptr;
}
