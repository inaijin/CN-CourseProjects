QT     += core
CONFIG += lrelease
CONFIG += no_keywords
CONFIG += c++20
CONFIG += no_keywords c++20 staticlib
TEMPLATE = lib

INCLUDEPATH += $$PWD\Globals

SOURCES += \
    $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/IP/IP.cpp \
    $$PWD/PortBindingManager/PortBindingManager.cpp \
    $$PWD/Port/Port.cpp \
    $$PWD/MACAddress/MACAddress.cpp \
    $$PWD/MACAddress/MACAddressGenerator.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/Globals/Globals.h \
    $$PWD/IP/IP.h \
    $$PWD/PortBindingManager/PortBindingManager.h \
    $$PWD/Port/Port.h \
    $$PWD/MACAddress/MACAddress.h \
    $$PWD/MACAddress/MACAddressGenerator.h

TARGET = cnca3lib
