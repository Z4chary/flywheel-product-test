#-------------------------------------------------
#
# Project created by QtCreator 2019-09-03T09:41:26
#
#-------------------------------------------------

QT       += core gui printsupport axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 20mNflywheel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    csqldatabase.cpp \
    motor.cpp \
    motordriver.cpp \
    qcustomplot.cpp \
    qdaqcard.cpp \
    qmotorreport.cpp \
    qword.cpp \
    qcandriver.cpp \
    readexcel.cpp

HEADERS  += mainwindow.h \
    csqldatabase.h \
    D2kDask.h \
    motor.h \
    motordriver.h \
    qcustomplot.h \
    qdaqcard.h \
    qmotorreport.h \
    qword.h \
    qcandriver.h \
    readexcel.h

FORMS    += mainwindow.ui

QT += sql


LIBS += -L./ -lD2K-Dask

QT += serialport


RC_ICONS = favicon.ico


#icons 图标

RESOURCES += \
    picture.qrc
