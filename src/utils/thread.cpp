#include <utils/thread.h>

std::mutex ThreadSupervisor::sm_mutex;

std::set<unsigned int> * ThreadSupervisor::sm_threadIds;

thread_local unsigned int ThreadSupervisor::sm_myThreadId;

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
    delete [] sm_threadIds;
    sm_threadIds = nullptr;
}
