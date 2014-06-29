TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = GBACFramework

LIBS += -pthread

CONFIG(release, debug|release) {
    message("GBACFramework: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    message("GBACFramework: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
    OBJECTS_DIR = .o_debug
}

#Includes
INCLUDEPATH += .

#Headers
HEADERS += \
    tokenizer.h \
    exceptions.h \
    prettyprint.h

#Sources
SOURCES += \
    gbacgenerator.cpp \
    tokenizer.cpp


#OBJECTS_DIR = .o
