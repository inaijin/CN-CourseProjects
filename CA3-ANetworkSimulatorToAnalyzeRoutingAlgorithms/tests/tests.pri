QT     += core testlib
CONFIG += console no_keywords c++20

INCLUDEPATH += $$PWD/../src

# Test source files
SOURCES += \
    # MACAddressTests.cpp

LIBS += -L$$OUT_PWD/../src -lcnca3lib
