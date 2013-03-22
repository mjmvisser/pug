#include "operation.h"
#include "node.h"

OperationAttached::OperationAttached(QObject *parent) :
        PugItem(parent), m_operation()
{
}

const QVariantMap OperationAttached::context() const
{
    return m_context;
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

const Node* OperationAttached::node() const
{
    return parent<Node>();
}

Node* OperationAttached::node()
{
    return parent<Node>();
}

void OperationAttached::resetStatus()
{
    trace() << node() << "resetStatus()";
    setStatus(OperationAttached::None);
}

void OperationAttached::reset()
{
    trace() << node() << "reset()";
    setStatus(OperationAttached::None);
}

OperationAttached::Status OperationAttached::inputsStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (const Node* input, node()->upstream()) {
        const OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        OperationAttached::Status inputStatus = inputAttached->status();
        trace() << "status of" << input << "is" << inputStatus;
        if (inputStatus > status)
            status = inputStatus;
    }

    trace() << node() << ".inputsStatus() ->" << status;

    return status;
}

OperationAttached::Status OperationAttached::childrenStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (QObject * o, node()->children()) {
        Node *child = qobject_cast<Node *>(o);
        if (child && child->isActive()) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            OperationAttached::Status childStatus = childAttached->status();
            if (childStatus > status)
                status = childStatus;
        }
    }

    trace() << node() << ".childrenStatus() ->" << status;

    return status;
}

void OperationAttached::resetInputsStatus()
{
    // reset inputs
    foreach (Node * input, node()->upstream()) {
        OperationAttached *inputAttached = input->attachedPropertiesObject<OperationAttached>(operationMetaObject());
        inputAttached->resetAllStatus();
    }
}

void OperationAttached::resetChildrenStatus()
{
    // reset children
    foreach (QObject * o, node()->children()) {
        Node *child = qobject_cast<Node *>(o);
        if (child && child->isActive()) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            childAttached->resetAllStatus();
        }
    }
}

void OperationAttached::resetAllStatus()
{
    // already reset?
    if (status() == OperationAttached::None)
        return;

    debug() << node() << ".resetAllStatus()";


    // reset ourself
    resetStatus();

    resetInputsStatus();
    resetChildrenStatus();
}

void OperationAttached::resetChildren()
{
    // reset children
    foreach (QObject * o, node()->children()) {
        Node *child = qobject_cast<Node *>(o);
        if (child) {
            OperationAttached *childAttached = child->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            childAttached->resetAll(m_context);
        }
    }
}

void OperationAttached::resetAll(const QVariantMap context)
{
    trace() << node() << ".resetAll(" << context << ")";

    info() << "Resetting" << node() << "to" << context;

    setContext(context);

    // reset ourself
    reset();

    resetChildren();
}

void OperationAttached::run(Operation *op)
{
    trace() << node() << ".run(" << op << "," << ")";
    Q_ASSERT(op);

    // possible values of status:
    //   - None: has not been run since reset
    //   - Idle: run already called by another input
    //   - Finished: finished by another index
    Q_ASSERT(status() == OperationAttached::None
            || status() == OperationAttached::Idle
            || status() == OperationAttached::Finished);

    if (status() == OperationAttached::None) {
        // we haven't been run yet
        debug() << node() <<  "setting status to" << OperationAttached::Idle;
        setStatus(OperationAttached::Idle);

        m_operation = op;
    }

    continueRunning();
}

void OperationAttached::continueRunning()
{
    trace() << node() << ".continueRunning()";
    Q_ASSERT(m_operation);

    OperationAttached::Status inStatus = inputsStatus();
    OperationAttached::Status childStatus = childrenStatus();
    debug() << node() << "-- inStatus" << inStatus << "status" << status()
            << "childStatus" << childStatus;

    switch (inStatus) {
    case OperationAttached::Running:
        break;

    case OperationAttached::Idle:
    case OperationAttached::None:
        // inputs haven't been run yet
        runInputs();
        break;

    case OperationAttached::Error:
        // error in inputs
        debug() << node()
                << ".continueRunning, inStatus is error, emitting finished";
        setStatus(OperationAttached::Error);
        emit finished(this);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        // no inputs or finished
        switch (status()) {
        case OperationAttached::Running:
            // nothing to do
            break;

        case OperationAttached::None:
            // should never get here, since our status is set to idle before we're called for the first time
            Q_ASSERT(false);
            break;

        case OperationAttached::Idle:
            debug() << node() << "setting status to" << OperationAttached::Running;
            setStatus(OperationAttached::Running);
            debug() << node() << "calling run()";
            run();
            break;

        case OperationAttached::Error:
            debug() << node()
                    << ".continueRunning, status is error, emitting finished";
            setStatus(OperationAttached::Error);
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
                debug() << node()
                        << ".continueRunning, childStatus is error, emitting finished";
                setStatus(OperationAttached::Error);
                emit finished(this);
                break;

            case OperationAttached::Invalid:
            case OperationAttached::Finished:
                debug() << node() << ".continueRunning, OK, emitting finished";
                setStatus(OperationAttached::Finished);
                emit finished(this);
                break;
            }
            break;

        case OperationAttached::Invalid:
            // we should never get here
            error() << node() << "status is invalid";
            Q_ASSERT(false);
            //setStatus(OperationAttached::Error);
            //emit finished(this);
            break;
        }
        break;
    }
}

