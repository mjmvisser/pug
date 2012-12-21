#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>

#include "nodebase.h"

class Node : public NodeBase
{
    Q_OBJECT
public:
    explicit Node(QObject *parent = 0);

    Q_INVOKABLE const QString tempFile(const QString fileName) const;
    Q_INVOKABLE const QString tempFile(const QString baseName, const QString extension) const;
    Q_INVOKABLE const QString tempFile(const QString baseName, const QString frameSpec, const QString extension) const;
};
Q_DECLARE_METATYPE(Node*) // makes available to QVariant

#endif // NODE_H
