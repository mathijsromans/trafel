#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T12:01:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trafel
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wall


SOURCES += main.cpp\
        mainwindow.cpp \
    tablescene.cpp \
    earth.cpp \
    distributions.cpp

HEADERS  += mainwindow.h \
    tablescene.h \
    earth.h \
    distributions.h

FORMS    +=
