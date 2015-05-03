TEMPLATE = lib

CONFIG += staticlib

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

TARGET = NewPanOpt
#LIBS += -lgmpxx -lgmp
LIBS += -pthread

CONFIG(release, debug|release) {
    #message("NewPanOpt: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11 -mavx
    #DESTDIR = ../build-NewPanOpt/release
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    #message("NewPanOpt: Debug mode!")
    #DEFINES += NDEBUG
    QMAKE_CXXFLAGS_DEBUG += -g -O2 -std=c++11 -Wextra -Wall
    #DESTDIR = ../build-NewPanOpt/debug
    OBJECTS_DIR = .o_debug
}

linux-g++:contains(QMAKE_HOST.arch, x86_64):{
    #message(Linux 64 bit system)
    QMAKE_CXXFLAGS += -rdynamic
    QMAKE_EXTRA_COMPILERS += nasmproc64
    ASM_SRCS_64 += src/utils/arch/x86/cpuinfo64.asm \
                    src/utils/arch/x86/memcpy64.asm \
                    src/linalg/arch/x86/dotproduct64.asm \
                    src/linalg/arch/x86/addvector64.asm
    #message($${DESTDIR}../$${OBJECTS_DIR})
    nasmproc64.output = ${DESTDIR}/../${OBJECTS_DIR}${QMAKE_FILE_BASE}.o
    nasmproc64.commands = nasm -f elf64 ${QMAKE_FILE_NAME} -g3 -o ${QMAKE_FILE_OUT}
    nasmproc64.input = ASM_SRCS_64

}

linux-g++:contains(QMAKE_HOST.arch, x86_32):{
    #message(Linux 32 bit system)
    QMAKE_CXXFLAGS += -rdynamic
    QMAKE_EXTRA_COMPILERS += nasmproc32
    ASM_SRCS_32 += src/utils/arch/x86/cpuinfo32.asm \
        src/utils/arch/x86/memcpy32.asm
    #message($${DESTDIR}../$${OBJECTS_DIR})
    nasmproc32.output = ${DESTDIR}/../${OBJECTS_DIR}${QMAKE_FILE_BASE}.o
    nasmproc32.commands = nasm -f elf32 ${QMAKE_FILE_NAME} -g3 -o ${QMAKE_FILE_OUT}
    nasmproc32.input = ASM_SRCS_32

}

win32-g++:contains(QMAKE_HOST.arch, x86_64):{
    #message(Windows 64 bit system)
}

#win32-g++:contains(QMAKE_HOST.arch, x86_32):{
win32:contains(QMAKE_HOST.arch, x86): {
    #message(Windows 32 bit system)
    QMAKE_EXTRA_COMPILERS += nasmproc32
    ASM_SRCS_32 += src\utils\arch\x86\cpuinfo32.asm
        #src/utils/arch/x86/memcpy32.asm
    #message($${DESTDIR}../$${OBJECTS_DIR}/$${QMAKE_FILE_BASE}.o)
    #message($${OBJECTS_DIR})
    nasmproc32.output = $${DESTDIR}/../$${OBJECTS_DIR}/${QMAKE_FILE_BASE}.o
    #message($$nasmproc32.output)
    nasmproc32.commands = nasm -f win32 ${QMAKE_FILE_NAME} -g -o ${QMAKE_FILE_OUT}
    nasmproc32.input = ASM_SRCS_32
}

macx {
    #message("Ez egy Macintosh")
    QMAKE_CXX = clang++
    QMAKE_CC = clang
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}


#Includes
INCLUDEPATH += . \
               include/ \
               include/utils/thirdparty/

