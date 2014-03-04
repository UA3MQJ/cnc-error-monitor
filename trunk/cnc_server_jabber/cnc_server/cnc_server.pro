include(../qxmpp-0.7.6/qxmpp.pri)

QT       += core network

QT       -= gui

QMAKE_LIBDIR += ../qxmpp-0.7.6/src
INCLUDEPATH += $$QXMPP_INCLUDEPATH
LIBS += $$QXMPP_LIBS

TARGET = cnc_server
DESTDIR = ../bin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cncreceiver.cpp jabberclient.cpp json.cpp

HEADERS += \
    cncreceiver.h jabberclient.h json.h
