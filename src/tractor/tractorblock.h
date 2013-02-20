#ifndef TRACTORBLOCK_H
#define TRACTORBLOCK_H

#include <QObject>
#include <QString>

class TractorBlock : public QObject
{
    Q_OBJECT
public:
    TractorBlock(QObject *parent = 0);

    Q_INVOKABLE virtual const QString asString(int indent = 0) const = 0;
};

#endif
