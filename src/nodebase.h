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
    Q_PROPERTY(QJSValue details READ details WRITE setDetails NOTIFY detailsChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
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

    Q_INVOKABLE const QString path() const;

    const QList<NodeBase *> upstream();
    Q_INVOKABLE const QVariantList upstreamNodes();
    const QList<const NodeBase *> upstream() const;

    const QList<NodeBase *> downstream();
    Q_INVOKABLE const QVariantList downstreamNodes();
    const QList<const NodeBase *> downstream() const;

    Q_INVOKABLE bool isUpstream(const NodeBase *other);
    Q_INVOKABLE bool isDownstream(const NodeBase *other);

    // TODO: not sure if this is the best way
    virtual bool isRoot() const;

    int count() const;
    void setCount(int);

    int index() const;
    void setIndex(int);

    QJSValue details();
    const QJSValue details() const;
    void setDetails(const QJSValue &);
    void clearDetails();

    const NodeBase *rootBranch() const;
    NodeBase *rootBranch();

    Q_INVOKABLE QJSValue detail(int index, const QString arg1=QString(), const QString arg2=QString(),
            const QString arg3=QString(), const QString arg4=QString(), const QString arg5=QString()) const;

    void setDetail(int, QJSValue, bool=true);
    void setDetail(int, const QString, QJSValue, bool=true);
    void setDetail(int, const QString, const QString, QJSValue, bool=true);
    void setDetail(int, const QString, const QString, const QString, QJSValue, bool=true);
    void setDetail(int, const QString, const QString, const QString, const QString, QJSValue, bool=true);
    void setDetail(int, const QString, const QString, const QString, const QString, const QString, QJSValue, bool=true);

    Q_INVOKABLE int childIndex() const;
    NodeBase *child(int index);
    int childCount() const;

    qreal x() const;
    void setX(qreal);
    qreal y() const;
    void setY(qreal);

signals:
    void paramsChanged();
    void nodesChanged();
    void inputsChanged();
    void outputsChanged();
    void activeChanged(bool active);
    void detailsChanged();
    void countChanged(int count);
    void indexChanged(int index);
    void xChanged(qreal x);
    void yChanged(qreal y);

protected:
    void addParam(const QString name);
    void addInput(const QString name);
    void addOutput(const QString name);

    virtual void componentComplete();

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
    qreal m_x, m_y; // should this be QVector2D? does it matter?
//    QList<NodeBase *> m_extraDependencies;
};

Q_DECLARE_METATYPE(NodeBase*) // makes available to QVariant

#endif // NODEBASE_H
