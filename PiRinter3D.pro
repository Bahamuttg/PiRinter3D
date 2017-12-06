#-------------------------------------------------
#
# Project created by QtCreator 2016-07-25T12:11:50
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiRinter3D
TEMPLATE = app
VERSION = 1.0.9
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
    heaterdriver.cpp \
    spindlemotor.cpp \
    Lib/SMTP/emailaddress.cpp \
    Lib/SMTP/mimeattachment.cpp \
    Lib/SMTP/mimecontentformatter.cpp \
    Lib/SMTP/mimefile.cpp \
    Lib/SMTP/mimehtml.cpp \
    Lib/SMTP/mimeinlinefile.cpp \
    Lib/SMTP/mimemessage.cpp \
    Lib/SMTP/mimemultipart.cpp \
    Lib/SMTP/mimepart.cpp \
    Lib/SMTP/mimetext.cpp \
    Lib/SMTP/quotedprintable.cpp \
    Lib/SMTP/smtpclient.cpp \
    fanworker.cpp

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
    heaterdriver.h \
    spindlemotor.h \
    Lib/SMTP/emailaddress.h \
    Lib/SMTP/mimeattachment.h \
    Lib/SMTP/mimecontentformatter.h \
    Lib/SMTP/mimefile.h \
    Lib/SMTP/mimehtml.h \
    Lib/SMTP/mimeinlinefile.h \
    Lib/SMTP/mimemessage.h \
    Lib/SMTP/mimemultipart.h \
    Lib/SMTP/mimepart.h \
    Lib/SMTP/mimetext.h \
    Lib/SMTP/quotedprintable.h \
    Lib/SMTP/smtpclient.h \
    Lib/SMTP/smtpexports.h \
    fanworker.h

FORMS    += mainwindow.ui \
    stepperdriver.ui \
    motorconfigdialog.ui \
    probeconfigdialog.ui \
    printareaconfigdialog.ui \
    heaterdriver.ui

INCLUDEPATH += /usr/include \


RESOURCES += \
    PiRinter.qrc
