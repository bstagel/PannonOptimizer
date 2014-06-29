TEMPLATE = lib

CONFIG += staticlib

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

TARGET = NewPanOpt

LIBS += -pthread

CONFIG(release, debug|release) {
    message("NewPanOpt: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11 -g
    DESTDIR = ../build-NewPanOpt/release
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    message("NewPanOpt: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11 -Wextra -Wall
    DESTDIR = ../build-NewPanOpt/debug
    OBJECTS_DIR = .o_debug
}

#Includes
INCLUDEPATH += . \
               include/ \
               include/utils/thirdparty/

#Input
HEADERS += include/debug.h \
           include/linalg/matrix.h \
           include/linalg/vector.h \
           include/linalg/vector_iterator.h \
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
    include/simplex/dualpricingcontroller.h

#Sources
SOURCES += src/linalg/matrix.cpp \
           src/linalg/vector.cpp \
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
    src/simplex/dualpricingcontroller.cpp

#OBJECTS_DIR = .o
