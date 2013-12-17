TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = MarosTester

CONFIG(release, debug|release) {
    message("MarosTester: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
}
CONFIG(debug, debug|release) {
    message("MarosTester: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
}

#Includes
INCLUDEPATH += .

#Input

#Sources
SOURCES += standalones/marostester.cpp

OBJECTS_DIR = .o
