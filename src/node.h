#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>
#include <QJSValue>
#include <QList>

#include "pugitem.h"
#include "param.h"
#include "operation.h"
#include "input.h"
#include "output.h"
#include "field.h"

class Node : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(DependencyOrder dependencyOrder READ dependencyOrder WRITE setDependencyOrder NOTIFY dependencyOrderChanged)
    Q_PROPERTY(QQmlListProperty<Field> fields READ fields_ NOTIFY fieldsChanged)
    Q_PROPERTY(QQmlListProperty<Param> params READ params_ NOTIFY paramsChanged)
    Q_PROPERTY(QQmlListProperty<Input> inputs READ inputs_ NOTIFY inputsChanged)
    Q_PROPERTY(QQmlListProperty<Output> outputs READ outputs_ NOTIFY outputsChanged)
    Q_PROPERTY(QQmlListProperty<Node> children READ nodes_ NOTIFY nodesChanged)
    Q_PROPERTY(QJSValue details READ details WRITE setDetails NOTIFY detailsChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_ENUMS(Status DependencyOrder)
public:
    enum DependencyOrder { InputsChildrenSelf, InputsSelfChildren };
    enum Status { Invalid, UpToDate, OutOfDate, Missing };

    explicit Node(QObject *parent = 0);

    DependencyOrder dependencyOrder() const;
    void setDependencyOrder(DependencyOrder);

    QQmlListProperty<Field> fields_();
    QQmlListProperty<Input> inputs_();
    QQmlListProperty<Output> outputs_();
    QQmlListProperty<Param> params_();
    QQmlListProperty<Node> nodes_();

    const QList<const Input*> inputs() const;
    const QList<Input*> inputs();
    const QList<const Output*> outputs() const;
    const QList<Output*> outputs();

    Field *findField(const QString name);
    const Field *findField(const QString name) const;
    //const QList<const Field *> fields(const QStringList fieldNameList = QStringList()) const;

    bool fieldsComplete(const QString pattern, const QVariantMap context) const;
    bool containsFields(const QVariantMap& needle, const QVariantMap& haystack) const;

    Q_INVOKABLE const QVariant match(const QString pattern, const QString path, bool exact, bool partial) const;
    Q_INVOKABLE const QString format(const QString pattern, const QVariant data) const;

    Q_INVOKABLE Node *node(const QString node);
    const Node *node(const QString node) const;

//    const QList<const Node *> upstream() const;
//    const QList<Node *> upstream();
//    Q_INVOKABLE const QVariantList upstreamNodes();
//
//    const QList<const Node *> upstream(const Input *) const;
//    const QList<Node *> upstream(const Input *);
//
//    const QList<const Node *> downstream() const;
//    const QList<Node *> downstream();
//    Q_INVOKABLE const QVariantList downstreamNodes();
//
//    Q_INVOKABLE bool isUpstream(const Node *other);
//    Q_INVOKABLE bool isDownstream(const Node *other);

    int count() const;
    void setCount(int);

    int index() const;
    void setIndex(int);

    QJSValue details();
    const QJSValue details() const;
    void setDetails(const QJSValue &);
    void clearDetails();

    const Node *rootBranch() const;
    Node *rootBranch();

    Q_INVOKABLE void setDetail(int, QJSValue, bool=true);
    Q_INVOKABLE void setDetail(int, const QString, QJSValue, bool=true);
    Q_INVOKABLE void setDetail(int, const QString, const QString, QJSValue, bool=true);
    Q_INVOKABLE void setDetail(int, const QString, const QString, const QString, QJSValue, bool=true);
    Q_INVOKABLE void setDetail(int, const QString, const QString, quint32, QJSValue, bool=true);
    Q_INVOKABLE void setDetail(int, const QString, const QString, quint32, const QString, QJSValue, bool=true);

    Q_INVOKABLE void setContext(int index, const QVariantMap context, bool emitChanged=true);

    Q_INVOKABLE int childIndex() const;
    Node *child(int index);
    int childCount() const;

    static const QVariantMap mergeContexts(const QVariantMap first, const QVariantMap second);

    qreal x() const;
    void setX(qreal);
    qreal y() const;
    void setY(qreal);

    Param *addParam(QObject *parent, const QString name);
    Input *addInput(QObject *parent, const QString name);

    int receivers(const char *signal) const;

    void printStatus(const QString indent = "") const;

    bool cycleCheck(const QList<const Node *>& visited = QList<const Node *>()) const;

    template <typename T>
    QJSValue toScriptValue(T value) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->toScriptValue(value);
    }

    QJSValue newArray(uint length = 0) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newArray(length);
    }

    QJSValue newObject() const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newObject();
    }

    QJSValue newQObject(QObject *object) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newQObject(object);
    }

signals:
    void dependencyOrderChanged(DependencyOrder dependencyOrder);
    void fieldsChanged();
    void paramsChanged();
    void nodesChanged();
    void inputsChanged();
    void outputsChanged();
    void detailsChanged();
    void countChanged(int count);
    void indexChanged(int index);
    void xChanged(qreal x);
    void yChanged(qreal y);

protected:
    virtual void classBegin();

private:
    const Node *nodeInChildren(const QString n) const;

    // fields property
    static void fields_append(QQmlListProperty<Field> *, Field *);
    static int fields_count(QQmlListProperty<Field> *);
    static Field *field_at(QQmlListProperty<Field> *, int);
    static void fields_clear(QQmlListProperty<Field> *);

    // children property
    static void nodes_append(QQmlListProperty<Node> *, Node *);
    static int nodes_count(QQmlListProperty<Node> *);
    static Node *node_at(QQmlListProperty<Node> *, int);
    static void nodes_clear(QQmlListProperty<Node> *);

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

    DependencyOrder m_dependencyOrder;
    QList<Field *> m_fields;
    QList<Param *> m_params;
    QList<Input *> m_inputs;
    QList<Output *> m_outputs;
    QJSValue m_details;
    int m_count;
    int m_index;
    QJSValue m_frames;
    qreal m_x, m_y; // should this be QVector2D? does it matter?
//    QList<Node *> m_extraDependencies;
};

Q_DECLARE_METATYPE(Node*) // makes available to QVariant

#endif // NODEBASE_H
