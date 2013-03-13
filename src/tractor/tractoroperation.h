#ifndef TRACTOROPERATION_H
#define TRACTOROPERATION_H

#include <QDataStream>

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

protected slots:
    void onTargetFinished(OperationAttached::Status status);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private:
    void generateTask();
    void executeTask();
    void cleanupTask();

    TractorOperationAttached *parentAttached();

    const QString tractorDataPath() const;

    void writeTractorData(const QString &dataPath) const;
    void writeTractorAttachedStatuses(QDataStream &stream, const Operation *operation) const;

    void readTractorData(const QString &dataPath);
    void readTractorAttachedStatuses(QDataStream &stream, Operation *operation);

    QString m_serviceKey;
    bool m_serialSubtasksFlag;
    TractorTask *m_tractorTask;
};

class TractorOperation : public Operation
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(Operation *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(TractorJob *tractorJob READ tractorJob NOTIFY tractorJobChanged)
    Q_ENUMS(Mode)
public:
    enum Mode { Generate, Submit, Execute, Cleanup };

    explicit TractorOperation(QObject *parent = 0);

    Mode mode() const;
    void setMode(Mode);

    Operation *target();
    const Operation *target() const;
    void setTarget(Operation *);

    TractorJob *tractorJob();

    Q_INVOKABLE virtual void run(Node *node, const QVariant context, bool reset=true);

    static TractorOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void modeChanged(Mode mode);
    void targetChanged(Operation *target);
    void tractorJobChanged(TractorJob *tractorJob);

protected:
    TractorJob *buildTractorJob(Node *node, const QVariant context);

private:
    Mode m_mode;
    Operation *m_target;
    TractorJob *m_tractorJob;
};

QDebug operator<<(QDebug dbg, TractorOperation::Mode mode);

QML_DECLARE_TYPEINFO(TractorOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
