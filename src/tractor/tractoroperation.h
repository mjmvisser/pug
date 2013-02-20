#ifndef TRACTOROPERATION_H
#define TRACTOROPERATION_H

#include "operation.h"
#include "tractorjob.h"
#include "tractortask.h"

class TractorOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_PROPERTY(QString serviceKey READ serviceKey WRITE setServiceKey NOTIFY serviceKeyChanged)
    Q_PROPERTY(bool serialSubtasks READ hasSerialSubtasks WRITE setSerialSubtasks NOTIFY serialSubtasksChanged)
    Q_PROPERTY(TractorTask *tractorTask READ tractorTask NOTIFY tractorTaskChanged)
public:
    explicit TractorOperationAttached(QObject *parent = 0);

    const QString &serviceKey() const;
    void setServiceKey(const QString &);

    bool hasSerialSubtasks() const;
    void setSerialSubtasks(bool);

    Q_INVOKABLE virtual void run();

    TractorTask *tractorTask();

signals:
    void serviceKeyChanged(const QString &serviceKey);
    void serialSubtasksChanged(bool serialSubtasks);
    void tractorTaskChanged();

protected:
    virtual const QMetaObject *operationMetaObject() const;

private:
    TractorOperationAttached *parentAttached();

    QString m_serviceKey;
    bool m_serialSubtasksFlag;
    TractorTask *m_tractorTask;
};

class TractorOperation : public Operation
{
    Q_OBJECT
    Q_PROPERTY(Operation *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(TractorJob *tractorJob READ tractorJob NOTIFY tractorJobChanged)
public:
    explicit TractorOperation(QObject *parent = 0);

    Operation *target();
    const Operation *target() const;
    void setTarget(Operation *);

    TractorJob *tractorJob();

    Q_INVOKABLE virtual void run(NodeBase *node, const QVariant context);

    static TractorOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void targetChanged(Operation *target);
    void tractorJobChanged(TractorJob *tractorJob);

protected:
    TractorJob *buildTractorJob(NodeBase *node, const QVariant context);

private:
    Operation *m_target;
    TractorJob *m_tractorJob;
};

QML_DECLARE_TYPEINFO(TractorOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
