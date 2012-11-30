#include "operation.h"
#include "nodebase.h"

OperationAttached::OperationAttached(QObject *parent) :
    PugItem(parent),
    m_operation()
{
}

QVariantMap OperationAttached::env() const
{
    return m_env;
}

void OperationAttached::setEnv(const QVariantMap env)
{
    if (m_env != env) {
        m_env = env;
        emit envChanged(env);
    }
}

OperationAttached::Status OperationAttached::status() const
{
    return m_status;
}

void OperationAttached::setStatus(OperationAttached::Status s)
{
    if (m_status != s) {
        m_status = s;
        emit statusChanged(s);
    }
}

const Operation *OperationAttached::operation() const
{
    return m_operation;
}

Operation *OperationAttached::operation()
{
    return m_operation;
}

const NodeBase* OperationAttached::node() const
{
    return parent<NodeBase>();
}

NodeBase* OperationAttached::node()
{
    return parent<NodeBase>();
}

void OperationAttached::resetStatus()
{
    setStatus(OperationAttached::None);
}

void OperationAttached::reset()
{
    setStatus(OperationAttached::None);
}

OperationAttached::Status OperationAttached::inputsStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (const NodeBase* input, node()->inputs()) {
        const OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        OperationAttached::Status inputsStatus = inputAttached->status();
        if (inputsStatus > status)
            status = inputsStatus;
    }

    return status;
}

OperationAttached::Status OperationAttached::childrenStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (QObject* o, node()->children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child && child->isOutput()) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            OperationAttached::Status childStatus = childAttached->status();
            if (childStatus > status)
                status = childStatus;
        }
    }

    return status;
}

OperationAttached::Status OperationAttached::extraStatus() const
{
    return OperationAttached::Invalid;
}

OperationAttached::Status OperationAttached::dependenciesStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    OperationAttached::Status is = inputsStatus();

    if (is > status)
        status = is;

    OperationAttached::Status es = extraStatus();

    if (es > status)
        status = es;

    return status;
}

void OperationAttached::resetInputsStatus()
{
    // reset inputs
    foreach (NodeBase* input, node()->inputs()) {
        OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        inputAttached->resetAllStatus();
    }
}

void OperationAttached::resetChildrenStatus()
{
    // reset children
    foreach (QObject* o, node()->children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child && child->isOutput()) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            childAttached->resetAllStatus();
        }
    }
}

void OperationAttached::resetExtraStatus()
{
}

void OperationAttached::resetAllStatus()
{
    // already reset?
    if (status() == OperationAttached::None)
        return;

    debug() << node() << "resetAllStatus";

    // reset ourself
    resetStatus();

    resetInputsStatus();
    resetChildrenStatus();
    resetExtraStatus();
}

void OperationAttached::resetInputs()
{
    // reset inputs
    foreach (NodeBase* input, node()->inputs()) {
        OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        inputAttached->resetAll();
    }
}

void OperationAttached::resetChildren()
{
    // reset children
    foreach (QObject* o, node()->children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child && child->isOutput()) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            childAttached->resetAll();
        }
    }
}

void OperationAttached::resetExtra()
{
}

void OperationAttached::resetAll()
{
    // already reset?
    if (status() == OperationAttached::None)
        return;

    debug() << node() << "resetAll";

    // reset ourself
    reset();

    resetInputs();
    resetChildren();
    resetExtra();
}

void OperationAttached::run(Operation *op, const QVariant env)
{
    debug() << node() << ".run" << op;
    Q_ASSERT(op);

    // possible values of status:
    //   - None: has not been run since reset
    //   - Idle: run already called by another dependency
    //   - Finished: finished by another index
    Q_ASSERT(status() == OperationAttached::None || status() == OperationAttached::Idle || status() == OperationAttached::Finished);
    setStatus(OperationAttached::Idle);

    setEnv(env.toMap());
    m_operation = op;

    continueRunning();
}