void OperationAttached::runInputs()
{
    trace() << node() << "runInputs()";
    Q_ASSERT(m_operation);

    // run inputs and connect their finished signal to ourself
    foreach (Node * input, node()->upstream()) {
        OperationAttached *inputAttached = input->attachedPropertiesObject
                < OperationAttached > (operationMetaObject());

        OperationAttached::Status inputStatus = inputAttached->status();

        debug() << node() << "input" << input << "has status" << inputStatus;

        if (inputStatus == OperationAttached::None
                || inputStatus == OperationAttached::Idle
                || inputStatus == OperationAttached::Running) {
            connect(inputAttached, SIGNAL(finished(OperationAttached *)),
                    this, SLOT(onInputFinished(OperationAttached *)));
        }

        if (inputStatus == OperationAttached::None
                || inputStatus == OperationAttached::Idle) {
            debug() << node() << "running input" << input;
            inputAttached->run(m_operation);
        }
    }
}

void OperationAttached::runChildren()
{
    trace() << node() << ".runChildren()";
    Q_ASSERT(m_operation);

    // run children with active flag and connect their finished signal to ourself
    foreach (QObject * o, node()->children())
    {
        Node *child = qobject_cast<Node *>(o);
        if (child && child->isActive()) {
            debug() << "got" << child;
            OperationAttached *childAttached = child->attachedPropertiesObject<
                    OperationAttached>(operationMetaObject());

            OperationAttached::Status childStatus = childAttached->status();

            if (childStatus == OperationAttached::None
                    || childStatus == OperationAttached::Idle
                    || childStatus == OperationAttached::Running) {
                connect(childAttached, SIGNAL(finished(OperationAttached *)),
                        this, SLOT(onChildFinished(OperationAttached *)));
            }

            if (childStatus == OperationAttached::None
                    || childStatus == OperationAttached::Idle)
                childAttached->run(m_operation);
        }
    }
}

void OperationAttached::onInputFinished(OperationAttached *attached)
{
    trace() << node() << ".onInputFinished(" << attached->node() << ")";
    disconnect(attached, SIGNAL(finished(OperationAttached *)),
            this, SLOT(onInputFinished(OperationAttached *)));
    // continue cooking
    continueRunning();
}

void OperationAttached::onChildFinished(OperationAttached *attached)
{
    trace() << node() << ".onChildFinished(" << attached->node() << ")";
    disconnect(attached, &OperationAttached::finished, this,
            &OperationAttached::onChildFinished);
    // continue cooking
    continueRunning();
}

void OperationAttached::setContext(const QVariantMap context)
{
    if (m_context != context) {
        m_context = context;
        emit contextChanged(context);
    }
}

Operation::Operation(QObject *parent) :
        PugItem(parent), m_node(), m_status(OperationAttached::Invalid)
{
}

QQmlListProperty<PugItem> Operation::data_()
{
    return QQmlListProperty<PugItem>(this, 0, Operation::data_append,
            Operation::data_count, Operation::data_at, Operation::data_clear);
}

QQmlListProperty<Operation> Operation::dependencies_()
{
    return QQmlListProperty<Operation>(this, m_dependencies);
}

QQmlListProperty<Operation> Operation::triggers_()
{
    return QQmlListProperty<Operation>(this, m_triggers);
}

const QList<const Operation *>& Operation::dependencies() const
{
    return reinterpret_cast<QList<const Operation *> const&>(m_dependencies);
}

const QList<Operation *>& Operation::dependencies()
{
    return m_dependencies;
}

const QList<const Operation *>& Operation::triggers() const
{
    return reinterpret_cast<QList<const Operation *> const&>(m_triggers);
}

const QList<Operation *>& Operation::triggers()
{
    return m_triggers;
}

