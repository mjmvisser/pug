#ifndef INPUT_H
#define INPUT_H

#include <QList>
#include <QQmlListProperty>

#include "pugitem.h"

class Node;

class Input : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Dependency dependency READ dependency WRITE setDependency NOTIFY dependencyChanged)
    Q_PROPERTY(QQmlListProperty<Node> nodes READ nodes_ NOTIFY nodesChanged)
    Q_ENUMS(Dependency)
public:
    enum Dependency { Invalid };
    explicit Input(QObject *parent = 0);
    
    Dependency dependency() const;
    void setDependency(Dependency);

    QQmlListProperty<Node> nodes_();

    const QList<const Node *> nodes() const;
    const QList<Node *> nodes();

signals:
    void dependencyChanged(Dependency dependency);
    void nodesChanged();
    
private:
    // nodes property
    static int nodes_count(QQmlListProperty<Node> *);
    static Node *node_at(QQmlListProperty<Node> *, int);

    Dependency m_dependency;
};

Q_DECLARE_METATYPE(Input*) // makes available to QVariant

#endif // INPUT_H
