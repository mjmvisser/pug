#include <QDebug>
#include <QJsonDocument>

#include "shotgunoperation.h"
#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branchbase.h"

#include "releaseoperation.h"

ShotgunOperationAttached::ShotgunOperationAttached(QObject *object) :
    OperationAttached(object),
    m_action(ShotgunOperationAttached::Find),
    m_pendingTransactions(0)
{
}

const QVariantMap ShotgunOperationAttached::entities() const
{
    return m_entities;
}

void ShotgunOperationAttached::setEntities(const QVariantMap e)
{
    if (m_entities != e) {
        m_entities = e;
        emit entitiesChanged();
    }
}

int ShotgunOperationAttached::index() const
{
    return m_index;
}

void ShotgunOperationAttached::setIndex(int i)
{
    if (m_index != i) {
        m_index = i;
        emit indexChanged(m_index);
    }
}

ShotgunOperationAttached::Action ShotgunOperationAttached::action() const
{
    return m_action;
}

void ShotgunOperationAttached::setAction(ShotgunOperationAttached::Action a)
{
    if (m_action != a) {
        m_action = a;
        emit actionChanged(m_action);
    }
}

void ShotgunOperationAttached::reset()
{
    OperationAttached::reset();
    setEntities(QVariantMap());
    m_pendingTransactions = 0;
}

void ShotgunOperationAttached::run()
{
    debug() << node() << operation() << ".run";

    ShotgunOperation *sgop = operation<ShotgunOperation>();

    if (!sgop->shotgun()) {
        error() << sgop << "has no shotgun object";
        setStatus(OperationAttached::Error);
        continueRunning();
        return;
    }

    BranchBase *branch = firstParent<BranchBase>();

    if (branch && !branch->isRoot()) {
        int count = branch->elements().count();

        // needed for lastVersion
        // TODO: find a way to drop this dependency
        const ReleaseOperationAttached *releaseAttached = branch->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
        Q_ASSERT(releaseAttached);

        foreach (QObject *o, node()->children()) {
            ShotgunEntity *sge = qobject_cast<ShotgunEntity*>(o);
            if (sge) {
                if (sgop->mode() == ShotgunOperation::Pull && m_action == ShotgunOperationAttached::Find) {
                    if (count == 1) {
                        QVariant fields = branch->parse(branch->elements().at(0)->path());
                        readEntity(sge, fields.toMap());
                    } else {
                        error() << node() << "Can't do a Shotgun Find of" << branch->elements().count() << "elements";
                    }
                } else if (sgop->mode() == ShotgunOperation::Push && m_action == ShotgunOperationAttached::Create) {
                    if (count == 1) {
                        setIndex(0);
                        debug() << "set ShotgunOperation.index to" << m_index;
                        QVariant fields = branch->parse(branch->elements().at(0)->path());
                        createEntity(sge, fields.toMap());
                    } else if (count) {
                        batchCreateEntities(sge);
                    } else {
                        error() << node() << "Can't do a Shotgun Create with no elements";
                    }
                }
            }
        }
    }

    if (m_pendingTransactions == 0) {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }

}

void ShotgunOperationAttached::readEntity(const ShotgunEntity* sge, const QVariantMap fields)
{
    m_pendingTransactions++;

    ShotgunOperation *sgop = operation<ShotgunOperation>();
    Q_ASSERT(sgop);

    QVariantMap d = env();
    for (QVariantMap::const_iterator i = fields.constBegin(); i != fields.constEnd(); ++i) {
        d.insert(i.key(), i.value());
    }

    QVariantList filters = sge->buildFilters(d);

    debug() << node() << operation() << ".readEntity" << sge;
    debug() << "--type" << sge->name() << "filters" << QJsonDocument::fromVariant(filters);

    ShotgunReply *reply = sgop->shotgun()->findOne(sge->name(), filters, sge->buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunOperationAttached::onReadCreateEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunOperationAttached::onShotgunError);
}

void ShotgunOperationAttached::createEntity(const ShotgunEntity* sge, const QVariantMap fields)
{
    m_pendingTransactions++;

    ShotgunOperation *sgop = qobject_cast<ShotgunOperation *>(operation());
    Q_ASSERT(sgop);

    QVariantMap d = env();
    for (QVariantMap::const_iterator i = fields.constBegin(); i != fields.constEnd(); ++i) {
        d.insert(i.key(), i.value());
    }

    QVariantMap data = sge->buildData(d);

    debug() << node() << operation() << ".createEntity" << sge;
    debug() << "--type" << sge->name() << "data" << QJsonDocument::fromVariant(data);

    ShotgunReply *reply = sgop->shotgun()->create(sge->name(), data, sge->buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunOperationAttached::onReadCreateEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunOperationAttached::onShotgunError);
}

