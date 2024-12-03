TEMPLATE = app
TARGET = cnca3tests
CONFIG += console c++20
QT += core testlib
QT += network

SOURCES += $$PWD/MACAddressTests.cpp \
           $$PWD/IPTests.cpp

INCLUDEPATH += $$PWD/../src \
               $$PWD/../src/Globals

LIBS += -L$$PWD/../lib -lcnca3lib
