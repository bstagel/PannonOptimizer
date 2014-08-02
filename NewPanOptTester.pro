TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptTester

LIBS += -pthread

CONFIG(release, debug|release) {
    message("NewPanOptTester: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
    POST_TARGETDEPS += ../build-NewPanOpt/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/release/ -lNewPanOpt
    OBJECTS_DIR = .o_release

}
CONFIG(debug, debug|release) {
    message("NewPanOptTester: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
    POST_TARGETDEPS += ../build-NewPanOpt/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/debug/ -lNewPanOpt
    OBJECTS_DIR = .o_debug

}

#Includes
INCLUDEPATH += . \
               include/ \
               test/

#Input
HEADERS += test/framework/tester.h \
           test/framework/unittest.h \
           test/constrainttest.h \
           test/indexlisttest.h \
           test/manualmodelbuildertest.h \
           test/matrixtest.h \
           test/modeltest.h \
           test/mpsproblemtest.h \
           test/testmacros.h \
           test/variabletest.h \
           test/vectortest.h \
           test/timertest.h \
           test/linalgparameterhandlertest.h \
           test/simplexparameterhandlertest.h \
    test/outputhandlertest.h \ #\
    test/coretest.h
   # test/dualratiotesttest.h

#Sources
SOURCES += test/framework/tester.cpp \
           test/test.cpp \
           test/constrainttest.cpp \
           test/indexlisttest.cpp \
           test/manualmodelbuildertest.cpp \
           test/matrixtest.cpp \
           test/modeltest.cpp \
           test/mpsproblemtest.cpp \
           test/variabletest.cpp \
           test/vectortest.cpp \
           test/timertest.cpp \
           test/linalgparameterhandlertest.cpp \
           test/simplexparameterhandlertest.cpp \
    test/outputhandlertest.cpp \
    test/coretest.cpp
  #  test/dualratiotesttest.cpp

OBJECTS_DIR = .o
