/**
 * @file thread.h This file contains macros defining the parallel processing settings.
 * @author Jozsef Smidla
 */

#ifndef THREAD_H
#define THREAD_H

#include <globals.h>
#include <set>
#include <thread>
#include <mutex>

/**
 * This class manages the identifiers of the running threads.
 */
class ThreadSupervisor {
    friend class InitPanOpt;
public:

    /**
     * Returns the id of the current thread.
     *
     * @return The current thread id.
     */
    ALWAYS_INLINE static unsigned int getThreadId() {
        return sm_myThreadId;
    }

    /**
     * Registers a new thread and creates an id.
     *
     * @return The id of the current thread.
     */
    static unsigned int registerMyThread();

    /**
     * Removes the id of the current thread.
     */
    static void unregisterMyThread();

private:

    /**
     * Mutex for protecting the variable sm_threadIds.
     */
    static std::mutex sm_mutex;

    /**
     * Stores the id's of the running threads.
     */
    static std::set<unsigned int> * sm_threadIds;

    /**
     * The id of the current thread.
     */
    static thread_local unsigned int sm_myThreadId;

    /**
     * Initializes the necessary data structures.
     */
    static void _globalInit();

    /**
     * Releases the necessary data structures.
     */
    static void _globalRelease();
};

#endif // THREAD_H
