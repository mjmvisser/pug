TEMPLATE = app
TARGET = tst_operations
CONFIG += qmltestcase warn_on debug
macx:CONFIG -= app_bundle
SOURCES += main.cpp
IMPORTPATH += ../../imports

unix:QMAKE_RPATHDIR += /prod/tools/common/glib-2.3.4.0/lib /prod/tools/common/at-spi2-core-2.6.0

