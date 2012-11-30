TEMPLATE = lib
CONFIG += console plugin no_plugin_name_prefix
QT += qml

TARGET = pug
#TARGET_x =
#TARGET_x.y.z =
#QMAKE_PREFIX_SHLIB =

SOURCES += \
    pugapi.cpp

LIBS += -lpugplugin -L../../imports/Pug -Wl,-rpath -Wl,\\\$$ORIGIN/../imports/Pug
LIBS += -lboost_python

INCLUDEPATH += ../../src /usr/include/python2.6
DEPENDPATH += ../../src

DESTDIR = ../../python
OBJECTS_DIR = obj
MOC_DIR = moc
#RCC_DIR = rcc
#UI_DIR = build/ui
