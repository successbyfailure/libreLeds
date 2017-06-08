#-------------------------------------------------
#
# Project created by QtCreator 2016-09-29T20:20:54
#
#-------------------------------------------------

QT       += core network serialport

INCLUDEPATH += ../common/

SOURCES += ../common/protocolclient.cpp \
    ../common/ledcontroller.cpp \
    ../common/ledcontrolerserver.cpp


HEADERS  += ../../Firmware/ledFirmware/protocol.h \
    ../../Firmware/ledFirmware/settings.h \
    ../common/protocolclient.h \
    ../common/ledcontroller.h \
    ../../Firmware/ledFirmware/qtcompat.h \
    ../common/ledcontrolerserver.h