#Input
HEADERS += include/debug.h \
           include/linalg/matrix.h \
           include/linalg/matrixbuilder.h \
           include/linalg/linalgparameterhandler.h \
           include/lp/abstractmodel.h \
           include/lp/method.h \
           include/lp/model.h \
           include/lp/modelbuilder.h \
           include/lp/manualmodelbuilder.h \
           include/lp/variable.h \
           include/lp/constraint.h \
           include/lp/mpsproblem.h \
           include/lp/scaler.h \
           include/lp/panoptstate.h \
           include/lp/presolver.h \
           include/lp/presolvermodule.h \
           include/simplex/pricing/dualdantzigpricing.h \
           include/simplex/pricing/primaldantzigpricing.h \
           include/simplex/startingbasisfinder/sbf_super.h \
           include/simplex/startingbasisfinder/sbf_logical.h \
           include/simplex/dualpricing.h \
           include/simplex/primalpricing.h \
           include/simplex/simplexmodel.h \
           include/simplex/simplex.h \
           include/simplex/primalsimplex.h \
           include/simplex/dualsimplex.h \
           include/simplex/simplexparameterhandler.h \
           include/simplex/basis.h \
           include/simplex/pfibasis.h \
           include/simplex/startingbasisfinder.h \
           include/simplex/dualfeasibilitychecker.h \
           include/simplex/dualratiotest.h \
           include/simplex/primalratiotest.h \
           include/simplex/primalfeasibilitychecker.h \
           include/simplex/basisheadpanopt.h \
           include/simplex/checker.h \
           include/simplex/basisheadio.h \
           include/simplex/basisheadbas.h \
           include/utils/thirdparty/prettyprint.h \
           include/utils/arch/x86.h \
           include/utils/numerical.h \
           include/utils/elementproducer.h \
           include/utils/file.h \
           include/utils/heap.h \
           include/utils/indexlist.h \
           include/utils/list.h \
           include/utils/exceptions.h \
           include/utils/parameter.h \
           include/utils/timer.h \
           include/utils/parameterhandler.h \
           include/utils/outputhandler.h \
           include/utils/doublehistory.h \
           include/utils/flags.h \
           include/utils/refobject.h \
           include/utils/architecture.h \
           include/utils/memoryman.h \
           include/utils/colors.h \
           include/utils/platform.h \
           include/utils/hashtable.h \
           include/utils/thread.h \
           include/utils/iterationreport.h \
           include/utils/iterationreportprovider.h \
           include/utils/iterationreportfield.h \
           include/utils/entry.h \
           include/utils/sha1.h \
           include/utils/datetime.h \
           include/utils/system.h \
           include/utils/tokenizer.h \
           include/utils/nodefile.h \
           include/defaultparameters.h \
           include/globals.h \
    include/simplex/startingbasisfinder/sbf_crash.h \
    include/utils/breakpointhandler.h \
    include/simplex/pricing/simpri.h \
    include/simplex/pricing/dualdevexpricing.h \
    include/simplex/pricing/dualsteepestedgepricing.h \
    include/simplex/pricing.h \
    include/simplex/simplexcontroller.h \  
	include/simplex/lubasis.h \
    include/simplex/startingbasisfinder/sbf_structural.h \
    include/initpanopt.h \
    include/utils/primitives.h \
    include/macros.h \
    include/utils/arch/unknown.h \
    include/utils/architectureinterface.h \
    include/linalg/lpmatrix.h \
    include/linalg/indexeddensevector.h \
    include/linalg/densevector.h \
    include/utils/iterator.h \
    include/linalg/sparsevector.h \
    include/simplex/simplexparametercomments.h \
    include/utils/parameterfilebuilder.h \
    include/linalg/linalgparametercomments.h \
    include/utils/multiplefloat.h \
    include/utils/stacktrace.h \
    include/simplex/simplexthread.h \
    include/simplex/pricing/primalsteepestedgepricing.h \
    include/simplex/pricing/primaldevexpricing.h \
    include/lp/hilbertmodelbuilder.h \
    include/double.h \
    include/lp/pascalmodelbuilder.h \
    include/lp/pnsinterface.h \
    include/lp/lpproblem.h

