#ifndef INITPANOPT_H
#define INITPANOPT_H

#include <utils/architecture.h>

class InitPanOpt {
public:
    static InitPanOpt & getInstance();

    static void init();

    const Architecture & getArchitecture() const;
private:
    static Architecture * sm_architecture;

    static InitPanOpt * sm_instance;

    InitPanOpt();
    InitPanOpt(const InitPanOpt & orig);
};

__attribute__((constructor))
void initPanOpt();

#endif // INITPANOPT_H
