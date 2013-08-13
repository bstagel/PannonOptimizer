TEMPLATE = lib

CONFIG += staticlib

QT -= gui
CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

CONFIG += console

TARGET = NewPanOpt

CONFIG(release) {
    message("Release mode!")
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -O2
}

#CONFIG(debug) {
#    message("debug mode!")
#    DEFINES = NDEBUG
#    QMAKE_CXXFLAGS_RELEASE = -O2
#}

#Includes
INCLUDEPATH += . \
               include/

#Input
HEADERS += include/debug.h \
           include/linalg/matrix.h \
           include/linalg/vector.h \
           include/linalg/vector_iterator.h \
           include/linalg/matrixbuilder.h \
           include/lp/model.h \
           include/lp/simplexmodel.h \
           include/lp/modelbuilder.h \
           include/lp/manualmodelbuilder.h \
           include/lp/variable.h \
           include/lp/constraint.h \
           include/lp/mpsproblem.h \
           include/simplex/simplex.h \
           include/simplex/primalsimplex.h \
           include/simplex/dualsimplex.h \
           include/utils/numerical.h \
           include/utils/elementproducer.h \
           include/utils/file.h \
           include/utils/heap.h \
           include/utils/indexlist.h \
           include/utils/list.h \
           include/utils/namestorer.h \
           include/utils/exceptions.h \
           include/globals.h

#Sources
SOURCES += src/linalg/matrix.cpp \
           src/linalg/vector.cpp \
           src/lp/model.cpp \
           src/lp/simplexmodel.cpp \
           src/lp/manualmodelbuilder.cpp \
           src/lp/variable.cpp \
           src/lp/constraint.cpp \
           src/lp/mpsproblem.cpp \
           src/simplex/simplex.cpp \
           src/simplex/primalsimplex.cpp \
           src/simplex/dualsimplex.cpp \
           src/utils/file.cpp \
           src/utils/indexlist.cpp \
           src/utils/numerical.cpp \
           src/debug.cpp \


OBJECTS_DIR = .o
