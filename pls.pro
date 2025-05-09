QT += core gui network widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG -= console
CONFIG += qt gui

SOURCES += \
    ModelARX.cpp \
    RegulatorPID.cpp \
    dialog_arx.cpp \
    dialog_sieciowy.cpp \
    main.cpp \
    mainwindow.cpp \
    symulator.cpp

HEADERS += \
    ModelARX.h \
    RegulatorPID.h \
    WartoscZadana.h \
    dialog_arx.h \
    dialog_sieciowy.h \
    mainwindow.h \
    symulator.h

FORMS += \
    dialog_arx.ui \
    dialog_sieciowy.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
