#include <initpanopt.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/vector.h>
#include <debug.h>
#include <iostream>
#include <cstdio>
#include <utils/platform.h>

using namespace std;


// architectures
#ifdef PLATFORM_X86
#include <utils/arch/x86.h>
#else
#include <utils/arch/unknown.h>
#endif

//static const InitPanOpt & instance = InitPanOpt::getInstance();

InitPanOpt * InitPanOpt::sm_instance;
ArchitectureInterface * InitPanOpt::sm_architecture;

InitPanOpt::InitPanOpt() {

}

InitPanOpt::InitPanOpt(const InitPanOpt & orig) {
    __UNUSED(orig);
}

InitPanOpt &InitPanOpt::getInstance() {
    return *sm_instance;
}

void InitPanOpt::init() {
#ifndef CLASSIC_NEW_DELETE
    MemoryManager::sm_smallStacks = 0;

    MemoryManager::sm_largeStacks = 0;

    Pool::sm_head = 0;

    MemoryManager::sm_initialized = false;


    MemoryManager::init();
#endif
    D::init();

    sm_instance = new InitPanOpt();

    LinalgParameterHandler::_globalInit();
    SimplexParameterHandler::_globalInit();
    Numerical::_globalInit();
    Vector::_globalInit();

#ifdef PLATFORM_X86
    sm_architecture = new ArchitectureX86;
#else
    sm_architecture = new ArchitectureUnknown;
#endif
    sm_architecture->detect();

}

const ArchitectureInterface & InitPanOpt::getArchitecture() const {
    return *sm_architecture;
}

__attribute__((constructor))
void initPanOpt() {

    setbuf(stdout, 0);
    printf("INIT PanOpt!\n");

    InitPanOpt::init();

}

__attribute__((destructor))
static void releasePanOpt() {
    std::cout << "RELEASE PanOpt" << std::endl;

#ifndef CLASSIC_NEW_DELETE
    MemoryManager::release();
#endif
}
