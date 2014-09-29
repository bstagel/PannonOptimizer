#include <initpanopt.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/vector.h>
#include <linalg/indexeddensevector.h>
#include <linalg/densevector.h>
#include <linalg/sparsevector.h>
#include <debug.h>
#include <iostream>
#include <cstdio>
#include <utils/platform.h>
#include <utils/thread.h>

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

    ThreadSupervisor::_globalInit();
    SimplexParameterHandler::_globalInit();
    LinalgParameterHandler::_globalInit();
    Numerical::_globalInit();
    Vector::_globalInit();
    IndexedDenseVector::_globalInit();
    DenseVector::_globalInit();
    SparseVector::_globalInit();

#ifdef PLATFORM_X86
    sm_architecture = new ArchitectureX86;
#else
    sm_architecture = new ArchitectureUnknown;
#endif
    sm_architecture->loadParameters();
    sm_architecture->detect();

}

void InitPanOpt::release()
{
    ThreadSupervisor::_globalRelease();

    SparseVector::_globalRelease();

    D::release();

    LinalgParameterHandler::_globalRelease();
    SimplexParameterHandler::_globalRelease();

    delete sm_instance;
    sm_instance = nullptr;

    delete sm_architecture;
    sm_architecture = nullptr;

#ifndef CLASSIC_NEW_DELETE
    MemoryManager::release();
#endif
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

    InitPanOpt::release();
}
