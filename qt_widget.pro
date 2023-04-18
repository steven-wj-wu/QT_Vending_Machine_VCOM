QT       += core gui widgets serialport
QT       += network
QT       += multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    touch_function/getevent.cpp \
    touchthread.cpp \
    videothread.cpp \
    widget.cpp

HEADERS += \
    touch_function/getevent.h \
    touchthread.h \
    videothread.h \
    widget.h

FORMS += \
    widget.ui

INCLUDEPATH += /opt/qt5pi/sysroot/usr/include/
LIBS += -L"/opt/qt5pi/sysroot/usr/lib/"
LIBS += -lwiringPi

# Default rules for deployment.
qnx: target.path = /home/steven/$${TARGET}
else: unix:!android: target.path = /home/steven/$${TARGET}
!isEmpty(target.path): INSTALLS += target
