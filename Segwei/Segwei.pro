#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T23:42:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Segwei
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    codeeditor.cpp \
    linenumber.cpp \
    scannar.cpp \
    error.cpp \
    parser.cpp \
    codegenerator.cpp

HEADERS  += gui.h \
    codeeditor.h \
    linenumber.h \
    scannar.h \
    error.h \
    parser.h \
    codegenerator.h

FORMS    += gui.ui \
    codegenerator.ui

RESOURCES += \
    Icons.qrc
