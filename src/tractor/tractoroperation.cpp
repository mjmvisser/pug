#include <iostream>
#include "tractoroperation.h"
#include "tractortask.h"
#include "tractorcmd.h"
#include "tractorjob.h"
#include "nodebase.h"

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

    Operation *targetOperation = qobject_cast<TractorOperation *>(operation())->target();

    Q_ASSERT(targetOperation);


    // build the task
    // connect it up to our parent (which we can safely assume is complete)
    m_tractorTask = new TractorTask(parentAttached() ? parentAttached()->m_tractorTask : 0);
    m_tractorTask->setTitle(node()->objectName());
    TractorRemoteCmd *cmd = new TractorRemoteCmd(m_tractorTask);
    cmd->setCmd(QString("pug %1 %2").arg(targetOperation->objectName()).arg(node()->path()));
    m_tractorTask->addCmd(cmd);

    setStatus(OperationAttached::Finished);

    continueRunning();
}

const QMetaObject *TractorOperationAttached::operationMetaObject() const
{
    return &TractorOperation::staticMetaObject;
}

TractorOperation::TractorOperation(QObject *parent) :
        Operation(parent),
        m_target(),
        m_tractorJob()
{
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

    m_tractorJob = buildTractorJob(node, context);
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
