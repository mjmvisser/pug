#ifndef OUTPUT_H
#define OUTPUT_H

#include "pugitem.h"

class Node;

class Output : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Node> nodes READ nodes_ NOTIFY nodesChanged)
public:
    explicit Output(QObject *parent = 0);

    QQmlListProperty<Node> nodes_();

    const QList<const Node *> nodes() const;
    const QList<Node *> nodes();

signals:
    void nodesChanged();
    
public slots:
    
private:
    // nodes property
    static int nodes_count(QQmlListProperty<Node> *);
    static Node *node_at(QQmlListProperty<Node> *, int);
};

Q_DECLARE_METATYPE(Output*) // makes available to QVariant

#endif // OUTPUT_H
