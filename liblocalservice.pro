#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T14:04:43
#
#-------------------------------------------------

QT       += gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = liblocalservice
TEMPLATE = lib
CONFIG += staticlib

win32 {
DESTDIR = Z:/fastman2
}

unix {
DESTDIR = /media/ramdisk/fastman2
}

HEADER_SRC = $$PWD/*.h
HEADER_DEST = $$DESTDIR/
win32 {
HEADER_SRC ~= s,/,\\,g
HEADER_DEST ~= s,/,\\,g
}
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$HEADER_SRC) $$quote($$HEADER_DEST)

DEFINES += CMD_DEBUG
DEFINES -= UNICODE
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += "$$DESTDIR/"

SOURCES += \
    clocalserver.cpp \
    clocalclient.cpp \
    singleapplication.cpp

HEADERS += \
    clocalserver.h \
    clocalclient.h \
    singleapplication.h
