#-------------------------------------------------
#
# Project created by QtCreator 2017-04-02T23:44:01
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PluriNotes
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    note.cpp \
    tablemodel.cpp \
    delegue.cpp \
    versiondisplayer.cpp \
    relation.cpp \
    notewid.cpp \
    newnotedialog.cpp \
    trash.cpp \
    commands.cpp \
    newrelationdialog.cpp \
    relationtreeview.cpp \
    notesmanager.cpp \
    relationview.cpp \
    datamanager.cpp

HEADERS  += mainwindow.h \
    note.h \
    tablemodel.h \
    delegue.h \
    notewid.h \
    newnotedialog.h \
    relation.h \
    versiondisplayer.h \
    trash.h \
    commands.h \
    newrelationdialog.h \
    iterator.h \
    relationtreeview.h \
    datamanager.h \
    notesmanager.h \
    relationview.h

RESOURCES += \
    res.qrc

FORMS += \
    addrelationdialog.ui \
    addcoupledialog.ui
