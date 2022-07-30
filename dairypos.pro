#-------------------------------------------------
#
# Project created by QtCreator 2021-02-24T08:44:44
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dairypos
TEMPLATE = app
#LIBS +=-L ./lib/ -lgl11pos


SOURCES += main.cpp\
        dpos.cpp \
    analyzer_thread.cpp \
    read_analyzer.c \
    read_weighscale.c

HEADERS  += dpos.h \
    database.h \
     gl11.h \
    analyzer_thread.h \
    read_analyzer.h \
    read_weighscale.h \
    database_master.h

FORMS    += dpos.ui

RESOURCES += \
    thumbnails.qrc \
    abc.qrc \
    img.qrc \
    img.qrc