void OperationAttached::continueRunning()
{
    Q_ASSERT(m_operation);

    debug() << node() << ".continueRunning";

    OperationAttached::Status depStatus = dependenciesStatus();
    OperationAttached::Status childStatus = childrenStatus();
    debug() << node() << "-- depStatus" << depStatus << "status" << status() << "childStatus" << childStatus;

    switch (depStatus) {
    case OperationAttached::Running:
        break;

    case OperationAttached::Idle:
    case OperationAttached::None:
        // dependencies haven't been run yet
        runDependencies();
        break;

    case OperationAttached::Error:
        // error in dependencies
        debug() << node() << ".continueRunning, depStatus is error, emitting finished";
        setStatus(OperationAttached::Error);
        emit finished(this);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        // no dependencies or finished
        switch (status()) {
        case OperationAttached::Running:
            // nothing to do
            break;

        case OperationAttached::None:
            // should never get here, since our status is set to idle before we're called for the first time
            Q_ASSERT(false);
            break;

        case OperationAttached::Idle:
            setStatus(OperationAttached::Running);
            debug() << node() << "calling run()";
            run();
            break;

        case OperationAttached::Error:
            debug() << node() << ".continueRunning, status is error, emitting finished";
            emit finished(this);
            break;

        case OperationAttached::Finished:
            switch (childStatus) {
            case OperationAttached::Running:
                // nothing to do;
                break;

            case OperationAttached::Idle:
            case OperationAttached::None:
                runChildren();
                break;

            case OperationAttached::Error:
                debug() << node() << ".continueRunning, childStatus is error, emitting finished";
                setStatus(OperationAttached::Error);
                emit finished(this);
                break;

            case OperationAttached::Invalid:
            case OperationAttached::Finished:
                debug() << node() << ".continueRunning, OK, emitting finished";
                emit finished(this);
                break;
            }
            break;

        case OperationAttached::Invalid:
            // we should never get here
            Q_ASSERT(false);
            break;
        }
        break;
    }
}

void OperationAttached::runInputs()
{
    Q_ASSERT(m_operation);

    debug() << node() << "runInputs";

    // run inputs and connect their finished signal to ourself
    foreach (NodeBase* input, node()->inputs()) {
        debug() << "got" << input;
        OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());

        OperationAttached::Status inputStatus = inputAttached->status();

        if (inputStatus == OperationAttached::None || inputStatus == OperationAttached::Idle || inputStatus == OperationAttached::Running) {
            connect(inputAttached, SIGNAL(finished(OperationAttached *)),
                    this, SLOT(onInputFinished(OperationAttached *)));
        }

        if (inputStatus == OperationAttached::None || inputStatus == OperationAttached::Idle)
            inputAttached->run(m_operation, m_env);
    }
}

void OperationAttached::runChildren()
{
    Q_ASSERT(m_operation);

    debug() << node() << "runChildren";

    // run children with outputs flag and connect their finished signal to ourself
    foreach (QObject* o, node()->children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child && child->isOutput()) {
            debug() << "got" << child;
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());

            OperationAttached::Status childStatus = childAttached->status();

            if (childStatus == OperationAttached::None || childStatus == OperationAttached::Idle || childStatus == OperationAttached::Running) {
                connect(childAttached, SIGNAL(finished(OperationAttached *)),
                        this, SLOT(onChildFinished(OperationAttached *)));
            }

            if (childStatus == OperationAttached::None || childStatus == OperationAttached::Idle)
                childAttached->run(m_operation, m_env);
        }
    }
}

void OperationAttached::runExtra()
{
}

void OperationAttached::runDependencies()
{
    Q_ASSERT(m_operation);
    // TODO: check for cycles

    runInputs();
    runExtra();
}

void OperationAttached::onInputFinished(OperationAttached *attached)
{
    debug() << node() << operation() << ".onInputFinished" << attached->node();
    disconnect(attached, SIGNAL(finished(OperationAttached *)),
               this, SLOT(onInputFinished(OperationAttached *)));
    // continue cooking
    continueRunning();
}

void OperationAttached::onChildFinished(OperationAttached *attached)
{
    debug() << node() << operation() << ".onChildFinished" << attached->node();
    disconnect(attached, &OperationAttached::finished, this, &OperationAttached::onChildFinished);
    // continue cooking
    continueRunning();
}

