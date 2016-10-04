TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui
LIBS += -L/usr/lib/nptl -lpthread -o MyPlayer

SOURCES += \
    VideoPlayer.cpp \
    player.cpp \
    reader.cpp

HEADERS += \
    VideoPlayer.h

