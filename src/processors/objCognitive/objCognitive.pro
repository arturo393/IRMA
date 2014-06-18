#Base

SOURCES += objCognitive.cpp \
    memorythread.cpp \
    hitreg.cpp
HEADERS += objCognitive.h \
            ../../base/processor.h \
    memorythread.h \
    ../../common/CogMem/parameters.h \
    hitreg.h

#Common
SOURCES += \
    ../../common/CogMem/memsegment.cpp \
    ../../common/CogMem/ANN.cpp

HEADERS += \
    ../../common/CogMem/memsegment.h \
    ../../common/CogMem/memregister.h \
    ../../common/CogMem/ANN.h \


# OPENCV
INCLUDEPATH += /usr/include/opencv/

LIBS += -L \
    /usr/lib/opencv/ \
    -lcv \
    -lhighgui


#QT
QT       += core

QT       -= gui

TARGET = untitled
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
