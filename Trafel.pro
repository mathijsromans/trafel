#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T12:01:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trafel
TEMPLATE = app

LIBS += -lraspicam

QMAKE_CXXFLAGS += -std=c++1y -Wall -g
CONFIG += console

SOURCES += main.cpp\
        mainwindow.cpp \
    trafficscene.cpp \
    earth.cpp \
    distributions.cpp \
    userinput.cpp \
    transformscene.cpp \
    image.cpp \
    scorekeeper.cpp \
    pointerevent.cpp \
    userinputthread.cpp

HEADERS  += mainwindow.h \
    trafficscene.h \
    earth.h \
    distributions.h \
    userinput.h \
    transformscene.h \
    image.h \
    scorekeeper.h \
    pointerevent.h \
    userinputthread.h

FORMS    +=
