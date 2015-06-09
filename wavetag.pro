#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T18:11:08
#
#-------------------------------------------------

CONFIG += c++11

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wavetag
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    editor.cpp

HEADERS  += mainwindow.h \
    editor.h

FORMS    += mainwindow.ui