void OperationAttached::onExtraFinished(OperationAttached *attached)
{
    debug() << node() << operation() << ".onExtraFinished" << attached->node();
    disconnect(attached, SIGNAL(finished(OperationAttached *)),
               this, SLOT(onExtraFinished(OperationAttached *)));
    // continue cooking
    continueRunning();
}

Operation::Operation(QObject *parent) :
    PugItem(parent),
    m_runningNode(),
    m_status(OperationAttached::Invalid)
{
}

QQmlListProperty<PugItem> Operation::data_()
{
    return QQmlListProperty<PugItem>(this, 0, Operation::data_append,
                                        Operation::data_count,
                                        Operation::data_at,
                                        Operation::data_clear);
}

QQmlListProperty<Operation> Operation::dependencies_()
{
    return QQmlListProperty<Operation>(this, m_dependencies);
}

QQmlListProperty<Operation> Operation::triggers_()
{
    return QQmlListProperty<Operation>(this, m_triggers);
}

// data property
void Operation::data_append(QQmlListProperty<PugItem> *prop, PugItem *item)
{
    if (!item)
        return;

    NodeBase *that = static_cast<NodeBase *>(prop->object);

    item->setParent(that);
}

int Operation::data_count(QQmlListProperty<PugItem> *prop)
{
    Q_UNUSED(prop);
    return 0;
}

PugItem *Operation::data_at(QQmlListProperty<PugItem> *prop, int i)
{
    Q_UNUSED(prop);
    Q_UNUSED(i);
    return 0;
}

void Operation::data_clear(QQmlListProperty<PugItem> *prop)
{
    Q_UNUSED(prop)
}

OperationAttached::Status Operation::status() const
{
    return m_status;
}

void Operation::setStatus(OperationAttached::Status s)
{
    if (m_status != s) {
        m_status = s;
        emit statusChanged(s);
    }
}

void Operation::resetAll(NodeBase *node)
{
    // TODO: cycle detection

    // reset dependencies
    foreach (Operation *op, m_dependencies) {
        op->resetAll(node);
    }

    // reset ourself
    OperationAttached *attached = node->attachedPropertiesObject<OperationAttached>(metaObject());
    Q_ASSERT(attached);
    attached->resetAll();
    setStatus(OperationAttached::None);

    // reset triggers
    foreach (Operation *op, m_triggers) {
        op->resetAll(node);
    }
}

void Operation::run(NodeBase *node, const QVariant env)
{
    if (!node)
        return;

    // ready...
    resetAll(node);

    Q_ASSERT(status() == OperationAttached::None);

    // onward!
    startRunning(node, env);
}

void Operation::startRunning(NodeBase *node, const QVariant env)
{
    debug() << ".startRunning" << node;
    Q_ASSERT(node);
    Q_ASSERT(status() == OperationAttached::None);

    // stash our running node and env to pass to dependencies and triggers
    m_runningNode = node;
    m_runningEnv = env;

    continueRunning();
}

OperationAttached::Status Operation::dependenciesStatus() const {
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (Operation *op, m_dependencies) {
        OperationAttached::Status depStatus = op->status();

        if (depStatus > status)
            status = depStatus;
    }

    return status;
}

void Operation::runDependencies()
{
    debug() << ".runDependencies";
    // TODO: check for cycles in triggers and dependencies

    foreach (Operation *op, m_dependencies) {
        OperationAttached::Status status = op->status();

        if (status == OperationAttached::None || status == OperationAttached::Running) {
            connect(op, SIGNAL(finished(OperationAttached::Status)),
                    this, SLOT(onDependencyFinished()));
        }

        if (status == OperationAttached::None)
            op->startRunning(m_runningNode, m_runningEnv);
    }
}

OperationAttached::Status Operation::triggersStatus() const {
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (Operation *op, m_triggers) {
        OperationAttached::Status triggerStatus = op->status();

        if (triggerStatus > status)
            status = triggerStatus;
    }

    return status;
}

