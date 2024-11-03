QT       += core gui quick qml multimedia multimediawidgets network quickcontrols2

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Stack protector
CXXFLAGS += -fstack-protector
QMAKE_LFLAGS += -fstack-protector

# Additional libraries for stack protection
LIBS += -lssp -lssp_nonshared

# You can make your code fail to compile if it uses deprecated APIs.
# Uncomment the following line to disable deprecated APIs before Qt 6.0.0
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Organized sources
SOURCES += \
    src/main.cpp \
    src/Audio/AudioInput.cpp \
    src/Audio/AudioOutput.cpp \
    src/Network/WebRTC.cpp \
    src/Network/SignalingServer.cpp \
    src/Network/SignalingClient.cpp \
    src/Network/CallManager.cpp

# Organized headers
HEADERS += \
    src/Audio/AudioInput.h \
    src/Audio/AudioOutput.h \
    src/Network/WebRTC.h \
    src/Network/SignalingServer.h \
    src/Network/SignalingClient.h \
    src/Network/CallManager.h

# QML file
QML += src/UI/Main.qml

# Resources
RESOURCES += qml.qrc

# Library and include paths
INCLUDEPATH += D:/UNIVERSITY/4031/CN/CAs/libdatachannel/include
LIBS += -LD:/UNIVERSITY/4031/CN/CAs/libdatachannel/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -lws2_32

INCLUDEPATH += D:/UNIVERSITY/4031/CN/CAs/opus/include
LIBS += -LD:/UNIVERSITY/4031/CN/CAs/opus/Windows/Mingw64 -lopus

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
