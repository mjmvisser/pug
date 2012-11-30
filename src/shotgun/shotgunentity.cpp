#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branchbase.h"
#include "shotgunoperation.h"

ShotgunEntity::ShotgunEntity(QObject *parent) :
    NodeBase(parent)
{
}

QQmlListProperty<ShotgunField> ShotgunEntity::shotgunFields_()
{
    return QQmlListProperty<ShotgunField>(this, 0, ShotgunEntity::shotgunFields_append,
                                        ShotgunEntity::shotgunFields_count,
                                        ShotgunEntity::shotgunFields_at,
                                        ShotgunEntity::shotgunFields_clear);
}

// fields property
void ShotgunEntity::shotgunFields_append(QQmlListProperty<ShotgunField> *prop, ShotgunField *p)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);

    p->setParent(that);
    emit that->shotgunFieldsChanged();
}

int ShotgunEntity::shotgunFields_count(QQmlListProperty<ShotgunField> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<ShotgunField *>(o))
            count++;
    }
    return count;
}

ShotgunField *ShotgunEntity::shotgunFields_at(QQmlListProperty<ShotgunField> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        ShotgunField *p = qobject_cast<ShotgunField *>(o);
        if (p) {
            if (index == i)
                return p;
            index++;
        }
    }
    return 0;
}

void ShotgunEntity::shotgunFields_clear(QQmlListProperty<ShotgunField> *prop)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);
    const QObjectList fields = prop->object->children();
    foreach (QObject *o, fields) {
        ShotgunField *p = qobject_cast<ShotgunField *>(o);
        if (p)
            p->setParent(0);
    }
    emit that->shotgunFieldsChanged();
}

const QVariantList ShotgunEntity::buildFilters(const QVariantMap fields) const
{
    const BranchBase *branch = firstParent<BranchBase>();
    if (!branch) {
        error() << this << "is not parented to a branch-derived node";
        return QVariantList();
    }

    QVariantList filterList;

    // build filters for this entity from the entity's shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf) {
            // sgf is a ShotgunField of ourself inside a Field f
            // ["<shotgun field name>", "is", <value for field name>]
            QVariant value = sgf->buildValue(branch, fields);
            if (value.isValid()) {
                QVariantList filter;
                filter << sgf->name() << "is" << value;
                filterList << QVariant(filter);
            }
        }
    }

    return filterList;
}

const QVariantMap ShotgunEntity::buildData(const QVariantMap fields) const
{
    const BranchBase *branch = firstParent<BranchBase>();
    if (!branch) {
        error() << this << "is not parented to a branch-derived node";
        return QVariantMap();
    }

    QVariantMap dataMap;

    // build data for this entity from the entity's shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf) {
            dataMap.insert(sgf->name(), sgf->buildValue(branch, fields));
        }
    }

    return dataMap;
}

const QStringList ShotgunEntity::buildFields() const
{
    const BranchBase *branch = firstParent<BranchBase>();
    if (!branch) {
        error() << this << "is not parented to a branch-derived node";
        return QStringList();
    }

    QStringList fieldList;

    // build a list of Shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf) {
            fieldList << sgf->name();
        }
    }

    return fieldList;
}
