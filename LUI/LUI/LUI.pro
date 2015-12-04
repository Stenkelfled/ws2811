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
    luiitem.cpp \
    luiview.cpp \
    luiwidgets.cpp \
    sequencescene.cpp \
    ledgroupitem.cpp \
    sequencegroupitem.cpp \
    sequenceitem.cpp \
    sequencecursoritem.cpp \
    utils.cpp \

HEADERS  += settings.h \
    utils.h \
    serial.h \
    lui.h \
    ../../protocoll.h \
    ledscene.h \
    ../../global.h \
    luiitem.h \
    leditem.h \
    luiview.h \
    luiwidgets.h \
    sequencescene.h \
    ledgroupitem.h \
    sequenceitem.h \
    sequencegroupitem.h \
    sequencecursoritem.h

INCLUDEPATH += ../../

DEPENPATH += ../../

FORMS    +=

RESOURCES += \
    res.qrc
