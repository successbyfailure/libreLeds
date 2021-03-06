#-------------------------------------------------
#
# Project created by QtCreator 2016-09-29T20:20:54
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LedRemoteControl
TEMPLATE = app
INCLUDEPATH += ../common/

include(../common/common.pri)

SOURCES += main.cpp\
        mainviewwindow.cpp \
    sensorswidget.cpp \
    connectwidget.cpp \
    gsimplegraph.cpp \
    gwidgettxrx.cpp \
    editbasicsettingsgroupbox.cpp \
    simpleprotocolcommandsgroupbox.cpp \
    editextrasettingsgroupbox.cpp \
    animationselectgroupbox.cpp \
    animationframe.cpp \
    spectrumwidget.cpp \
    gblinkwidget.cpp \
    ledgadgetgroupbox.cpp \
    lampwidget.cpp \
    ledgadgetservergb.cpp \
    ledgadgetclientwidget.cpp \


HEADERS  += mainviewwindow.h \
    sensorswidget.h \
    connectwidget.h \
    gsimplegraph.h \
    gwidgettxrx.h \
    editbasicsettingsgroupbox.h \
    simpleprotocolcommandsgroupbox.h \
    editextrasettingsgroupbox.h \
    animationselectgroupbox.h \
    animationframe.h \
    spectrumwidget.h \
    gblinkwidget.h \
    ledgadgetgroupbox.h \
    lampwidget.h \
    ledgadgetservergb.h \
    ledgadgetclientwidget.h \


FORMS    += mainviewwindow.ui \
    sensorswidget.ui \
    connectwidget.ui \
    gsimplegraph.ui \
    editbasicsettingsgroupbox.ui \
    simpleprotocolcommandsgroupbox.ui \
    editextrasettingsgroupbox.ui \
    animationselectgroupbox.ui \
    animationframe.ui \
    spectrumwidget.ui \
    gblinkwidget.ui \
    ledgadgetgroupbox.ui \
    lampwidget.ui \
    ledgadgetservergb.ui \
    ledgadgetclientwidget.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    TestForm.ui.qml \
    Test.qml \

