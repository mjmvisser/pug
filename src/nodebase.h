#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>
#include <QJSValue>

#include "pugitem.h"
#include "param.h"
#include "input.h"
#include "output.h"
#include "operation.h"

class NodeBase : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Param> params READ params_ NOTIFY paramsChanged)
    Q_PROPERTY(QQmlListProperty<Input> inputs READ inputs_ NOTIFY inputsChanged)
    Q_PROPERTY(QQmlListProperty<Output> outputs READ outputs_ NOTIFY outputsChanged)
    Q_PROPERTY(QQmlListProperty<NodeBase> children READ nodes_ NOTIFY nodesChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QJSValue details READ details NOTIFY detailsChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
public:
    explicit NodeBase(QObject *parent = 0);

    QQmlListProperty<Input> inputs_();
    QQmlListProperty<Output> outputs_();
    QQmlListProperty<Param> params_();
    QQmlListProperty<NodeBase> nodes_();

    bool isActive() const;
    void setActive(bool o);

    Q_INVOKABLE NodeBase* node(const QString node);
    const NodeBase* node(const QString node) const;

    const QList<NodeBase *> upstream();
    const QList<const NodeBase *> upstream() const;

    const QList<NodeBase *> downstream();
    const QList<const NodeBase *> downstream() const;

    // TODO: not sure if this is the best way
    virtual bool isRoot() const;

    int count() const;
    void setCount(int);

    int index() const;
    void setIndex(int);

    QJSValue details();
    const QJSValue details() const;

    const NodeBase *rootBranch() const;
    NodeBase *rootBranch();

signals:
    void paramsChanged();
    void nodesChanged();
    void inputsChanged();
    void outputsChanged();
    void activeChanged(bool active);
    void detailsChanged();
    void countChanged(int index);
    void indexChanged(int index);

protected:
    void addParam(const QString name);
    void addInput(const QString name);
    void addOutput(const QString name);

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
    static NodeBase *node_at(QQmlListProperty<NodeBase> *, int);
    static void nodes_clear(QQmlListProperty<NodeBase> *);

    // params property
    static void params_append(QQmlListProperty<Param> *, Param *);
    static int params_count(QQmlListProperty<Param> *);
    static Param *param_at(QQmlListProperty<Param> *, int);
    static void params_clear(QQmlListProperty<Param> *);

    // inputs property
    static void inputs_append(QQmlListProperty<Input> *, Input *);
    static int inputs_count(QQmlListProperty<Input> *);
    static Input *input_at(QQmlListProperty<Input> *, int);
    static void inputs_clear(QQmlListProperty<Input> *);

    // outputs property
    static void outputs_append(QQmlListProperty<Output> *, Output *);
    static int outputs_count(QQmlListProperty<Output> *);
    static Output *output_at(QQmlListProperty<Output> *, int);
    static void outputs_clear(QQmlListProperty<Output> *);

    QList<Param *> m_params;
    QList<Input *> m_inputs;
    QList<Output *> m_outputs;
    bool m_activeFlag;
    QJSValue m_details;
    int m_count;
    int m_index;
};

Q_DECLARE_METATYPE(NodeBase*) // makes available to QVariant

#endif // NODEBASE_H
