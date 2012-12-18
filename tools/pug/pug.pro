TEMPLATE = app
QT += qml #debug
#CONFIG += link_prl

TARGET = pug

# Add more folders to ship with the application, here
#folder_01.source = src/qml
#folder_01.target = src/qml
#DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

#LIBS += -L 3rdparty/qgetopts/lib -lqgetopts
#INCLUDEPATH += 3rdparty/qgetopts/include

SOURCES += \
    main.cpp

RESOURCES += pug.qrc

LIBS += -lpugplugin -L../../imports/Pug -Wl,-rpath -Wl,\\\$$ORIGIN/../imports/Pug

INCLUDEPATH += ../../src
DEPENDPATH += ../../src

DESTDIR = ../../bin
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc
#UI_DIR = build/ui
