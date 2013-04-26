#ifndef OPERATION_H
#define OPERATION_H

#include <QDebug>
#include <QtQml>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QSet>
#include <QFlags>

#include "pugitem.h"

class Operation;
class Node;

// TODO: add params

class OperationAttached : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap context READ context NOTIFY contextChanged)
    Q_ENUMS(Status)
public:
    // these are listed in order of increasing comparison precedence
    enum Status { Invalid, Finished, None, Idle, Running, Error };

    enum Target { Inputs=0x01, Self=0x02, Children=0x04 };
    Q_DECLARE_FLAGS(Targets, Target)

    explicit OperationAttached(QObject *parent = 0);

    Status status() const;
    void setStatus(Status);

    const QVariantMap context() const;

    // use like this: foo->operation<MyOperation>()
    template <class T>
    const T *operation() const { return qobject_cast<const T*>(m_operation); }

    template <class T>
    T *operation() { return qobject_cast<T*>(m_operation); }

    const Operation* operation() const;
    Operation *operation();

    const Node *node() const;
    Node *node();

    virtual void reset();
    virtual void run();

    void run(Operation *, Targets);

    void resetAllStatus();
    void resetAll(const QVariantMap, QSet<const OperationAttached *>&);

signals:
    // attachable signals
    void prepare(const QVariant context);
    void prepareFinished(int status);

    void cook(const QVariant context);
    void cookFinished(int status);

    void statusChanged(Status status);
    void contextChanged(const QVariant context);
    void finished(OperationAttached *);

protected slots:
    void onPrepareFinished(int);
    void onCookFinished(int);

    void onInputFinished(OperationAttached *);
    void onChildFinished(OperationAttached *);

protected:
    void setContext(const QVariantMap);

    Status inputsStatus() const;
    Status childrenStatus() const;

    void resetInputsStatus();
    void resetChildrenStatus();

    void resetInputs(QSet<const OperationAttached *>&);
    void resetChildren(QSet<const OperationAttached *>&);

    void runInputs();
    void runChildren();

    void continueRunning();

    virtual const QMetaObject *operationMetaObject() const = 0;

    template <typename T>
    QJSValue toScriptValue(T value) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(QObject::parent());
        Q_ASSERT(context);

        return context->engine()->toScriptValue(value);
    }

    QJSValue newArray(uint length = 0) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(QObject::parent());
        Q_ASSERT(context);

        return context->engine()->newArray(length);
    }

    QJSValue newObject() const
    {
        QQmlContext *context = QQmlEngine::contextForObject(QObject::parent());
        Q_ASSERT(context);

        return context->engine()->newObject();
    }

    QJSValue newQObject(QObject *object) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(QObject::parent());
        Q_ASSERT(context);

        return context->engine()->newQObject(object);
    }

private:
    void dispatchChildren();
    void dispatchInputs();
    void dispatch();

    int m_prepareCount;
    int m_cookCount;
    Status m_status;
    QVariantMap m_context;
    Operation *m_operation;
    Targets m_targets;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(OperationAttached::Targets)

class OperationStatusList : public QList<OperationAttached::Status>
{
public:
    OperationAttached::Status status() const
    {
        OperationAttached::Status result = OperationAttached::Invalid;
        foreach (const OperationAttached::Status s, *this) {
            if (s > result)
                result = s;
        }
        return result;
    }
};


class Operation : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PugItem> data READ data_ NOTIFY dataChanged)
    Q_PROPERTY(QQmlListProperty<Operation> dependencies READ dependencies_ NOTIFY dependenciesChanged)
    Q_PROPERTY(OperationAttached::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(OperationAttached::Status)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit Operation(QObject *parent = 0);

    QQmlListProperty<PugItem> data_();
    QQmlListProperty<Operation> dependencies_();

    const QList<const Operation *>& dependencies() const;
    const QList<Operation *>& dependencies();

    OperationAttached::Status status() const;
    void setStatus(OperationAttached::Status);

    void resetAll(Node *node, const QVariantMap context);
    void resetAllStatus(Node *node);

    Q_INVOKABLE virtual void run(Node *node, const QVariant context, bool reset=true,
            OperationAttached::Targets targets=OperationAttached::Inputs|OperationAttached::Self|OperationAttached::Children);

    //static OperationAttached *qmlAttachedProperties(QObject *); // must be defined in subclasses

    Node *node();
    const Node *node() const;

signals:
    void dataChanged();
    void dependenciesChanged();
    void statusChanged(OperationAttached::Status status);
    void finished(OperationAttached::Status status);

protected slots:
    void onDependencyFinished();
    void onTriggerFinished();
    void onFinished(OperationAttached *);

protected:
    void startRunning(Node *, OperationAttached::Targets);
    void continueRunning();

    OperationAttached::Status dependenciesStatus() const;
    void runDependencies();

    void runTriggers();

    void failOperation();

private:
    // data property
    static void data_append(QQmlListProperty<PugItem> *, PugItem *);
    static int data_count(QQmlListProperty<PugItem> *);
    static PugItem *data_at(QQmlListProperty<PugItem> *, int);
    static void data_clear(QQmlListProperty<PugItem> *);

    QList<Operation *> m_dependencies;

    Node *m_node;
    QVariantMap m_context;
    OperationAttached::Status m_status;
    OperationAttached::Targets m_targets;
};

inline QDebug operator<<(QDebug dbg, OperationAttached::Status s)
{
    switch (s) {
    case OperationAttached::Invalid:
        dbg << "Invalid";
        break;
    case OperationAttached::Finished:
        dbg << "Finished";
        break;
    case OperationAttached::None:
        dbg << "None";
        break;
    case OperationAttached::Idle:
        dbg << "Idle";
        break;
    case OperationAttached::Running:
        dbg << "Running";
        break;
    case OperationAttached::Error:
        dbg << "Error";
        break;
    default:
        dbg << "Unknown!";
        break;
    }

    return dbg;
}

#endif
