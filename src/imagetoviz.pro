QT += widgets

TARGET = image-to-viz
TEMPLATE = app
CONFIG += c++14
VERSION = 1.0.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += ../resources/imagetoviz.ico
ICON = ../resources/imagetoviz.icns

HEADERS       = aspectratiopixmaplabel.h \
                imagetoviz.h
                

SOURCES       = aspectratiopixmaplabel.cpp \
                imagetoviz.cpp \
                main.cpp

wince {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}

RESOURCES += resources.qrc
