#ifndef INITPANOPT_H
#define INITPANOPT_H

#include <utils/architectureinterface.h>

class InitPanOpt {
public:
    static InitPanOpt & getInstance();

    static void init();

    static void release();

    const ArchitectureInterface & getArchitecture() const;
private:
    static ArchitectureInterface * sm_architecture;

    static InitPanOpt * sm_instance;

    InitPanOpt();
    InitPanOpt(const InitPanOpt & orig);
};

__attribute__((constructor))
void initPanOpt();

#endif // INITPANOPT_H