void Operation::runTriggers()
{
    debug() << ".runTriggers";
    // TODO: check for cycles in triggers and dependencies

    foreach (Operation *op, m_triggers) {
        if (op->status() == OperationAttached::None || op->status() == OperationAttached::Running) {
            connect(op, SIGNAL(finished(OperationAttached::Status)),
                    this, SLOT(onTriggerFinished()));
        }

        if (op->status() == OperationAttached::None)
            op->startRunning(m_runningNode, m_runningEnv);
    }
}

void Operation::continueRunning()
{
    debug() << ".continueRunning";
    OperationAttached::Status depStatus = dependenciesStatus();

    debug() << "-- depStatus" << depStatus;

    OperationAttached *attached = m_runningNode->attachedPropertiesObject<OperationAttached>(metaObject());

    OperationAttached::Status trigStatus = triggersStatus();


    switch (depStatus) {
    case OperationAttached::None:
        // dependencies have not run
        runDependencies();
        break;

    case OperationAttached::Idle:
        // we should never get here
        Q_ASSERT(false);
        break;

    case OperationAttached::Running:
        // nothing to do
        break;

    case OperationAttached::Error:
        // failed!
        m_runningNode = 0;
        m_runningEnv.clear();
        debug() << "depStatus is error, emitting finished";
        emit finished(OperationAttached::Error);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        // no dependencies or dependencies finished
        debug() << "-- status" << status();
        switch (status()) {
        case OperationAttached::Idle:
        case OperationAttached::Invalid:
            // we should never get here
            Q_ASSERT(false);
            break;

        case OperationAttached::None:
            // run this operation
            debug() << "running" << attached;
            setStatus(OperationAttached::Running);

            // reset the attachment statuses
            // we do this right before we run it, since the same attachment object can be used more than once
            attached->resetAllStatus();

            connect(attached, SIGNAL(finished(OperationAttached*)),
                    this, SLOT(onFinished(OperationAttached*)));
            Q_ASSERT(this);
            attached->run(this, m_runningEnv);
            break;

        case OperationAttached::Running:
            // nothing to do
            break;

        case OperationAttached::Error:
            // failed!
            m_runningNode = 0;
            m_runningEnv.clear();
            debug() << "status is error, emitting finished";
            emit finished(OperationAttached::Error);
            break;

        case OperationAttached::Finished:
            debug() << "-- trigStatus" << trigStatus;
            switch (trigStatus) {
            case OperationAttached::None:
                // run any pending triggers
                runTriggers();
                break;

            case OperationAttached::Idle:
                // we should never get here
                Q_ASSERT(false);
                break;

            case OperationAttached::Running:
                // nothing to do
                break;

            case OperationAttached::Error:
                // failed!
                m_runningNode = 0;
                m_runningEnv.clear();
                debug() << "trigStatus is error, emitting finished";
                emit finished(OperationAttached::Error);
                break;

            case OperationAttached::Invalid:
            case OperationAttached::Finished:
                // no triggers or triggers finished
                m_runningNode = 0;
                m_runningEnv.clear();
                debug() << "trigStatus is" << trigStatus << "emitting finished";
                emit finished(OperationAttached::Finished);
                break;
            }
            break;
        }
        break;
    }
}

void Operation::onDependencyFinished()
{
    Operation *op = qobject_cast<Operation *>(QObject::sender());
    debug() << "onDependencyFinished" << op;
    if (op) {
        disconnect(op, SIGNAL(finished(OperationAttached::Status)),
                   this, SLOT(onDependencyFinished()));

        continueRunning();
    } else {
        warning() << "WTF";
    }
}

void Operation::onTriggerFinished()
{
    Operation *op = qobject_cast<Operation *>(QObject::sender());
    debug() << "onTriggerFinished" << op;
    if (op) {
        disconnect(op, SIGNAL(finished(OperationAttached::Status)),
                   this, SLOT(onTriggerFinished()));

        continueRunning();
    } else {
        warning() << "WTF";
    }
}

void Operation::onFinished(OperationAttached *attached)
{
    debug() << attached << attached->node() << "finished";
    disconnect(attached, SIGNAL(finished(OperationAttached*)),
               this, SLOT(onFinished(OperationAttached*)));

    setStatus(attached->status());

    continueRunning();
}