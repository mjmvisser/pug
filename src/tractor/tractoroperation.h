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
    Q_PROPERTY(QString tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(bool serialSubtasks READ hasSerialSubtasks WRITE setSerialSubtasks NOTIFY serialSubtasksChanged)
    Q_PROPERTY(TractorTask *tractorTask READ tractorTask NOTIFY tractorTaskChanged)
public:
    explicit TractorOperationAttached(QObject *parent = 0);

    const QString &serviceKey() const;
    void setServiceKey(const QString &);

    const QString &tags() const;
    void setTags(const QString &);

    bool hasSerialSubtasks() const;
    void setSerialSubtasks(bool);

    Q_INVOKABLE virtual void run();

    TractorTask *tractorTask();

signals:
    void serviceKeyChanged(const QString &serviceKey);
    void tagsChanged(const QString &tags);
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
    const QString contextString() const;

    void writeTractorData(const QString &dataPath) const;
    const QVariantMap tractorAttachedStatuses(const Operation *operation, const QVariantMap statuses=QVariantMap()) const;

    void readTractorData(const QString &dataPath);
    const QVariantMap setTractorAttachedStatuses(Operation *operation, const QVariantMap statuses);

    QString m_serviceKey;
    QString m_tags;
    bool m_serialSubtasksFlag;
    TractorTask *m_task;
};

class TractorOperation : public Operation
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(Operation *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_ENUMS(Mode)
public:
    enum Mode { Generate, Submit, Execute, Cleanup };

    explicit TractorOperation(QObject *parent = 0);

    Mode mode() const;
    void setMode(Mode);

    Operation *target();
    const Operation *target() const;
    void setTarget(Operation *);

    Q_INVOKABLE virtual void run(Node *node, const QVariant context, bool reset=true);

    static TractorOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void modeChanged(Mode mode);
    void targetChanged(Operation *target);

protected:
    TractorJob *buildTractorJob(Node *node, const QVariant context);

private:
    Mode m_mode;
    Operation *m_target;
};

QDebug operator<<(QDebug dbg, TractorOperation::Mode mode);

QML_DECLARE_TYPEINFO(TractorOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
