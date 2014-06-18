
QT       += core

QT       -= gui

TARGET = untitled
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    memoryTrainer.cpp \
    ../../common/CogMem/memsegment.cpp \
    ../../common/CogMem/ANN.cpp

HEADERS += \
    ../../common/CogMem/memsegment.h \
    ../../common/CogMem/memregister.h \
    ../../common/CogMem/ANN.h \
    memoryTrainer.h
