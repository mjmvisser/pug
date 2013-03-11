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
    enum Mode { Skip=0x00, Pull=0x01, PullAtIndex=0x02, Push=0x04, PushAtIndex=0x08 };

    explicit ShotgunOperationAttached(QObject *object);

    Action action() const;
    void setAction(Action);

    virtual void run();

signals:
    void actionChanged(Action action);

    void shotgunPull(const QVariant env, Shotgun *shotgun);
    void shotgunPullAtIndex(int index, const QVariant env, Shotgun *shotgun);
    void shotgunPush(const QVariant env, Shotgun *shotgun);
    void shotgunPushAtIndex(int index, const QVariant env, Shotgun *shotgun);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onShotgunPulled(int);
    void onShotgunPulledAtIndex(int, int);
    void onShotgunPushed(int);
    void onShotgunPushedAtIndex(int, int);

private:
    Action m_action;
    int m_mode;
    OperationStatusList m_indexStatus;
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

QDebug operator<<(QDebug dbg, ShotgunOperation::Mode mode);

QML_DECLARE_TYPEINFO(ShotgunOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
