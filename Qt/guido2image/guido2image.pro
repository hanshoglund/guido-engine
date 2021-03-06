SOURCES += main.cpp

win32 {
	SOURCES += XGetopt.cpp
	HEADERS += XGetopt.h
}
else {
	DEFINES += USE_UNISTD
}

TEMPLATE = app
win32 {
	TEMPLATE = vcapp
}
CONFIG += console
macx:CONFIG -= app_bundle
DESTDIR = ../bin

# SVG support
# QT += svg

# GuidoQt library link for each platform
win32:LIBS += ../GuidoQt/GuidoQt.lib
unix:LIBS += -L../GuidoQt -lGuidoQt
INCLUDEPATH += ../GuidoQt/include

include( ../GUIDOEngineLink.pri )
