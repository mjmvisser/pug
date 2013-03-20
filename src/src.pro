TEMPLATE = lib
CONFIG += qt network plugin debug silent
QT += qml

TARGET = pugplugin

uri = Pug

VERSION = 1.0

# Add more folders to ship with the application, here
#folder_01.source = src/qml
#folder_01.target = src/qml
#DEPLOYMENTFOLDERS = folder_01

#LIBS += -L 3rdparty/qgetopts/lib -lqgetopts
#INCLUDEPATH += 3rdparty/qgetopts/include

INCLUDEPATH += views operations shotgun tractor

DEFINES += PUG_LIBRARY

HEADERS += Pug *.h views/*.h operations/*.h shotgun/*.h tractor/*.h
SOURCES += *.cpp views/*.cpp operations/*.cpp shotgun/*.cpp tractor/*.cpp

headers.path = ../include
headers.files = $$HEADERS

imports.path = ../imports
imports.files = qml/Pug
imports.commands = rsync -a --exclude=".svn" $$imports.files $$imports.path

QMAKE_EXTRA_TARGETS += imports
POST_TARGETDEPS += imports

INSTALLS += headers

DESTDIR = ../imports/Pug
OBJECTS_DIR = obj
MOC_DIR = moc

unix:QMAKE_RPATHDIR += /prod/tools/common/glib-2.3.4.0/lib /prod/tools/common/at-spi2-core-2.6.0
