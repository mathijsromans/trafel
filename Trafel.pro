#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T12:01:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trafel
TEMPLATE = app

exists( USE_CAMERA ) {
  message( "INFO: Configuring for use with camera!" )
  LIBS += -lraspicam
  DEFINES += USE_CAMERA
}

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
    spafel/cargo.cpp \
    spafel/environment.cpp \
    spafel/gravityscene.cpp \
    spafel/spaceship.cpp \
    button.cpp \
    clickableitem.cpp \
    spafel/planet.cpp \
    poker/pokerscene.cpp \
    flafel/flafelscene.cpp \
    pixmapitem.cpp \
    flafel/ball.cpp \
    flafel/stopper.cpp \
    flafel/player.cpp \
    mainmenuscene.cpp \
    rafel/rafelscene.cpp

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
    spafel/cargo.h \
    spafel/bodyitem.h \
    spafel/environment.h \
    spafel/gravityscene.h \
    spafel/spaceship.h \
    button.h \
    clickableitem.h \
    spafel/planet.h \
    poker/pokerscene.h \
    flafel/flafelscene.h \
    pixmapitem.h \
    flafel/ball.h \
    flafel/stopper.h \
    flafel/player.h \
    nocopy.h \
    mainmenuscene.h \
    rafel/rafelscene.h

FORMS    +=
