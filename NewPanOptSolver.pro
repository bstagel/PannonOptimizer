#=================================================================================================
#
#  This file is part of the Pannon Optimizer library.
#  This library is free software; you can redistribute it and/or modify it under the
#  terms of the GNU Lesser General Public License as published by the Free Software
#  Foundation; either version 3.0, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License; see the file
#  COPYING. If not, see http://www.gnu.org/licenses/.
#
#=================================================================================================

TEMPLATE = app

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = PannonOptimizer

LIBS += -pthread
#LIBS += -lgmpxx -lgmp
#LIBS += -rdynamic -ldl

CONFIG(release, debug|release) {
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../release/libPannonOptimizer.a
    LIBS += -L../release/ -lPannonOptimizer # -lqd
    OBJECTS_DIR = .o_release
}
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS_DEBUG += -g -O2 -std=c++11
    QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -lgcc_eh
    POST_TARGETDEPS += ../debug/libPannonOptimizer.a
    LIBS += -L../debug/ -lPannonOptimizer # -lqd
    OBJECTS_DIR = .o_debug
}

macx {
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
