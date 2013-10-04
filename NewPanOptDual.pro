TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptDual

CONFIG(release, debug|release) {
    message("NewPanOptDual: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    POST_TARGETDEPS += ../build-NewPanOpt-Release/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt-Release/release/ -lNewPanOpt
}
CONFIG(debug, debug|release) {
    message("NewPanOptDual: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    POST_TARGETDEPS += ../build-NewPanOpt-Debug/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt-Debug/debug/ -lNewPanOpt
}

#Includes
INCLUDEPATH += . \
               include/


#Sources
SOURCES += standalones/dual.cpp


OBJECTS_DIR = .o