#Sources
SOURCES += src/linalg/matrix.cpp \
           src/linalg/linalgparameterhandler.cpp \
           src/lp/model.cpp \
           src/lp/manualmodelbuilder.cpp \
           src/lp/variable.cpp \
           src/lp/constraint.cpp \
           src/lp/mpsproblem.cpp \
           src/lp/scaler.cpp \
           src/lp/presolver.cpp \
           src/lp/presolvermodule.cpp \
           src/simplex/pricing/dualdantzigpricing.cpp \
           src/simplex/pricing/primaldantzigpricing.cpp \
           src/simplex/startingbasisfinder/sbf_super.cpp \
           src/simplex/startingbasisfinder/sbf_logical.cpp \
           src/simplex/simplexmodel.cpp \
           src/simplex/simplex.cpp \
           src/simplex/primalsimplex.cpp \
           src/simplex/dualsimplex.cpp \
           src/simplex/simplexparameterhandler.cpp \
           src/simplex/basis.cpp \
           src/simplex/pfibasis.cpp \
           src/simplex/startingbasisfinder.cpp \
           src/simplex/dualfeasibilitychecker.cpp \
           src/simplex/dualratiotest.cpp \
           src/simplex/dualpricing.cpp \
           src/simplex/primalratiotest.cpp \
           src/simplex/primalfeasibilitychecker.cpp \
           src/simplex/basisheadbas.cpp \
           src/simplex/basisheadpanopt.cpp \
           src/simplex/checker.cpp \
           src/simplex/primalpricing.cpp \
           src/utils/arch/x86.cpp \
           src/utils/file.cpp \
           src/utils/numerical.cpp \
           src/utils/timer.cpp \
           src/utils/parameterhandler.cpp \
           src/utils/outputhandler.cpp \
           src/utils/memoryman.cpp \
           src/utils/hashtable.cpp \
           src/utils/iterationreport.cpp \
           src/utils/sha1.cpp \
           src/utils/datetime.cpp \
           src/utils/system.cpp \
           src/utils/tokenizer.cpp \
           src/utils/nodefile.cpp \
           src/utils/doublehistory.cpp \
           src/utils/flags.cpp \
           src/debug.cpp \
           src/defaultparameters.cpp \
           src/globals.cpp \
           src/simplex/startingbasisfinder/sbf_crash.cpp \
           src/utils/breakpointhandler.cpp \
    src/simplex/pricing/simpri.cpp \
    src/simplex/pricing/dualdevexpricing.cpp \
    src/simplex/pricing/dualsteepestedgepricing.cpp \
    src/simplex/simplexcontroller.cpp \
    src/simplex/lubasis.cpp \
    src/simplex/startingbasisfinder/sbf_structural.cpp \
    src/initpanopt.cpp \
    src/utils/architecture.cpp \
    src/utils/primitives.cpp \
    src/utils/arch/unknown.cpp \
    src/test.cpp \
    src/linalg/lpmatrix.cpp \
    src/linalg/indexeddensevector.cpp \
    src/linalg/densevector.cpp \
    src/linalg/sparsevector.cpp \
    src/utils/thread.cpp \
    src/lp/abstractmodel.cpp \
    src/utils/parameterfilebuilder.cpp \
    src/utils/stacktrace.cpp \
    src/simplex/simplexthread.cpp \
    src/simplex/pricing/primalsteepestedgepricing.cpp \
    src/simplex/pricing/primaldevexpricing.cpp \
    src/utils/multiplefloat.cpp \
    src/lp/hilbertmodelbuilder.cpp \
    src/lp/pascalmodelbuilder.cpp \
    src/lp/pnsinterface.cpp \
    src/lp/lpproblem.cpp

#OBJECTS_DIR = .o

OTHER_FILES += \
    src/linalg/arch/x86/dotproduct64.asm \
    src/utils/arch/x86/memcpy64.asm \
    src/utils/arch/x86/cpuinfo64.asm \
    src/utils/arch/x86/cpuinfo32.asm \
    src/linalg/arch/x86/addvector64.asm \
    src/linalg/arch/x86/tolerances.asm
