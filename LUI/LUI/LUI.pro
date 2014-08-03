#-------------------------------------------------
#
# Project created by QtCreator 2014-08-01T11:06:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets\
                                        serialport

TARGET = LUI
TEMPLATE = app


SOURCES += main.cpp\
        lui.cpp \
    serial.cpp

HEADERS  += lui.h \
    serial.h

FORMS    += lui.ui
