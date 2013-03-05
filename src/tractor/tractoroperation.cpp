#include <iostream>

#include <QVariant>
#include <QFile>
#include <QDir>

#include "tractoroperation.h"
#include "tractortask.h"
#include "tractorcmd.h"
#include "tractorjob.h"
#include "nodebase.h"
#include "branch.h"
#include "serializers.h"

TractorOperationAttached::TractorOperationAttached(QObject *parent) :
        OperationAttached(parent),
        m_serialSubtasksFlag(false),
        m_tractorTask()
{
}

TractorOperationAttached *TractorOperationAttached::parentAttached()
{
    NodeBase *p = node()->firstParent<NodeBase>();
    return p ? p->attachedPropertiesObject<TractorOperationAttached>(operationMetaObject()) : 0;
}

TractorTask *TractorOperationAttached::tractorTask()
{
    return m_tractorTask;
}

const QString &TractorOperationAttached::serviceKey() const
{
    return m_serviceKey;
}

void TractorOperationAttached::setServiceKey(const QString &key)
{
    if (m_serviceKey != key) {
        m_serviceKey = key;
        emit serviceKeyChanged(key);
    }
}

bool TractorOperationAttached::hasSerialSubtasks() const
{
    return m_serialSubtasksFlag;
}

void TractorOperationAttached::setSerialSubtasks(bool flag)
{
    if (m_serialSubtasksFlag != flag) {
        m_serialSubtasksFlag = flag;
        emit serialSubtasksChanged(flag);
    }
}

void TractorOperationAttached::run()
{
    trace() << node() << ".run()";

    switch(operation<TractorOperation>()->mode()) {
    case TractorOperation::Submit:
        generateTask();
        break;
    case TractorOperation::Execute:
        executeTask();
        break;
    case TractorOperation::Cleanup:
        cleanupTask();
        break;
    }

    continueRunning();
}

void TractorOperationAttached::generateTask()
{
    trace() << node() << ".generateTask()";

    // build the task
    // connect it up to our parent (which we can safely assume is complete)
    if (m_tractorTask)
        m_tractorTask->deleteLater();

    m_tractorTask = new TractorTask(parentAttached() ? parentAttached()->m_tractorTask : 0);
    m_tractorTask->setTitle(node()->objectName());

    // find the first branch parent and get the path
    Branch *branch = node()->firstParent<Branch>();
    if (!branch) {
        setStatus(OperationAttached::Finished);
        return;
    }

    QString branchPath;
    Element *element = branch->element(0);
    if (element)
    {
        branchPath = element->pattern();

        if (branchPath.isEmpty()) {
            error() << branch << "has no element path";
            return;
        }
    } else {
        error() << branch << ".elements has no entries";
        return;
    }

    // build a unique path for this tractor job and node (%j is replaced by tractor with job id)
    QString tractorDataDir = QDir(QDir(branchPath).filePath("tractor")).filePath("%j");

    TractorRemoteCmd *cmd = new TractorRemoteCmd(m_tractorTask);
    cmd->setCmd(QString("pug %1 %2 -properties mode=Execute -context TRACTOR_DATA_DIR=%3").arg(operation()->objectName()).arg(node()->path()).arg(tractorDataDir));
    m_tractorTask->addCmd(cmd);

    setStatus(OperationAttached::Finished);
}

void TractorOperationAttached::executeTask()
{
    trace() << node() << ".executeTask()";

    Operation *targetOperation = operation<TractorOperation>()->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

    if (node() == operation()->node()) {
        Q_ASSERT(targetOperation);

        connect(targetOperation, SIGNAL(finished(OperationAttached::Status)),
                this, SLOT(onTargetFinished(OperationAttached::Status)));

        targetOperation->run(node(), context(), false);
    } else {
        // we're an input of the run node
        readTractorData(tractorDataPath());

        setStatus(targetAttached->status());
    }
}

void TractorOperationAttached::cleanupTask()
{
    trace() << node() << ".cleanupTask()";
    debug() << node() << "removing" << tractorDataPath();
    bool success = QFile::remove(tractorDataPath());
    if (!success)
        warning() << node() << "Unable to remove file:" << tractorDataPath();
    setStatus(OperationAttached::Finished);
}

void TractorOperationAttached::onTargetFinished(OperationAttached::Status status)
{
    Operation *targetOperation = operation<TractorOperation>()->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

    trace() << node() << ".onTargetFinished(" << status << ")";

    disconnect(targetOperation, SIGNAL(finished(OperationAttached::Status)),
               this, SLOT(onTargetFinished(OperationAttached::Status)));

    // write this node's data
    writeTractorData(tractorDataPath());

    setStatus(targetAttached->status());

    // continue cooking
    continueRunning();
}

