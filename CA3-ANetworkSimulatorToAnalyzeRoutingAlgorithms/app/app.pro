TEMPLATE = app
TARGET = cnca3app
CONFIG += console c++20
QT += core

SOURCES += $$PWD/main.cpp

INCLUDEPATH += $$PWD/../src \
               $$PWD/../src/Globals

LIBS += -L$$PWD/../lib -lcnca3lib
