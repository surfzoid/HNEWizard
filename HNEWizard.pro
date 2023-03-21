VERSION = 1.0.3
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
OUTPUT += Console
# Specifies name of the binary.
TARGET = HikNetExtractor

# Denotes that project is an application.
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
unix {
    isEmpty(PREFIX) {
        #PREFIX = $$(HOME)/.local/share
        PREFIX = /usr/share
    }
    translationfiles.path = $$(PREFIX)/HikNetExtractor/translations
    translationfiles.files += $$PWD/*.qm
    }

SOURCES += \
    main.cpp \
    wizard.cpp

HEADERS += \
    wizard.h

FORMS += \
    wizard.ui

TRANSLATIONS += \
    HNEWizard_fr_FR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $$(PREFIX)/../bin
!isEmpty(target.path): INSTALLS += target translationfiles

DISTFILES += \
    images/QtVsPlayer.icns \
    images/QtVsPlayer.png
