TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptSolver

LIBS += -pthread
#linux specific...
LIBS += -lgmpxx -lgmp
#LIBS += -rdynamic -ldl

CONFIG(release, debug|release) {
    #message("NewPanOptSolver: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../build-NewPanOpt/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/release/ -lNewPanOpt -L../panopt/lib/# -lqd
    DESTDIR = ../build-NewPanOptSolver/release
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    #message("NewPanOptSolver: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../build-NewPanOpt/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/debug/ -lNewPanOpt -L../panopt/lib/# -lqd
    DESTDIR = ../build-NewPanOptSolver/debug
    OBJECTS_DIR = .o_debug
}

macx {
    #message("Ez egy Macintosh")
    QMAKE_CXX = clang++
    QMAKE_CC = clang
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_LFLAGS -= -static-libgcc -static-libstdc++ -lgcc_eh
    LIBS -= -pthread -L../panopt/lib/
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    CONFIG -= app_bundle
}

#Includes
INCLUDEPATH += . \
               include/ \
               include/utils/thirdparty


#Sources
SOURCES += \
    standalones/solver.cpp


#OBJECTS_DIR = .o