// data property
void Operation::data_append(QQmlListProperty<PugItem> *prop, PugItem *item)
{
    if (!item)
        return;

    Node *that = static_cast<Node *>(prop->object);

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

void Operation::resetAll(Node *node, const QVariantMap context)
{
    trace() << ".resetAll(" << node << "," << context << ")";
    // TODO: cycle detection

    m_context = context;

    // reset dependencies
    foreach (Operation * op, m_dependencies)
    {
        op->resetAll(node, context);
    }

    // reset the whole tree
    OperationAttached *rootAttached = node->rootBranch()->attachedPropertiesObject<OperationAttached>(metaObject());
    Q_ASSERT(rootAttached);
    rootAttached->resetAll(context);
    setStatus(OperationAttached::None);

    // reset triggers
    foreach (Operation * op, m_triggers)
    {
        op->resetAll(node, context);
    }
}

void Operation::run(Node *node, const QVariant context, bool reset)
{
    info() << "Running" << name() << "on" << node->path();
    trace() << ".run(" << node << "," << context << "," << reset << ")";
    if (!node)
        return;

    // ready...
    if (reset)
        resetAll(node, context.toMap());

    Q_ASSERT(status() == OperationAttached::None);

    // onward!
    startRunning(node);
}

void Operation::startRunning(Node *node)
{
    trace() << ".startRunning(" << node << ")";
    Q_ASSERT(node);
    Q_ASSERT(status() == OperationAttached::None);

    // stash our running node and context to pass to dependencies and triggers
    m_node = node;

    continueRunning();
}

OperationAttached::Status Operation::dependenciesStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (Operation * op, m_dependencies)
    {
        OperationAttached::Status depStatus = op->status();

        if (depStatus > status)
            status = depStatus;
    }

    return status;
}

void Operation::runDependencies()
{
    trace() << ".runDependencies()";
    // TODO: check for cycles in triggers and dependencies

    foreach (Operation * op, m_dependencies)
    {
        OperationAttached::Status status = op->status();

        if (status == OperationAttached::None
                || status == OperationAttached::Running) {
            connect(op, SIGNAL(finished(OperationAttached::Status)), this,
                    SLOT(onDependencyFinished()));
        }

        if (status == OperationAttached::None)
            op->startRunning(m_node);
    }
}

OperationAttached::Status Operation::triggersStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    foreach (Operation * op, m_triggers)
    {
        OperationAttached::Status triggerStatus = op->status();

        if (triggerStatus > status)
            status = triggerStatus;
    }

    return status;
}

void Operation::runTriggers()
{
    trace() << ".runTriggers()";
    // TODO: check for cycles in triggers and dependencies

    foreach (Operation * op, m_triggers)
    {
        if (op->status() == OperationAttached::None
                || op->status() == OperationAttached::Running) {
            connect(op, SIGNAL(finished(OperationAttached::Status)), this,
                    SLOT(onTriggerFinished()));
        }

        if (op->status() == OperationAttached::None)
            op->startRunning(m_node);
    }
}

void Operation::continueRunning()
{
    trace() << ".continueRunning()";
    OperationAttached::Status depStatus = dependenciesStatus();

    debug() << "-- depStatus" << depStatus;

    OperationAttached *attached = m_node->attachedPropertiesObject<
            OperationAttached>(metaObject());

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
        m_node = 0;
        m_context.clear();
        debug() << "depStatus is error, emitting finished";
        setStatus(OperationAttached::Error);
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
            debug() << node() <<  "setting status to running" << attached;
            setStatus(OperationAttached::Running);

            // reset the attachment statuses
            // we do this right before we run it, since the same attachment object can be used more than once
            attached->resetAllStatus();

            connect(attached, SIGNAL(finished(OperationAttached*)),
                    this, SLOT(onFinished(OperationAttached*)));
            Q_ASSERT(this);
            attached->run(this);
            break;

        case OperationAttached::Running:
            // nothing to do
            break;

        case OperationAttached::Error:
            // failed!
            m_node = 0;
            m_context.clear();
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
                m_node = 0;
                m_context.clear();
                debug() << "trigStatus is error, emitting finished";
                emit finished(OperationAttached::Error);
                break;

            case OperationAttached::Invalid:
            case OperationAttached::Finished:
                // no triggers or triggers finished
                m_node = 0;
                m_context.clear();
                debug() << "trigStatus is" << trigStatus << "emitting finished";
                setStatus(OperationAttached::Finished);
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
    trace() << ".onDependencyFinished() [signal from" << op << "]";
    if (op) {
        disconnect(op, SIGNAL(finished(OperationAttached::Status)), this,
                SLOT(onDependencyFinished()));

        continueRunning();
    } else {
        warning() << "WTF";
    }
}

void Operation::onTriggerFinished()
{
    Operation *op = qobject_cast<Operation *>(QObject::sender());
    trace() << ".onTriggerFinished() [signal from" << op << "]";
    if (op) {
        disconnect(op, SIGNAL(finished(OperationAttached::Status)), this,
                SLOT(onTriggerFinished()));

        continueRunning();
    } else {
        warning() << "WTF";
    }
}

void Operation::onFinished(OperationAttached *attached)
{
    trace() << ".onFinished(" << attached << ")";
    disconnect(attached, SIGNAL(finished(OperationAttached*)),
            this, SLOT(onFinished(OperationAttached*)));

    debug() << node() <<  "onFinished, setting status to" << attached->status();
    setStatus(attached->status());

    continueRunning();
}

Node *Operation::node()
{
    return m_node;
}

const Node *Operation::node() const
{
    return m_node;
}
