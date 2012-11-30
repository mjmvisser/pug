#ifndef UTIL_H

#include <QObject>
#include <QString>

#include "element.h"

class Util : public QObject
{
    Q_OBJECT
public:
    Util(QObject *parent = 0);

    Q_INVOKABLE bool mkpath(const QString path);
    Q_INVOKABLE bool rmdir(const QString path);
    Q_INVOKABLE const QString readFile(const QString path);
    Q_INVOKABLE bool exists(const QString path);
    Q_INVOKABLE bool touch(const QString path);
    Q_INVOKABLE bool remove(const QString path);
    Q_INVOKABLE bool copy(const QString src, const QString dest);
    Q_INVOKABLE Element *createElement(QObject *parent, const QVariant data = QVariant());
};

#endif
