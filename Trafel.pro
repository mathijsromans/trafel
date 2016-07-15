#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T12:01:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trafel
TEMPLATE = app

#LIBS += -lraspicam

QMAKE_CXXFLAGS += -std=c++1y -Wall -g
CONFIG += console

SOURCES += main.cpp\
    mainwindow.cpp \
    trafficscene.cpp \
    earth.cpp \
    userinput.cpp \
    transformscene.cpp \
    image.cpp \
    scorekeeper.cpp \
    pointerevent.cpp \
    userinputthread.cpp \
    mouseping.cpp \
    utilities.cpp \
    calibration.cpp \
    spafel/body.cpp \
    spafel/bodyitem.cpp \
    spafel/environment.cpp \
    spafel/gravityscene.cpp \
    button.cpp \
    clickableitem.cpp

HEADERS  += mainwindow.h \
    trafficscene.h \
    earth.h \
    userinput.h \
    transformscene.h \
    image.h \
    scorekeeper.h \
    pointerevent.h \
    userinputthread.h \
    mouseping.h \
    utilities.h \
    calibration.h \
    spafel/body.h \
    spafel/bodyitem.h \
    spafel/environment.h \
    spafel/gravityscene.h \
    button.h \
    clickableitem.h

FORMS    +=
