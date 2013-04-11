#ifndef TRACTORBLOCK_H
#define TRACTORBLOCK_H

#include <QObject>
#include <QString>
#include <QSet>

class TractorBlock : public QObject
{
    Q_OBJECT
public:
    TractorBlock(QObject *parent = 0);

    Q_INVOKABLE virtual const QString asString(int indent, QSet<const TractorBlock *>& visited) const = 0;
};

#endif
