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
    debug() << this << node() << operation() << ".run";

    switch(qobject_cast<TractorOperation *>(operation())->mode()) {
    case TractorOperation::Submit:
        generateTask();
        break;
    case TractorOperation::Execute:
        executeTask();
        break;
    }

    continueRunning();
}

void TractorOperationAttached::generateTask()
{
    debug() << this << node() << operation() << ".generateTask";

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
    debug() << this << node() << operation() << ".executeTask";

    Operation *targetOperation = qobject_cast<TractorOperation *>(operation())->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

    Q_ASSERT(targetOperation);

    // read data for each input
    foreach(NodeBase *input, node()->upstream()) {
        TractorOperationAttached *inputAttached = input->attachedPropertiesObject<TractorOperationAttached>(operation()->metaObject());
        Q_ASSERT(inputAttached);

        inputAttached->readTractorData(tractorDataPath());
    }

    targetOperation->run(node(), context());

    // write this node's data
    writeTractorData(tractorDataPath());

    setStatus(targetAttached->status());
}

void TractorOperationAttached::writeTractorData(const QString &dataPath)
{
    Operation *targetOperation = qobject_cast<TractorOperation *>(operation())->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

    QFile file(dataPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error() << "can't open file" << tractorDataPath() << "for writing";
        return;
    }

    QQmlContext *context = QQmlEngine::contextForObject(node());
    Q_ASSERT(context);
    JSDataStream out(context->engine(), &file);

    // write details and status to data dir
    out << node()->details();
    // need to downcast here to prevent QJSValue(int)
    static_cast<QDataStream&>(out) << static_cast<int>(targetAttached->status());
}

void TractorOperationAttached::readTractorData(const QString &dataPath)
{
    Operation *targetOperation = qobject_cast<TractorOperation *>(operation())->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

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
    int status;
    in >> details;
    // need to downcast here to prevent QJSValue(int)
    static_cast<QDataStream&>(in) >> status;

    // TODO: error handling

    node()->setDetails(details);
    targetAttached->setStatus(static_cast<OperationAttached::Status>(status));
    setStatus(static_cast<OperationAttached::Status>(status));
}

const QString TractorOperationAttached::tractorDataPath() const
{
    QString filename = node()->path() + ".json";
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

void TractorOperation::run(NodeBase *node, const QVariant context)
{
    Operation::run(node, context);

    if (m_mode == TractorOperation::Submit) {
        m_tractorJob = buildTractorJob(node, context);
    } else {
        m_tractorJob = 0;
    }
}

TractorJob *TractorOperation::buildTractorJob(NodeBase *node, const QVariant context)
{
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
