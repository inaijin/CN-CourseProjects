TEMPLATE = lib
TARGET = cnca3lib
CONFIG += staticlib c++20
QT += core
QT += network

DESTDIR = $$PWD/../lib

INCLUDEPATH += $$PWD/Globals

SOURCES += \
    $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/IP/IP.cpp \
    $$PWD/PortBindingManager/PortBindingManager.cpp \
    $$PWD/Port/Port.cpp \
    $$PWD/MACAddress/MACAddress.cpp \
    $$PWD/MACAddress/MACAddressGenerator.cpp \
    $$PWD/DataGenerator/DataGenerator.cpp \
    $$PWD/Packet/Packet.cpp \
    $$PWD/Header/DataLinkHeader.cpp \
    $$PWD/Header/TCPHeader.cpp \
    $$PWD/NetworkSimulator/Simulator.cpp \
    $$PWD/NetworkSimulator/Network.cpp \
    $$PWD/Network/AutonomousSystem.cpp \
    $$PWD/Network/Router.cpp \
    $$PWD/Network/PC.cpp

HEADERS += \
    $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/Globals/Globals.h \
    $$PWD/IP/IP.h \
    $$PWD/PortBindingManager/PortBindingManager.h \
    $$PWD/Port/Port.h \
    $$PWD/MACAddress/MACAddress.h \
    $$PWD/MACAddress/MACAddressGenerator.h \
    $$PWD/DataGenerator/DataGenerator.h \
    $$PWD/Packet/Packet.h \
    $$PWD/Header/DataLinkHeader.h \
    $$PWD/Header/TCPHeader.h \
    $$PWD/NetworkSimulator/Simulator.h \
    $$PWD/NetworkSimulator/Network.h \
    $$PWD/Network/AutonomousSystem.h \
    $$PWD/Network/Router.h \
    $$PWD/Network/PC.h
