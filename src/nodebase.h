#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>

#include "pugitem.h"
#include "param.h"
#include "input.h"
#include "operation.h"

class NodeBase : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Param> params READ params_ NOTIFY paramsChanged)
    Q_PROPERTY(QQmlListProperty<Input> inputs READ inputs_ NOTIFY inputsChanged)
    Q_PROPERTY(QQmlListProperty<NodeBase> children READ nodes_ NOTIFY nodesChanged)
    Q_PROPERTY(bool output READ isOutput WRITE setOutput NOTIFY outputChanged)
    Q_PROPERTY(QVariantList details READ details WRITE setDetails NOTIFY detailsChanged)
public:
    explicit NodeBase(QObject *parent = 0);

    QQmlListProperty<Input> inputs_();
    QQmlListProperty<Param> params_();
    QQmlListProperty<NodeBase> nodes_();

    bool isOutput() const;
    void setOutput(bool o);

    Q_INVOKABLE NodeBase* node(const QString node);
    const NodeBase* node(const QString node) const;

    const QList<NodeBase *> upstream();
    const QList<const NodeBase *> upstream() const;

    const QList<NodeBase *> downstream();
    const QList<const NodeBase *> downstream() const;

    // TODO: not sure if this is the best way
    virtual bool isRoot() const;

    const QVariantList details() const;
    void setDetails(const QVariantList);
    void addDetail(const QVariantMap detail, bool notify=true);
    void setDetail(int index, const QVariantMap detail, bool notify=true);
    void clearDetails(bool notify=true);

    const NodeBase *rootBranch() const;
    NodeBase *rootBranch();

signals:
    void paramsChanged();
    void nodesChanged();
    void inputsChanged();
    void outputChanged(bool output);
    void detailsChanged();

protected:
    void addInput(const QString name);
    void addParam(const QString name);

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

    QList<Param *> m_params;
    QList<Input *> m_inputs;
    bool m_outputFlag;
    QVariantList m_details;
};

Q_DECLARE_METATYPE(NodeBase*) // makes available to QVariant

#endif // NODEBASE_H
