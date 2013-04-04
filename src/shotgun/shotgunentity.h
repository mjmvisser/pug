#ifndef SHOTGUNENTITY_H
#define SHOTGUNENTITY_H

#include <QString>
#include <QQmlListProperty>

#include "node.h"
#include "branch.h"
#include "shotgunfield.h"

class ShotgunEntity : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString shotgunEntity READ shotgunEntityName WRITE setShotgunEntityName NOTIFY shotgunEntityNameChanged)
    Q_PROPERTY(Branch *branch READ branch NOTIFY branchChanged);
    Q_PROPERTY(QQmlListProperty<ShotgunField> shotgunFields READ shotgunFields_ NOTIFY shotgunFieldsChanged)
    Q_PROPERTY(Action action READ action WRITE setAction NOTIFY actionChanged)
    Q_ENUMS(Action)
public:
    enum Action { Invalid, None, Find, Create };

    explicit ShotgunEntity(QObject *parent = 0);

    const QString shotgunEntityName() const;
    void setShotgunEntityName(const QString);

    const Branch *branch() const;
    Branch *branch();

    QQmlListProperty<ShotgunField> shotgunFields_();
    const QList<const ShotgunField *> shotgunFields() const;

    Action action() const;
    void setAction(Action);

signals:
    void shotgunEntityNameChanged(const QString shotgunEntity);
    void branchChanged(const Branch *branch);
    void shotgunFieldsChanged();
    void actionChanged(Action action);

    void update(const QVariant context);
    void updateFinished(int status);
    void release(const QVariant context);
    void releaseFinished(int status);

protected slots:
    void onUpdate(const QVariant context);
    void onRelease(const QVariant context);

private slots:
    void onFindFinished(const QVariant);
    void onCreateFinished(const QVariant);
    void onBatchCreateFinished(const QVariant);
    void onShotgunReadError();
    void onShotgunCreateError();

private:
    void readEntity(Shotgun *);
    void createEntity(Shotgun *);
    void batchCreateEntities(Shotgun *);

    const QVariant data() const;
    const QStringList fields() const;
    const QVariant filters();

    static const QVariant jsValueToVariant(const QJSValue &jsValue);

    // shotgunFields property
    static void shotgunFields_append(QQmlListProperty<ShotgunField> *, ShotgunField *);
    static int shotgunFields_count(QQmlListProperty<ShotgunField> *);
    static ShotgunField *shotgunField_at(QQmlListProperty<ShotgunField> *, int);
    static void shotgunFields_clear(QQmlListProperty<ShotgunField> *);

    QList<ShotgunField *> m_shotgunFields;
    QString m_shotgunEntityName;
    Action m_action;
};


#endif