void TractorOperationAttached::writeTractorData(const QString &dataPath) const
{
    trace() << node() << ".writeTractorData(" << dataPath << ")";

    const Operation *targetOperation = operation<TractorOperation>()->target();

    QFile file(dataPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error() << "can't open file" << tractorDataPath() << "for writing";
        return;
    }

    QQmlContext *context = QQmlEngine::contextForObject(node());
    Q_ASSERT(context);
    JSDataStream out(context->engine(), &file);

    // write details and statuses to data dir
    out << node()->details();

    writeTractorAttachedStatuses(out, targetOperation);
}

void TractorOperationAttached::writeTractorAttachedStatuses(QDataStream &stream, const Operation *operation) const
{
    // write dependency statuses
    foreach(const Operation *dep, operation->dependencies()) {
        writeTractorAttachedStatuses(stream, dep);
    }

    // write own status
    const OperationAttached *attached = node()->attachedPropertiesObject<OperationAttached>(operation->metaObject());
    Q_ASSERT(attached->status() != OperationAttached::Invalid);
    stream << static_cast<int>(attached->status());

    // write trigger statuses
    foreach(const Operation *trig, operation->triggers()) {
        writeTractorAttachedStatuses(stream, trig);
    }
}

void TractorOperationAttached::readTractorData(const QString &dataPath)
{
    trace() << node() << ".readTractorData(" << dataPath << ")";

    Operation *targetOperation = operation<TractorOperation>()->target();

    Q_ASSERT(targetOperation);

    QFile file(dataPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error() << "can't open file" << tractorDataPath() << "for reading";
        setStatus(OperationAttached::Error);
        return;
    }

    QQmlContext *context = QQmlEngine::contextForObject(node());
    Q_ASSERT(context);
    JSDataStream in(context->engine(), &file);

    QJSValue details;
    in >> details;
    node()->setDetails(details);

    // TODO: error handling

    readTractorAttachedStatuses(in, targetOperation);

    Q_ASSERT(in.atEnd());
}

void TractorOperationAttached::readTractorAttachedStatuses(QDataStream &stream, Operation *operation)
{
    // read dependency statuses
    foreach(Operation *dep, operation->dependencies()) {
        readTractorAttachedStatuses(stream, dep);
    }

    // read own status
    OperationAttached *attached = node()->attachedPropertiesObject<OperationAttached>(operation->metaObject());

    int status;
    stream >> status;
    attached->setStatus(static_cast<OperationAttached::Status>(status));
    Q_ASSERT(static_cast<OperationAttached::Status>(status) != OperationAttached::Invalid);

    // read trigger statuses
    foreach(Operation *trig, operation->triggers()) {
        readTractorAttachedStatuses(stream, trig);
    }
}

const QString TractorOperationAttached::tractorDataPath() const
{
    QString filename = node()->path() + ".dat";
    filename.replace(QDir::separator(), '-');
    QString path = QDir(context()["TRACTOR_DATA_DIR"].toString()).filePath(filename);
    return path;
}

const QMetaObject *TractorOperationAttached::operationMetaObject() const
{
    return &TractorOperation::staticMetaObject;
}

TractorOperation::TractorOperation(QObject *parent) :
        Operation(parent),
        m_mode(TractorOperation::Submit),
        m_target(),
        m_tractorJob()
{
}

TractorOperation::Mode TractorOperation::mode() const
{
    return m_mode;
}

void TractorOperation::setMode(TractorOperation::Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        emit modeChanged(mode);
    }
}

Operation *TractorOperation::target()
{
    return m_target;
}

const Operation *TractorOperation::target() const
{
    return m_target;
}

void TractorOperation::setTarget(Operation *target)
{
    if (m_target != target) {
        m_target = target;
        emit targetChanged(target);
    }
}

TractorJob *TractorOperation::tractorJob()
{
    return m_tractorJob;
}

void TractorOperation::run(NodeBase *node, const QVariant context, bool reset)
{
    trace() << ".run(" << node << "," << context << "," << reset << ")";
    m_target->resetAll(node);
    Operation::run(node, context, reset);

    if (m_mode == TractorOperation::Submit) {
        m_tractorJob = buildTractorJob(node, context);
    } else {
        m_tractorJob = 0;
    }
}

TractorJob *TractorOperation::buildTractorJob(NodeBase *node, const QVariant context)
{
    trace() << ".buildTractorJob(" << node << "," << context << ")";
    TractorJob *job = new TractorJob(this);

    TractorOperationAttached *attached = node->attachedPropertiesObject<TractorOperationAttached>(metaObject());

    job->setTitle(context.toMap()["TITLE"].toString());
    job->addSubtask(attached->tractorTask());

    return job;
}

TractorOperationAttached *TractorOperation::qmlAttachedProperties(QObject *object)
{
    return new TractorOperationAttached(object);
}
