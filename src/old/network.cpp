#include <QDebug>
#include "network.h"

Network::Network(QObject *parent) :
    QObject(parent)
{
}

QQmlListProperty<Node> Network::children()
{
    return QQmlListProperty<Node>(this, m_children);
}


int Network::childCount() const
{
    return m_children.count();
}

Node *Network::child(int index) const
{
    return m_children.at(index);
}

Node *Network::parent()
{
    return qobject_cast<Node*>(QObject::parent());
}

void Network::setParent(Node *n)
{
    QObject::setParent(n);
}

Node* Network::findNode(const QString n) const
{
    // networks are not part of the naming hierarchy
    foreach (Node *node, m_children) {
        Node* child = node->findNode(n);
        if (child)
            return child;
    }

    return 0;
}
