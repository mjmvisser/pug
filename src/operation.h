#ifndef OPERATION_H
#define OPERATION_H

#include <QDebug>
#include <QtQml>
#include <QObject>
#include <QString>
#include <QVariant>

#include "pugitem.h"

class Operation;
class NodeBase;

class OperationAttached : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap env READ env WRITE setEnv NOTIFY envChanged)
    Q_ENUMS(Status)
public:
    // these are listed in order of increasing comparison precedence
    enum Status { Invalid, Finished, None, Idle, Running, Error };

    explicit OperationAttached(QObject *parent = 0);

    Status status() const;
    void setStatus(Status);

    QVariantMap env() const;
    void setEnv(const QVariantMap);

    // use like this: foo->operation<MyOperation>()
    template <class T>
    const T *operation() const { return qobject_cast<const T*>(m_operation); }

    template <class T>
    T *operation() { return qobject_cast<T*>(m_operation); }

    const Operation* operation() const;
    Operation *operation();

    const NodeBase *node() const;
    NodeBase *node();

    Q_INVOKABLE void resetStatus();
    Q_INVOKABLE virtual void reset();
    Q_INVOKABLE virtual void run() = 0;

    void run(Operation *, const QVariant);

    void resetAllStatus();
    void resetAll();

signals:
    void statusChanged(Status status);
    void envChanged(QVariant env);
    void finished(OperationAttached *operation);

protected slots:
    void onInputFinished(OperationAttached *);
    void onChildFinished(OperationAttached *);
    virtual void onExtraFinished(OperationAttached *);

protected:
    Status inputsStatus() const;
    Status childrenStatus() const;
    virtual Status extraStatus() const;
    Status dependenciesStatus() const;

    void resetInputsStatus();
    void resetChildrenStatus();
    virtual void resetExtraStatus();

    void resetInputs();
    void resetChildren();
    virtual void resetExtra();

    void runInputs();
    void runChildren();
    virtual void runExtra();
    void runDependencies();

    void continueRunning();

    virtual const QMetaObject *operationMetaObject() const = 0;

private:
    Status m_status;
    QVariantMap m_env;
    Operation *m_operation;
};

class Operation : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PugItem> data READ data_ NOTIFY dataChanged)
    Q_PROPERTY(QQmlListProperty<Operation> dependencies READ dependencies_ NOTIFY dependenciesChanged)
    Q_PROPERTY(QQmlListProperty<Operation> triggers READ triggers_ NOTIFY triggersChanged)
    Q_PROPERTY(OperationAttached::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(OperationAttached::Status)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit Operation(QObject *parent = 0);

    QQmlListProperty<PugItem> data_();
    QQmlListProperty<Operation> dependencies_();
    QQmlListProperty<Operation> triggers_();

    OperationAttached::Status status() const;
    void setStatus(OperationAttached::Status);

    Q_INVOKABLE virtual void run(NodeBase *node, const QVariant env);

    //static OperationAttached *qmlAttachedProperties(QObject *); // must be defined in subclasses

signals:
    void dataChanged();
    void dependenciesChanged();
    void triggersChanged();
    void statusChanged(OperationAttached::Status status);
    void finished(OperationAttached::Status status);

protected slots:
    void onDependencyFinished();
    void onTriggerFinished();
    void onFinished(OperationAttached *);

protected:
    void resetAll(NodeBase *node);

    void startRunning(NodeBase *node, const QVariant env);
    void continueRunning();

    OperationAttached::Status dependenciesStatus() const;
    void runDependencies();

    OperationAttached::Status triggersStatus() const;
    void runTriggers();

    void failOperation();

private:
    // data property
    static void data_append(QQmlListProperty<PugItem> *, PugItem *);
    static int data_count(QQmlListProperty<PugItem> *);
    static PugItem *data_at(QQmlListProperty<PugItem> *, int);
    static void data_clear(QQmlListProperty<PugItem> *);

    QList<Operation *> m_dependencies;
    QList<Operation *> m_triggers;

    NodeBase *m_runningNode;
    QVariant m_runningEnv;
    OperationAttached::Status m_status;
};

inline QDebug operator<<(QDebug dbg, OperationAttached::Status s)
{
    switch (s) {
    case OperationAttached::Invalid:
        dbg.nospace() << "Invalid";
        break;
    case OperationAttached::Finished:
        dbg.nospace() << "Finished";
        break;
    case OperationAttached::None:
        dbg.nospace() << "None";
        break;
    case OperationAttached::Idle:
        dbg.nospace() << "Idle";
        break;
    case OperationAttached::Running:
        dbg.nospace() << "Running";
        break;
    case OperationAttached::Error:
        dbg.nospace() << "Error";
        break;
    default:
        dbg.nospace() << "Unknown!";
        break;
    }

    return dbg.space();
}

#endif
