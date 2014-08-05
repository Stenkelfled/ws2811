#-------------------------------------------------
#
# Project created by QtCreator 2014-08-01T11:06:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets\
                                        serialport

CONFIG += c++11

TARGET = LUI
TEMPLATE = app


SOURCES += main.cpp\
        lui.cpp \
    serial.cpp \
    ledscene.cpp \
    leditem.cpp \
    groupitem.cpp \
    luiitem.cpp

HEADERS  += lui.h \
    serial.h \
    ../../protocoll.h \
    ledscene.h \
    ../../global.h \
    leditem.h \
    settings.h \
    groupitem.h \
    luiitem.h

INCLUDEPATH += ../../

DEPENPATH += ../../

FORMS    += lui.ui
