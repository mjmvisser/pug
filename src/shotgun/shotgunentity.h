#ifndef SHOTGUNENTITY_H
#define SHOTGUNENTITY_H

#include <QString>
#include <QQmlListProperty>

#include "node.h"
#include "branch.h"
#include "shotgunoperation.h"
#include "shotgunfield.h"

class ShotgunEntity : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString shotgunEntity READ shotgunEntityName WRITE setShotgunEntityName NOTIFY shotgunEntityNameChanged)
    Q_PROPERTY(Branch *branch READ branch NOTIFY branchChanged);
    Q_PROPERTY(QQmlListProperty<ShotgunField> shotgunFields READ shotgunFields_ NOTIFY shotgunFieldsChanged)
public:
    explicit ShotgunEntity(QObject *parent = 0);

    const QString shotgunEntityName() const;
    void setShotgunEntityName(const QString);

    const Branch *branch() const;
    Branch *branch();

    QQmlListProperty<ShotgunField> shotgunFields_();

signals:
    void shotgunEntityNameChanged(const QString shotgunEntity);
    void branchChanged(const Branch *branch);
    void shotgunFieldsChanged();

    void shotgunPull(const QVariant context, Shotgun *shotgun);
    void shotgunPullFinished(int status);
    void shotgunPush(const QVariant context, Shotgun *shotgun);
    void shotgunPushFinished(int status);

protected:
    virtual void componentComplete();

protected slots:
    void onShotgunPull(const QVariant context, Shotgun *shotgun);
    void onShotgunPush(const QVariant context, Shotgun *shotgun);

private slots:
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
