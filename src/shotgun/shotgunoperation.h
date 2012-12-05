#ifndef SHOTGUNOPERATION_H
#define SHOTGUNOPERATION_H

#include "operation.h"
#include "shotgun.h"

class ShotgunEntity;
class BranchBase;

class ShotgunOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_ENUMS(Action)
    Q_PROPERTY(Action action READ action WRITE setAction NOTIFY actionChanged)
public:
    enum Action { Find, Create };

    explicit ShotgunOperationAttached(QObject *object);

    Action action() const;
    void setAction(Action);

    virtual void reset();
    virtual void run();

signals:
    void actionChanged(Action action);

protected:
    virtual OperationAttached::Status extraStatus() const;
    virtual void runExtra();
    virtual void resetExtra();
    virtual void resetExtraStatus();
    
    virtual const QMetaObject *operationMetaObject() const;

    void readEntity(const ShotgunEntity *, const QVariantMap);
    void createEntity(const ShotgunEntity *, const QVariantMap);
    void batchCreateEntities(ShotgunEntity *, const QVariantList);

    void addDetail(const QVariantMap entity);

protected slots:
    void onReadCreateEntityFinished(const QVariant);
    void onBatchCreateEntitiesFinished(const QVariant);
    void onShotgunError();

private:
    const QVariantMap updateFieldsWithEnv(const QVariantMap fields) const;

    Action m_action;
    int m_pendingTransactions;
};

class ShotgunOperation : public Operation
{
    Q_OBJECT
    Q_ENUMS(Mode ShotgunOperationAttached::Action)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(Shotgun *shotgun READ shotgun WRITE setShotgun NOTIFY shotgunChanged)
public:
    enum Mode { Pull, Push };
    ShotgunOperation(QObject *parent = 0);

    Mode mode() const;
    void setMode(Mode);

    const Shotgun *shotgun() const;
    Shotgun *shotgun();
    void setShotgun(Shotgun *);

    static ShotgunOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void modeChanged(Mode mode);
    void shotgunChanged(Shotgun *shotgun);

private:
    Mode m_mode;
    Shotgun *m_shotgun;
    NodeBase *m_runningNode;
    QVariant m_runningEnv;
};

QML_DECLARE_TYPEINFO(ShotgunOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
