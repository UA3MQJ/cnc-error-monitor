QT       += core network sql
QT       -= gui

TARGET = cnc_client
DESTDIR = ./bin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += ./src/main.cpp \
    ./src/json.cpp \
    ./src/cncsender.cpp

HEADERS += \
    ./src/json.h \
    ./src/cncsender.h

OTHER_FILES += ./bin/config/database.json \
               ./bin/config/errorlist.ini
