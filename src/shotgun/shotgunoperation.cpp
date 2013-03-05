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
        int count = branch->details().property("length").toInt();

        // needed for lastVersion
        // TODO: find a way to drop this dependency
        const ReleaseOperationAttached *releaseAttached = branch->attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);
        Q_ASSERT(releaseAttached);

        foreach (QObject *o, node()->children()) {
            ShotgunEntity *sge = qobject_cast<ShotgunEntity*>(o);
            if (sge) {
                if (sgop->mode() == ShotgunOperation::Pull && m_action == ShotgunOperationAttached::Find) {
                    if (count == 1) {
                        branch->setIndex(0);
                        QVariantMap context = qjsvalue_cast<QVariantMap>(branch->details().property(0).property("context"));
                        readEntity(sge, updateFieldsWithEnv(context));
                    } else {
                        error() << node() << "Can't do a Shotgun Find of" << branch->details().property("length").toInt() << "elements";
                    }
                } else if (sgop->mode() == ShotgunOperation::Push && m_action == ShotgunOperationAttached::Create) {
                    if (count == 1) {
                        branch->setIndex(0);
                        QVariantMap context = qjsvalue_cast<QVariantMap>(branch->details().property(0).property("context"));
                        createEntity(sge, updateFieldsWithEnv(context));
                    } else if (count) {
                        QVariantList contextList;
                        for (int i = 0; i < branch->details().property("length").toInt(); i++) {
                            branch->setIndex(i);

                            QVariantMap context = qjsvalue_cast<QVariantMap>(branch->details().property(i).property("context"));

                            contextList.append(updateFieldsWithEnv(context));
                        }

                        batchCreateEntities(sge, contextList);
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

const QVariantMap ShotgunOperationAttached::updateFieldsWithEnv(const QVariantMap newContext) const
{
    // add data from env that is not in the context
    QVariantMap d = context();
    for (QVariantMap::const_iterator i = newContext.constBegin(); i != newContext.constEnd(); ++i) {
        d.insert(i.key(), i.value());
    }

    return d;
}

void ShotgunOperationAttached::readEntity(const ShotgunEntity* sge, const QVariantMap context)
{
    trace() << node() << ".readEntity(" << sge << "," << context << ")";
    m_pendingTransactions++;

    ShotgunOperation *sgop = operation<ShotgunOperation>();
    Q_ASSERT(sgop);

    QVariantList filters = sge->buildFilters(context);

    debug() << "--type" << sge->name() << "filters" << QJsonDocument::fromVariant(filters);

    ShotgunReply *reply = sgop->shotgun()->findOne(sge->name(), filters, sge->buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunOperationAttached::onReadCreateEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunOperationAttached::onShotgunError);
}

void ShotgunOperationAttached::createEntity(const ShotgunEntity* sge, const QVariantMap context)
{
    trace() << node() << ".createEntity(" << sge << "," << context << ")";
    m_pendingTransactions++;

    ShotgunOperation *sgop = qobject_cast<ShotgunOperation *>(operation());
    Q_ASSERT(sgop);

    QVariantMap data = sge->buildData(context);

    debug() << node() << operation() << ".createEntity" << sge;
    debug() << "--type" << sge->name() << "data" << QJsonDocument::fromVariant(data);

    ShotgunReply *reply = sgop->shotgun()->create(sge->name(), data, sge->buildFields());
    connect(reply, &ShotgunReply::finished, this, &ShotgunOperationAttached::onReadCreateEntityFinished);
    connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
            this, &ShotgunOperationAttached::onShotgunError);
}

void ShotgunOperationAttached::batchCreateEntities(ShotgunEntity *sge, const QVariantList contextList)
{
    trace() << node() << ".batchCreateEntities(" << sge << "," << contextList << ")";
    m_pendingTransactions++;

    ShotgunOperation *sgop = operation<ShotgunOperation>();
    Q_ASSERT(sgop);

    QVariantList batchRequests;

    foreach (const QVariant context, contextList) {
        QVariantMap batchRequest;

        batchRequest["request_type"] = "create";
        batchRequest["entity_type"] = sge->name();
        batchRequest["data"] = sge->buildData(context.toMap());
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

void ShotgunOperationAttached::addDetail(const QVariantMap entity)
{
    BranchBase *branch = firstParent<BranchBase>();
    Q_ASSERT(branch);

    QJSValue detail = branch->details().property(branch->index());
    if (detail.isUndefined()) {
        detail = newObject();
        branch->details().setProperty(branch->index(), detail);
    }

    QJSValue entities = detail.property("sg");
    if (entities.isUndefined()) {
        entities = newObject();
        detail.setProperty("sg", entities);
    }

    entities.setProperty(entity.value("type").toString(), toScriptValue(entity));

    // TODO: should only do this once, instead of after every update
    emit branch->detailsChanged();
}

void ShotgunOperationAttached::onReadCreateEntityFinished(const QVariant result)
{
    trace() << ".onReadCreateEntityFinished(" << result << ")";
    addDetail(result.toMap());

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
    trace() << ".onBatchCreateEntitiesFinished(" << result << ")";
    BranchBase *branch = firstParent<BranchBase>();
    Q_ASSERT(branch);

    for (int i = 0; i < result.toList().length(); i++) {
        branch->setIndex(i);
        addDetail(result.toList().at(i).toMap());
    }

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
        trace() << node() << ".onShotgunError() [error is" << reply->errorString() << "]";
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
            targetAttached->run(operation(), context());
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
