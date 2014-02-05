TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptParameterAnalyzer

CONFIG(release, debug|release) {
    message("NewPanOptParameterAnalyzer: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
    POST_TARGETDEPS += ../build-NewPanOpt/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/release/ -lNewPanOpt
}
CONFIG(debug, debug|release) {
    message("NewPanOptParameterAnalyzer: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++
    POST_TARGETDEPS += ../build-NewPanOpt/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/debug/ -lNewPanOpt
}

#Includes
INCLUDEPATH += . \
               include/


#Sources
SOURCES += \
    standalones/parameteranalyzer.cpp


OBJECTS_DIR = .o
