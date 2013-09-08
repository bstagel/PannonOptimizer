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
           include/lp/abstractmodel.h \
           include/lp/method.h \
           include/lp/model.h \
           include/lp/modelbuilder.h \
           include/lp/manualmodelbuilder.h \
           include/lp/variable.h \
           include/lp/constraint.h \
           include/lp/mpsproblem.h \
           include/simplex/pricing/dualdantzigpricing.h \
           include/simplex/pricing/dualdantzigpricingfactory.h \
           include/simplex/pricing/primaldantzigpricing.h \
           include/simplex/pricing/primaldantzigpricingfactory.h \
           include/simplex/dualpricing.h \
           include/simplex/dualpricingfactory.h \
           include/simplex/dualupdater.h \
           include/simplex/primalpricing.h \
           include/simplex/primalpricingfactory.h \
           include/simplex/primalreducedcostupdater.h \
           include/simplex/primalupdater.h \
           include/simplex/simplexmodel.h \
           include/simplex/simplex.h \
           include/simplex/primalsimplex.h \
           include/simplex/dualsimplex.h \
           include/utils/numerical.h \
           include/utils/elementproducer.h \
           include/utils/file.h \
           include/utils/heap.h \
           include/utils/indexlist.h \
           include/utils/list.h \
           include/utils/exceptions.h \
           include/globals.h \
    include/simplex/dualpricingupdater.h \
    include/simplex/pricing/primaldantzigpricingupdater.h \
    include/simplex/pricing/dualdantzigpricingupdater.h

#Sources
SOURCES += src/linalg/matrix.cpp \
           src/linalg/vector.cpp \
           src/lp/model.cpp \
           src/lp/manualmodelbuilder.cpp \
           src/lp/variable.cpp \
           src/lp/constraint.cpp \
           src/lp/mpsproblem.cpp \
           src/simplex/pricing/dualdantzigpricing.cpp \
           src/simplex/pricing/dualdantzigpricingfactory.cpp \
           src/simplex/pricing/primaldantzigpricing.cpp \
           src/simplex/pricing/primaldantzigpricingfactory.cpp \
           src/simplex/pricing/primaldantzigreducedcostupdater.cpp \
           src/simplex/simplexmodel.cpp \
           src/simplex/simplex.cpp \
           src/simplex/primalsimplex.cpp \
           src/simplex/dualsimplex.cpp \
           src/utils/file.cpp \
           src/utils/indexlist.cpp \
           src/utils/numerical.cpp \
           src/debug.cpp \
    src/simplex/pricing/dualdantzigpricingupdater.cpp \
    src/simplex/dualupdater.cpp


OBJECTS_DIR = .o
