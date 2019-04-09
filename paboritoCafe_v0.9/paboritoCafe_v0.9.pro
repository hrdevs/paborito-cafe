#-------------------------------------------------
#
# Project created by QtCreator 2018-07-03T12:49:47
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paboritocafe_v0.9
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    confirmorderstatus.cpp \
    createaccount.cpp \
    edititem.cpp \
    loginwindow.cpp \
    newitem.cpp \
    kitchenview.cpp \
    editaccount.cpp \
    addvat.cpp \
    adddiscount.cpp \
    managerpass.cpp

HEADERS  += mainwindow.h \
    confirmorderstatus.h \
    createaccount.h \
    edititem.h \
    loginwindow.h \
    newitem.h \
    kitchenview.h \
    editaccount.h \
    addvat.h \
    adddiscount.h \
    managerpass.h

FORMS    += mainwindow.ui \
    confirmorderstatus.ui \
    createaccount.ui \
    edititem.ui \
    loginwindow.ui \
    newitem.ui \
    kitchenview.ui \
    editaccount.ui \
    addvat.ui \
    adddiscount.ui \
    managerpass.ui

RESOURCES += \
    pics/pics.qrc

DISTFILES += \
    pics/paborito Logo.jpg \
    pics/paborito Logo.jpg
