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
    tablescene.cpp \
    earth.cpp \
    distributions.cpp \
    userinput.cpp \
    transformscene.cpp \
    image.cpp \
    scorekeeper.cpp \
    pointerevent.cpp

HEADERS  += mainwindow.h \
    tablescene.h \
    earth.h \
    distributions.h \
    userinput.h \
    transformscene.h \
    image.h \
    scorekeeper.h \
    pointerevent.h

FORMS    +=
