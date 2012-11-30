#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QQmlListProperty>

#include "node.h"

class Network : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Node> children READ children)
    Q_PROPERTY(Node *parent READ parent WRITE setParent)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    explicit Network(QObject *parent = 0);

    QQmlListProperty<Node> children();
    int childCount() const;
    Node* child(int index) const;

    Node* parent();
    void setParent(Node* n);

    Q_INVOKABLE Node* findNode(const QString n) const;

signals:

public slots:

private:
    QList<Node *> m_children;
};
//Q_DECLARE_METATYPE(Network*) // makes available to QVariant

#endif // NETWORK_H
