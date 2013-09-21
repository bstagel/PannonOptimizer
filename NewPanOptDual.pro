TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOptTester


CONFIG(release, debug|release) {
    message("NewPanOpt: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2
}
CONFIG(debug, debug|release) {
    message("NewPanOpt: Debug mode!")
}

#Includes
INCLUDEPATH += . \
               include/


#Sources
SOURCES += standalones/dual.cpp

POST_TARGETDEPS += ../build-NewPanOpt-Release/release/libNewPanOpt.a
LIBS += -L../build-NewPanOpt-Release/release/ -lNewPanOpt

OBJECTS_DIR = .o
