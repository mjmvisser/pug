#ifndef SHOTGUNENTITY_H
#define SHOTGUNENTITY_H

#include <QString>
#include <QQmlListProperty>

#include "nodebase.h"
#include "branchbase.h"
#include "shotgunoperation.h"
#include "shotgunfield.h"

class ShotgunEntity : public NodeBase
{
    Q_OBJECT
    Q_PROPERTY(QString shotgunEntity READ shotgunEntityName WRITE setShotgunEntityName NOTIFY shotgunEntityNameChanged)
    Q_PROPERTY(BranchBase *branch READ branch NOTIFY branchChanged);
    Q_PROPERTY(QQmlListProperty<ShotgunField> shotgunFields READ shotgunFields_ NOTIFY shotgunFieldsChanged)
public:
    explicit ShotgunEntity(QObject *parent = 0);

    const QString shotgunEntityName() const;
    void setShotgunEntityName(const QString);

    const BranchBase *branch() const;
    BranchBase *branch();

    QQmlListProperty<ShotgunField> shotgunFields_();
    // filters for find
    Q_INVOKABLE const QVariantList buildFilters(const QVariantMap fields) const;

    // data for create
    Q_INVOKABLE const QVariantMap buildData(const QVariantMap fields) const;

    // list of fields by name
    Q_INVOKABLE const QStringList buildFields() const;

signals:
    void shotgunEntityNameChanged(const QString shotgunEntity);
    void branchChanged(const BranchBase *branch);
    void shotgunFieldsChanged();

    void shotgunPull(const QVariant context, Shotgun *shotgun);
    void shotgunPulled(int status);
    void shotgunPush(const QVariant context, Shotgun *shotgun);
    void shotgunPushed(int status);

protected:
    virtual void componentComplete();

protected slots:
    void onShotgunPull(const QVariant context, Shotgun *shotgun);
    void onShotgunPush(const QVariant context, Shotgun *shotgun);

private slots:
    void onBranchCountChanged(int count);
    void onReadFinished(const QVariant);
    void onCreateFinished(const QVariant);
    void onBatchCreateFinished(const QVariant);
    void onShotgunReadError();
    void onShotgunCreateError();
    void updateCount();

private:
    void readEntity(Shotgun *);
    void createEntity(Shotgun *);
    void batchCreateEntities(Shotgun *);

    const QVariantList data() const;
    const QStringList fields() const;
    const QVariantList filters() const;

    // shotgunFields property
    static void shotgunFields_append(QQmlListProperty<ShotgunField> *, ShotgunField *);
    static int shotgunFields_count(QQmlListProperty<ShotgunField> *);
    static ShotgunField *shotgunFields_at(QQmlListProperty<ShotgunField> *, int);
    static void shotgunFields_clear(QQmlListProperty<ShotgunField> *);

    QString m_shotgunEntityName;
    OperationAttached::Status m_status;
};


#endif
