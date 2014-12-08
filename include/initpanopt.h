#ifndef INITPANOPT_H
#define INITPANOPT_H

#include <utils/architectureinterface.h>

//class ArchitectureInterface;

class InitPanOpt {
public:
    static InitPanOpt & getInstance();

    static void init();

    static void threadInit();

    static void release();

    static void threadRelease();

    const ArchitectureInterface & getArchitecture() const;
private:
    static thread_local ArchitectureInterface * sm_architecture;

    static thread_local InitPanOpt * sm_instance;

    InitPanOpt();
    InitPanOpt(const InitPanOpt & orig);
};

__attribute__((constructor))
void initPanOpt();

#endif // INITPANOPT_H
