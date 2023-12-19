QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    data.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp

HEADERS += \
    data.h \
    mainwindow.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

DISTFILES += \
    images/air-conditioner.png \
    images/air_conditioner.ico \
    images/application-exit.png \
    images/clear.png \
    images/connect.png \
    images/disconnect.png \
    images/icons8-recording.gif \
    images/icons8-replay.gif \
    images/settings.png
