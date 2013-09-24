TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptTester

CONFIG(release, debug|release) {
    message("NewPanOptTester: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    POST_TARGETDEPS += ../build-NewPanOpt-Release/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt-Release/release/ -lNewPanOpt
}
CONFIG(debug, debug|release) {
    message("NewPanOptTester: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    POST_TARGETDEPS += ../build-NewPanOpt-Debug/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt-Debug/debug/ -lNewPanOpt
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
           test/simplexparameterhandlertest.h
#           test/outputhandlertest.h

#Sources
SOURCES += test/framework/tester.cpp \
           test/constrainttest.cpp \
           test/indexlisttest.cpp \
           test/manualmodelbuildertest.cpp \
           test/matrixtest.cpp \
           test/modeltest.cpp \
           test/mpsproblemtest.cpp \
           test/variabletest.cpp \
           test/vectortest.cpp \
           test/test.cpp \
           test/timertest.cpp \
           test/linalgparameterhandlertest.cpp \
           test/simplexparameterhandlertest.cpp
#           test/outputhandlertest.cpp

OBJECTS_DIR = .o
