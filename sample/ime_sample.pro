#-------------------------------------------------
#
# Project created by QtCreator 2019-09-09T10:06:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ime_sample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../ime/keyboard.cpp \
    ../ime/inputcontext.cpp

HEADERS  += mainwindow.h \
    ../ime/keyboard.h \
    ../ime/inputcontext.h

FORMS    += mainwindow.ui

RESOURCES += \
    ../ime/res/imeres.qrc
