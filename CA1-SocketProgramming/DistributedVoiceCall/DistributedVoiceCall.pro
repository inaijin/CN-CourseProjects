QT       += core gui
QT       += quick qml
QT       += core multimedia
QT       += core multimedia multimediawidgets
QT       += network
QT       += quickcontrols2


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Stack protector
CXXFLAGS += -fstack-protector
QMAKE_LFLAGS += -fstack-protector

# Additional libraries for stack protection
LIBS += -lssp -lssp_nonshared

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    webrtc.cpp \
    AudioOutput.cpp \
    AudioInput.cpp

HEADERS += \
    webrtc.h \
    AudioOutput.h \
    AudioInput.h

INCLUDEPATH += "PATHTOFILE"/include
LIBS += -L"PATHTOFILE"/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -lws2_32

INCLUDEPATH += "PATHTOFILE"/opus/include
LIBS += -L"PATHTOFILE"/opus/Windows/Mingw64 -lopus

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
