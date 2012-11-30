#ifndef PUGPLUGIN_H
#define PUGPLUGIN_H

#include <QQmlExtensionPlugin>

class PugPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mokkostudio.Pug")
public:
    virtual void registerTypes(const char *uri);
};

#endif
