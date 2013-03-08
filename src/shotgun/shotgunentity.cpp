#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branchbase.h"
#include "shotgunoperation.h"

ShotgunEntity::ShotgunEntity(QObject *parent) :
    NodeBase(parent)
{
    addInput("branch");

    connect(this, &ShotgunEntity::shotgunPull, this, &ShotgunEntity::onShotgunPull);
    connect(this, &ShotgunEntity::shotgunPush, this, &ShotgunEntity::onShotgunPush);
}

BranchBase *ShotgunEntity::branch()
{
    // TODO: how do we emit branchChanged() when the parent changes?
    return parent<BranchBase>();
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


void ShotgunEntity::onShotgunPull(const QVariant context, Shotgun *shotgun)
{
    m_status = OperationAttached::Finished;
}

void ShotgunEntity::onShotgunPush(const QVariant context, Shotgun *shotgun)
{
    m_status = OperationAttached::Finished;
}

const QVariantList ShotgunEntity::buildFilters(const QVariantMap fields) const
{
    if (!branch()) {
        error() << "Not parented to a branch-derived node";
        return QVariantList();
    }

    QVariantList filterList;

    // build filters for this entity from the entity's shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf) {
            // ["<shotgun field name>", "is", <value for field name>]
            QVariant value = qjsvalue_cast<QVariant>(sgf->detail(0, "value"));
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


void ShotgunEntity::readEntity(Shotgun *shotgun, const QVariantMap context)
{
    trace() << ".readEntity(" << shotgun << "," context << ")";
    Q_ASSERT(shotgun);

    m_pendingTransactions++;

    QVariantList filters = buildFilters(context);

    debug() << "--type" << name() << "filters" << QJsonDocument::fromVariant(filters);

    ShotgunReply *reply = shotgun->findOne(name(), filters, buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onReadEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunEntity::onShotgunError);
}

void ShotgunEntity::createEntity(Shotgun *shotgun, const QVariantMap context)
{
    trace() << ".createEntity(" << shotgun << "," << context << ")";
    Q_ASSERT(shotgun);

    m_pendingTransactions++;

    QVariantMap data = buildData(context);

    debug() << "--type" << name() << "data" << QJsonDocument::fromVariant(data);

    ShotgunReply *reply = shotgun->create(name(), data, buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onCreateEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunEntity::onShotgunError);
}

void ShotgunEntity::batchCreateEntities(Shotgun *shotgun, const QVariantList contextList)
{
    trace() << ".batchCreateEntities(" << shotgun << "," contextList << ")";
    Q_ASSERT(shotgun);

    m_pendingTransactions++;

    QVariantList batchRequests;

    foreach (const QVariant context, contextList) {
        QVariantMap batchRequest;

        batchRequest["request_type"] = "create";
        batchRequest["entity_type"] = name();
        batchRequest["data"] = buildData(context.toMap());
        QStringList returnFields = buildFields();
        if (returnFields.length() > 0)
            batchRequest["return_fields"] = returnFields;

        batchRequests.append(batchRequest);
    }

    debug() << "requests:" << batchRequests;

    ShotgunReply *reply = shotgun->batch(batchRequests);
    connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onBatchCreateEntitiesFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunEntity::onShotgunError);
}

void ShotgunEntity::addDetail(int index, const QVariantMap entity)
{
    QJSValue detail = details().property(index);
    if (detail.isUndefined()) {
        detail = newObject();
        branch->details().setProperty(index, detail);
    }

    detail.setProperty("entity", toScriptValue(entity));

    // TODO: should only do this once, instead of after every update
    emit detailsChanged();
}

void ShotgunEntity::onReadEntityFinished(const QVariant result)
{
    trace() << ".onReadEntityFinished(" << result << ")";
    addDetail(0, result.toMap());

    QObject::sender()->deleteLater();

    debug() << "received" << QJsonDocument::fromVariant(result);

    m_pendingTransactions--;

    if (m_pendingTransactions == 0)
        emit shotgunPulled(static_cast<int>(m_status));
}

void ShotgunEntity::onCreateFinished(const QVariant result)
{
    trace() << ".onCreateEntityFinished(" << result << ")";
    addDetail(0, result.toMap());

    QObject::sender()->deleteLater();

    debug() << "received" << QJsonDocument::fromVariant(result);

    m_pendingTransactions--;

    if (m_pendingTransactions == 0)
        emit shotgunPushed(static_cast<int>(m_status));
}

void ShotgunEntity::onBatchCreateEntitiesFinished(const QVariant result)
{
    trace() << ".onBatchCreateEntitiesFinished(" << result << ")";
    BranchBase *branch = firstParent<BranchBase>();
    Q_ASSERT(branch);

    for (int i = 0; i < result.toList().length(); i++) {
        addDetail(i, result.toList().at(i).toMap());
    }

    QObject::sender()->deleteLater();

    debug() << "received" << QJsonDocument::fromVariant(result);

    m_pendingTransactions--;

    if (m_pendingTransactions == 0)
        emit shotgunPushed(static_cast<int>(m_status));
}

void ShotgunEntity::onShotgunReadError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    if (reply) {
        trace() << node() << ".onShotgunReadError() [error is" << reply->errorString() << "]";
        m_status = OperationAttached::Error;
    } else {
        error() << ".onShotgunError WTF";
    }

    m_pendingTransactions--;

    reply->deleteLater();

    if (m_pendingTransactions == 0)
        emit shotgunPulled(static_cast<int>(m_status));
}

void ShotgunEntity::onShotgunCreateError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    if (reply) {
        trace() << node() << ".onShotgunCreateError() [error is" << reply->errorString() << "]";
        m_status = OperationAttached::Error;
    } else {
        error() << ".onShotgunError WTF";
    }

    m_pendingTransactions--;

    reply->deleteLater();

    if (m_pendingTransactions == 0)
        emit shotgunPushed(static_cast<int>(m_status));
}
