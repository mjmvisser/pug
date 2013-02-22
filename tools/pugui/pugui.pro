TEMPLATE = app
QT += qml quick #debug
#CONFIG += link_prl

TARGET = pugui

# Add more folders to ship with the application, here
#folder_01.source = src/qml
#folder_01.target = src/qml
#DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

#LIBS += -L 3rdparty/qgetopts/lib -lqgetopts
#INCLUDEPATH += 3rdparty/qgetopts/include

SOURCES += *.cpp
HEADERS += *.h

RESOURCES += pugui.qrc

LIBS += -lpugplugin -L../../imports/Pug -Wl,-rpath -Wl,\\\$$ORIGIN/../imports/Pug

INCLUDEPATH += ../../src
DEPENDPATH += ../../src

DESTDIR = ../../bin
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc
#UI_DIR = build/ui

imports.path = ../../imports
imports.files = qml/PugUI
imports.commands = rsync -a --exclude=".svn" $$imports.files $$imports.path

QMAKE_EXTRA_TARGETS += imports
POST_TARGETDEPS += imports
