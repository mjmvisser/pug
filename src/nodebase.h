#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>

#include "pugitem.h"
#include "propertybase.h"
#include "operation.h"

class NodeBase : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PropertyBase> properties READ properties_ NOTIFY propertiesChanged)
    Q_PROPERTY(QQmlListProperty<NodeBase> children READ nodes_ NOTIFY nodesChanged)
    Q_PROPERTY(QQmlListProperty<NodeBase> inputs READ inputs_ NOTIFY inputsChanged)
    Q_PROPERTY(QQmlListProperty<Operation> operations READ operations_ NOTIFY operationsChanged)
    Q_PROPERTY(bool output READ isOutput WRITE setOutput NOTIFY outputChanged)
public:
    explicit NodeBase(QObject *parent = 0);

    QQmlListProperty<NodeBase> nodes_();
    QQmlListProperty<PropertyBase> properties_();
    QQmlListProperty<NodeBase> inputs_();
    QQmlListProperty<Operation> operations_();

    bool isOutput() const;
    void setOutput(bool o);

    Q_INVOKABLE NodeBase* node(const QString node);
    const NodeBase* node(const QString node) const;

    const QList<NodeBase *> inputs(); // TODO: rename to upstream?
    const QList<const NodeBase *> inputs() const;

    const QList<NodeBase *> downstream();
    const QList<const NodeBase *> downstream() const;

    // TODO: not sure if this is the best way
    virtual bool isRoot() const;

    const NodeBase *rootBranch() const;
    NodeBase *rootBranch();

signals:
    void propertiesChanged();
    void nodesChanged();
    void inputsChanged();
    void operationsChanged();
    void outputChanged(bool output);

private:
    const NodeBase *nodeInChildren(const QString n) const;
    NodeBase *nodeInChildren(const QString n);

    NodeBase *firstNamedParent();
    const NodeBase *firstNamedParent() const;

    const NodeBase *nodeInFirstNamedParent(const QString n) const;
    NodeBase *nodeInFirstNamedParent(const QString n);

    // children property
    static void nodes_append(QQmlListProperty<NodeBase> *, NodeBase *);
    static int nodes_count(QQmlListProperty<NodeBase> *);
    static NodeBase *nodes_at(QQmlListProperty<NodeBase> *, int);
    static void nodes_clear(QQmlListProperty<NodeBase> *);

    // properties property
    static void properties_append(QQmlListProperty<PropertyBase> *, PropertyBase *);
    static int properties_count(QQmlListProperty<PropertyBase> *);
    static PropertyBase *properties_at(QQmlListProperty<PropertyBase> *, int);
    static void properties_clear(QQmlListProperty<PropertyBase> *);

    // inputs property
    static void inputs_append(QQmlListProperty<NodeBase> *, NodeBase *);
    static int inputs_count(QQmlListProperty<NodeBase> *);
    static NodeBase *inputs_at(QQmlListProperty<NodeBase> *, int);
    static void inputs_clear(QQmlListProperty<NodeBase> *);

    // operations property
    static void operations_append(QQmlListProperty<Operation> *, Operation *);
    static int operations_count(QQmlListProperty<Operation> *);
    static Operation *operations_at(QQmlListProperty<Operation> *, int);
    static void operations_clear(QQmlListProperty<Operation> *);

    bool m_outputFlag;
};

Q_DECLARE_METATYPE(NodeBase*) // makes available to QVariant

#endif // NODEBASE_H
