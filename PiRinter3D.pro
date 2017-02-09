#-------------------------------------------------
#
# Project created by QtCreator 2016-07-25T12:11:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiRinter3D
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    steppermotor.cpp \
    stepperdriver.cpp \
    motorconfigdialog.cpp \
    gcodeinterpreter.cpp \
    motorcontroller.cpp \
    thermalprobe.cpp \
    probeworker.cpp \
    probeconfigdialog.cpp \
    printareaconfigdialog.cpp \
    Lib/pigpio.c \
    Lib/command.c \
    adccontroller.cpp \
    fan.cpp \
    tooltrigger.cpp \
    heaterdriver.cpp

HEADERS  += mainwindow.h \
    motorworker.h \
    steppermotor.h \
    stepperdriver.h \
    motorconfigdialog.h \
    gcodeinterpreter.h \
    motorcontroller.h \
    thermalprobe.h \
    probeworker.h \
    probeconfigdialog.h \
    printareaconfigdialog.h \
    Lib/pigpio.h \
    Lib/command.h \
    Lib/custom.cext \
    adccontroller.h \
    fan.h \
    tooltrigger.h \
    heaterdriver.h

FORMS    += mainwindow.ui \
    stepperdriver.ui \
    motorconfigdialog.ui \
    probeconfigdialog.ui \
    printareaconfigdialog.ui \
    heaterdriver.ui

INCLUDEPATH += /usr/include


RESOURCES += \
    PiRinter.qrc
