TEMPLATE = app
TARGET = tst_tools
CONFIG += qmltestcase warn_on debug
macx:CONFIG -= app_bundle
SOURCES += main.cpp
IMPORTPATH += ../../imports ../../qml

unix:QMAKE_RPATHDIR += /prod/tools/common/glib-2.34.0/lib /prod/tools/common/at-spi2-core-2.6.0
