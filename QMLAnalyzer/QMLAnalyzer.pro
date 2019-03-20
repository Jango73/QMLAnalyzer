#-------------------------------------------------
#
# Project created by QtCreator 2016-01-18T09:04:28
#
#-------------------------------------------------

QT += core gui xml testlib qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QMLAnalyzer
TEMPLATE = app

CONFIG -= console

DESTDIR = ../bin
MOC_DIR = ../moc/QMLAnalyzer
OBJECTS_DIR = ../obj/QMLAnalyzer

QMAKE_CLEAN *= $$DESTDIR/*$$TARGET*
QMAKE_CLEAN *= $$MOC_DIR/*$$TARGET*
QMAKE_CLEAN *= $$OBJECTS_DIR/*$$TARGET*

INCLUDEPATH += $$PWD/cpp/Core
INCLUDEPATH += $$PWD/cpp/DataModel
INCLUDEPATH += $$PWD/cpp/DOMView
INCLUDEPATH += $$PWD/cpp/Processors
INCLUDEPATH += $$PWD/cpp/Views
INCLUDEPATH += $$PWD/cpp/Widgets

INCLUDEPATH += $$PWD/../qt-plus/source/cpp

include(QMLAnalyzer.pri)

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Generate help
helpfile = $$PWD/QMLAnalyzer.qdocconf

QMAKE_POST_LINK += qdoc $$quote($$shell_path($$helpfile)) $$escape_expand(\\n\\t)

# Copy the index.html to the help directory
copyfile = ./misc/index.html
copydest = ./help/index.html

QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)
