TEMPLATE = app

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NewPanOptGUI

CONFIG(release, debug|release) {
    #message("NewPanOptGUI: Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../build-NewPanOpt/release/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/release/ -lNewPanOpt -L../panopt/lib/# -lqd
    DESTDIR = ../build-NewPanOptGUI/release
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    #message("NewPanOptGUI: Debug mode!")
    QMAKE_CXXFLAGS_DEBUG += -g -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../build-NewPanOpt/debug/libNewPanOpt.a
    LIBS += -L../build-NewPanOpt/debug/ -lNewPanOpt -L../panopt/lib/# -lqd
    DESTDIR = ../build-NewPanOptGUI/debug
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

INCLUDEPATH += . \
               include/ \

SOURCES += 		gui/main.cpp\
				gui/core/mainwindow.cpp \
				gui/constants.cpp \
    gui/modules/panopthandler.cpp

HEADERS  += 	gui/core/mainwindow.h \
				gui/core/constants.h \
    gui/modules/panopthandler.h

FORMS    +=

RESOURCES += 	\
				gui/resources/resources.qrc
