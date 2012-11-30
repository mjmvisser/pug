#ifndef SHOTGUNENTITY_H
#define SHOTGUNENTITY_H

#include <QString>
#include <QQmlListProperty>

#include "nodebase.h"
#include "shotgunoperation.h"
#include "shotgunfield.h"

class ShotgunEntity : public NodeBase
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ShotgunField> shotgunFields READ shotgunFields_ NOTIFY shotgunFieldsChanged)
public:
    explicit ShotgunEntity(QObject *parent = 0);

    QQmlListProperty<ShotgunField> shotgunFields_();
    // filters for find
    Q_INVOKABLE const QVariantList buildFilters(const QVariantMap fields) const;

    // data for create
    Q_INVOKABLE const QVariantMap buildData(const QVariantMap fields) const;

    // list of fields by name
    Q_INVOKABLE const QStringList buildFields() const;

signals:
    void shotgunFieldsChanged();

private:
    // shotgunFields property
    static void shotgunFields_append(QQmlListProperty<ShotgunField> *, ShotgunField *);
    static int shotgunFields_count(QQmlListProperty<ShotgunField> *);
    static ShotgunField *shotgunFields_at(QQmlListProperty<ShotgunField> *, int);
    static void shotgunFields_clear(QQmlListProperty<ShotgunField> *);
};


#endif