void ShotgunOperationAttached::batchCreateEntities(ShotgunEntity *sge)
{
    m_pendingTransactions++;

    ShotgunOperation *sgop = operation<ShotgunOperation>();
    Q_ASSERT(sgop);

    QVariantList batchRequests;

    BranchBase *branch = firstParent<BranchBase>();
    Q_ASSERT(branch);

    for (int i = 0; i < branch->elements().count(); i++) {
        setIndex(i);
        debug() << "set ShotgunOperation.index to" << m_index;

        QVariantMap fields = branch->parse(branch->elements().at(i)->path()).toMap();

        QVariantMap d = env();
        for (QVariantMap::const_iterator it = fields.constBegin(); it != fields.constEnd(); ++it) {
            d.insert(it.key(), it.value());
        }

        QVariantMap batchRequest;

        batchRequest["request_type"] = "create";
        batchRequest["entity_type"] = sge->name();
        batchRequest["data"] = sge->buildData(d);
        QStringList returnFields = sge->buildFields();
        if (returnFields.length() > 0)
            batchRequest["return_fields"] = returnFields;

        batchRequests.append(batchRequest);
    }

    debug() << node() << operation() << ".batchCreateEntities" << sge;
    debug() << "requests:" << batchRequests;

    ShotgunReply *reply = sgop->shotgun()->batch(batchRequests);
    connect(reply, &ShotgunReply::finished, this, &ShotgunOperationAttached::onBatchCreateEntitiesFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunOperationAttached::onShotgunError);
}

void ShotgunOperationAttached::onReadCreateEntityFinished(const QVariant result)
{
    m_entities.insert(result.toMap().value("type").toString(), result);
    emit entitiesChanged();

    QObject::sender()->deleteLater();

    debug() << node() << operation() << ".onReadCreateEntityFinished";
    debug() << "received" << QJsonDocument::fromVariant(result);

    m_pendingTransactions--;

    if (m_pendingTransactions == 0) {
        if (status() != OperationAttached::Error)
            setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void ShotgunOperationAttached::onBatchCreateEntitiesFinished(const QVariant result)
{
    m_entities.insert(result.toList().at(0).toMap().value("type").toString(), result);
    emit entitiesChanged();

    QObject::sender()->deleteLater();

    debug() << node() << operation() << ".onReadCreateEntityFinished";
    debug() << "received" << QJsonDocument::fromVariant(result);

    m_pendingTransactions--;

    if (m_pendingTransactions == 0) {
        if (status() != OperationAttached::Error)
            setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void ShotgunOperationAttached::onShotgunError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    if (reply) {
        error() << node() << ".onShotgunError for" << node() << "got Shotgun error:" << reply->errorString();
        setStatus(OperationAttached::Error);
    } else {
        error() << ".onShotgunError WTF";
    }

    m_pendingTransactions--;

    reply->deleteLater();

    if (m_pendingTransactions == 0) {
        continueRunning();
    }
}

OperationAttached::Status ShotgunOperationAttached::extraStatus() const
{
    OperationAttached::Status es = OperationAttached::Invalid;

    // only valid if we're on a branch
    const ReleaseOperationAttached *releaseAttached = node()->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
    Q_ASSERT(releaseAttached);

    // release target is a dependency
    const BranchBase *targetBranch = releaseAttached->target();
    if (targetBranch) {
        const OperationAttached *targetAttached = targetBranch->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        OperationAttached::Status targetStatus = targetAttached->status();
        if (targetStatus > es)
            es = targetStatus;
    }

    return es;
}

void ShotgunOperationAttached::runExtra()
{
    ReleaseOperationAttached *releaseAttached = node()->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
    Q_ASSERT(releaseAttached);

    BranchBase *targetBranch = releaseAttached->target();
    if (targetBranch) {
        OperationAttached *targetAttached = targetBranch->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        OperationAttached::Status targetStatus = targetAttached->status();

        if (targetStatus == OperationAttached::None || targetStatus == OperationAttached::Idle || targetStatus == OperationAttached::Running)
            connect(targetAttached, &OperationAttached::finished, this, &ShotgunOperationAttached::onExtraFinished);

        if (targetStatus == OperationAttached::None || targetStatus == OperationAttached::Idle)
            targetAttached->run(operation(), env());
    }
}

void ShotgunOperationAttached::resetExtra()
{
    ReleaseOperationAttached *releaseAttached = node()->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
    Q_ASSERT(releaseAttached);

    BranchBase *targetBranch = releaseAttached->target();
    if (targetBranch) {
        OperationAttached *targetAttached = targetBranch->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        targetAttached->resetAll();
    }
}

void ShotgunOperationAttached::resetExtraStatus()
{
    ReleaseOperationAttached *releaseAttached = node()->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
    Q_ASSERT(releaseAttached);

    BranchBase *targetBranch = releaseAttached->target();
    if (targetBranch) {
        OperationAttached *targetAttached = targetBranch->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        targetAttached->resetAllStatus();
    }
}

const QMetaObject *ShotgunOperationAttached::operationMetaObject() const
{
    return &ShotgunOperation::staticMetaObject;
}

ShotgunOperation::ShotgunOperation(QObject *parent) :
    Operation(parent),
    m_shotgun()
{
}

ShotgunOperation::Mode ShotgunOperation::mode() const
{
    return m_mode;
}

void ShotgunOperation::setMode(ShotgunOperation::Mode m)
{
    m_mode = m;
}

Shotgun *ShotgunOperation::shotgun()
{
    return m_shotgun;
}

void ShotgunOperation::setShotgun(Shotgun *sg)
{
    m_shotgun = sg;
}

ShotgunOperationAttached *ShotgunOperation::qmlAttachedProperties(QObject *object)
{
    return new ShotgunOperationAttached(object);
}
