#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QQmlListProperty>

#include "node.h"

class Network : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Node> children READ children)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    explicit Network(QObject *parent = 0);

    QQmlListProperty<Node> children();
    int childCount() const;
    Node* child(int index) const;

signals:

public slots:

private:
    QList<Node *> m_children;
};
//Q_DECLARE_METATYPE(Network*) // makes available to QVariant

#endif // NETWORK_H
