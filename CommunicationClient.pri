QT += core network serialport

TEMPLATE = lib
DEFINES += COMMUNICATIONCLIENT_LIBRARY

## convert to debug mode
#DEFINES += LIBRARY_UNEXPORT
contains(DEFINES, LIBRARY_UNEXPORT) {
    TEMPLATE = app
    include($$PWD/TestDemoCode/TestDemoCode.pri)
}

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/Communication/Communication.pri)
include($$PWD/CommunicationConf/CommunicationConf.pri)
include($$PWD/QCommonFunction/QCommonFunction.pri)

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/CommunicationClient.cpp \
    $$PWD/CommunicationException.cpp

HEADERS += \
    $$PWD/CommunicationClient.h \
    $$PWD/CommunicationClientDefine.h \
    $$PWD/CommunicationClientExport.h \
    $$PWD/CommunicationException.h

DESTDIR = $$USERDATA_INSTALL_DIR

win32: {
}

unix: {
    QMAKE_RPATHDIR += ./
}
