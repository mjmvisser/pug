#include "operation.h"
#include "node.h"
#include "root.h"
#include "logger.h"

OperationAttached::OperationAttached(QObject *parent) :
    PugItem(parent),
    m_status(OperationAttached::Invalid),
    m_operation(),
    m_targets(OperationAttached::Inputs|OperationAttached::Children|OperationAttached::Self)
{
    setLog(new Log(this));

    connect(this, &OperationAttached::prepareFinished, this, &OperationAttached::onPrepareFinished);
    connect(this, &OperationAttached::cookFinished, this, &OperationAttached::onCookFinished);
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

void OperationAttached::reset()
{
    log()->clear();
}

void OperationAttached::run()
{
    trace() << node() << ".run()";

    m_prepareCount = receivers(SIGNAL(prepare()));
    m_cookCount = receivers(SIGNAL(cook()));

    if (m_prepareCount > 0) {
        info() << "Preparing" << node() << "with" << context();
        node()->setLog(log());
        node()->setContext(context());
        emit prepare();
    } else if (m_cookCount > 0) {
        info() << "Cooking" << node() << "with" << context();
        node()->setLog(log());
        node()->setContext(context());
        emit cook();
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void OperationAttached::onPrepareFinished()
{
    m_prepareCount--;
    trace() << node() << ".onPrepareFinished() [" << m_prepareCount << "]";
    if (m_prepareCount == 0) {
        Q_ASSERT(status() == OperationAttached::Running);
        if (log()->maxLevel() < Log::Error) {
            if (m_cookCount > 0) {
                info() << "Cooking" << node() << "with" << context();
                node()->setLog(log());
                node()->setContext(context());
                emit cook();
                return;
            } else {
                // no cook receivers, we're done
                setStatus(OperationAttached::Finished);
            }
        } else {
            setStatus(OperationAttached::Error);
        }
        continueRunning();
    }
}

void OperationAttached::onCookFinished()
{
    m_cookCount--;
    trace() << node() << ".onCookFinished() [" << m_cookCount << "]";
    if (m_cookCount == 0) {
        Q_ASSERT(status() == OperationAttached::Running);

        if (log()->maxLevel() < Log::Error)
            setStatus(OperationAttached::Finished);
        else
            setStatus(OperationAttached::Error);

        info() << "Cooked" << node() << "with status" << status() << "details" << QJsonDocument::fromVariant(node()->details().toVariant()).toJson();

        node()->setLog(0);
        continueRunning();
    }
}

OperationAttached::Status OperationAttached::inputsStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    if (m_targets & OperationAttached::Inputs) {
        foreach (const Input* input, node()->inputs()) {
            foreach (const Node* n, input->nodes()) {
                const OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
                if (attached->status() > status)
                    status = attached->status();
            }
        }
    }

    return status;
}

OperationAttached::Status OperationAttached::childrenStatus() const
{
    OperationAttached::Status status = OperationAttached::Invalid;

    if (m_targets & OperationAttached::Children) {
        foreach (const Output *output, node()->outputs()) {
            foreach (const Node *n, output->nodes()) {
                const OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
                if (attached->status() > status)
                    status = attached->status();
            }
        }
    }

    return status;
}

void OperationAttached::resetInputsStatus()
{
    // reset inputs
    foreach (Input *input, node()->inputs()) {
        foreach (Node *n, input->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            attached->resetAllStatus();
        }
    }
}

void OperationAttached::resetChildrenStatus()
{
    // reset children
    foreach (Output *output, node()->outputs()) {
        foreach (Node *n, output->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            attached->resetAllStatus();
        }
    }
}

void OperationAttached::resetAllStatus()
{
    // already reset?
    if (m_status == OperationAttached::None)
        return;

    debug() << node() << ".resetAllStatus()";

    setStatus(OperationAttached::None);

    resetInputsStatus();
    resetChildrenStatus();
}

void OperationAttached::resetInputs(QSet<const OperationAttached *>& visited)
{
    foreach (Input* input, node()->inputs()) {
        foreach (Node* n, input->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            attached->resetAll(m_context, visited);
        }
    }
}

void OperationAttached::resetChildren(QSet<const OperationAttached *>& visited)
{
    foreach (Output* output, node()->outputs()) {
        foreach (Node* n, output->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());
            attached->resetAll(m_context, visited);
        }
    }
}

void OperationAttached::resetAll(const QVariantMap context, QSet<const OperationAttached *>& visited)
{
    if (visited.contains(this))
        return;
    else
        visited.insert(this);

    trace() << node() << ".resetAll(" << context << ")";

    debug() << "Resetting" << node();

    setContext(context);
    setStatus(OperationAttached::Invalid);

    resetInputs(visited);

    switch (node()->dependencyOrder()) {
    case Node::InputsChildrenSelf:
        resetChildren(visited);
        reset();
        break;

    case Node::InputsSelfChildren:
        reset();
        resetChildren(visited);
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void OperationAttached::run(Operation *op, OperationAttached::Targets targets)
{
    debug() << "running" << node() << "with" << op << static_cast<int>(targets);
    trace() << node() << ".run(" << op << "," << ")";
    Q_ASSERT(op);

    m_targets = targets;

    // check for cycles
    if (node()->cycleCheck()) {
        error() << "Cycle found on" << node();

        Node *p = node();
        while (p) {
            error() << p;
            p = qobject_cast<Node *>(p->QObject::parent());
        }

        setStatus(OperationAttached::Error);
        emit finished(this);
        return;
    }

    // possible values of status:
    //   - None: has not been run since reset
    //   - Idle: run already called by another input
    //   - Finished: finished by another index
    debug() << node() << "status is" << m_status;
    Q_ASSERT(m_status == OperationAttached::None);

    if (m_status == OperationAttached::None) {
        // we haven't been run yet
        debug() << node() <<  "setting status to" << OperationAttached::Idle;
        setStatus(OperationAttached::Idle);

        m_operation = op;

        dispatchInputs();
    }
}

void OperationAttached::continueRunning()
{
    trace() << node() << ".continueRunning()";
    Q_ASSERT(m_operation);

    dispatchInputs();
//
//    OperationAttached::Status inStatus = inputsStatus();
//    OperationAttached::Status childStatus = childrenStatus();
//    info() << node() << "inputsStatus:" << inStatus << "status:" << m_status << "childrenStatus:" << childStatus;
//
//    switch (inStatus) {
//    case OperationAttached::Running:
//        break;
//
//    case OperationAttached::Idle:
//    case OperationAttached::None:
//        // inputs haven't been run yet
//        runInputs();
//        break;
//
//    case OperationAttached::Error:
//        // error in inputs
//        setStatus(OperationAttached::Error);
//        emit finished(this);
//        break;
//
//    case OperationAttached::Invalid:
//    case OperationAttached::Finished:
//        // no inputs or finished
//        switch (m_status) {
//        case OperationAttached::Running:
//            // nothing to do
//            break;
//
//        case OperationAttached::None:
//            // should never get here, since our status is set to idle before we're called for the first time
//            Q_ASSERT(false);
//            break;
//
//        case OperationAttached::Idle:
//            debug() << node() << "setting status to" << OperationAttached::Running;
//            setStatus(OperationAttached::Running);
//            debug() << node() << "calling run()";
//            run();
//            break;
//
//        case OperationAttached::Error:
//            debug() << node()
//                    << ".continueRunning, status is error, emitting finished";
//            setStatus(OperationAttached::Error);
//            emit finished(this);
//            break;
//
//        case OperationAttached::Finished:
//            switch (childStatus) {
//            case OperationAttached::Running:
//                // nothing to do;
//                break;
//
//            case OperationAttached::Idle:
//            case OperationAttached::None:
//                runChildren();
//                break;
//
//            case OperationAttached::Error:
//                debug() << node()
//                        << ".continueRunning, childStatus is error, emitting finished";
//                setStatus(OperationAttached::Error);
//                emit finished(this);
//                break;
//
//            case OperationAttached::Invalid:
//            case OperationAttached::Finished:
//                debug() << node() << ".continueRunning, OK, emitting finished";
//                setStatus(OperationAttached::Finished);
//                emit finished(this);
//                break;
//            }
//            break;
//
//        case OperationAttached::Invalid:
//            // we should never get here
//            error() << node() << "status is invalid";
//            Q_ASSERT(false);
//            //setStatus(OperationAttached::Error);
//            //emit finished(this);
//            break;
//        }
//        break;
//    }
}

void OperationAttached::runInputs()
{
    trace() << node() << "runInputs()";
    Q_ASSERT(m_operation);

    debug() << node() << "running inputs";

    // run inputs and connect their finished signal to ourself
    foreach (Input *input, node()->inputs()) {
        foreach (Node *n, input->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());

            if (attached->status() == OperationAttached::None
                    || attached->status() == OperationAttached::Idle
                    || attached->status() == OperationAttached::Running)
            {
                connect(attached, &OperationAttached::finished,
                        this, &OperationAttached::onInputFinished);
            }

            if (attached->status() == OperationAttached::None) {
                debug() << node() << "running input" << n;
                attached->run(m_operation, m_targets);
            } else if (attached->status() == OperationAttached::Idle) {
                // input has already been run, so transfer control to it
                attached->continueRunning();
            }
        }
    }
}

void OperationAttached::runChildren()
{
    trace() << node() << ".runChildren()";
    Q_ASSERT(m_operation);

    debug() << node() << "running children";

    foreach (Output *output, node()->outputs()) {
        foreach (Node *n, output->nodes()) {
            OperationAttached *attached = n->attachedPropertiesObject<OperationAttached>(operationMetaObject());

            if (attached->status() == OperationAttached::None
                    || attached->status() == OperationAttached::Idle
                    || attached->status() == OperationAttached::Running)
            {
                connect(attached, &OperationAttached::finished,
                        this, &OperationAttached::onChildFinished);
            }

            if (attached->status() == OperationAttached::None) {
                debug() << node() << "running child" << n;
                attached->run(m_operation, m_targets);
            } else if (attached->status() == OperationAttached::Idle) {
                // child has already been run, so transfer control to it
                attached->continueRunning();
            }
        }
    }
}

void OperationAttached::onInputFinished(OperationAttached *attached)
{
    trace() << node() << ".onInputFinished(" << attached->node() << ")";
    debug() << attached->node() << "finished for" << node();
    disconnect(attached, &OperationAttached::finished,
               this, &OperationAttached::onInputFinished);

    dispatchInputs();
}

void OperationAttached::dispatchInputs()
{
    OperationAttached::Status inStatus = inputsStatus();

    debug() << node() << "inputs status is" << inStatus;

    switch (inStatus) {
    case OperationAttached::Running:
        // we'll get triggered again when another input finishes
        break;

    case OperationAttached::None:
    case OperationAttached::Idle:
        // continue running inputs
        debug() << "Running inputs for" << node();
        runInputs();
        break;

    case OperationAttached::Error:
        // error in inputs
        setStatus(OperationAttached::Error);
        emit finished(this);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        // no inputs or finished
        switch (node()->dependencyOrder()) {
        case Node::InputsChildrenSelf:
            dispatchChildren();
            break;

        case Node::InputsSelfChildren:
            dispatch();
            break;

        default:
            Q_ASSERT(false);
            break;
        }
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void OperationAttached::onChildFinished(OperationAttached *attached)
{
    trace() << node() << ".onChildFinished(" << attached->node() << ")";
    disconnect(attached, &OperationAttached::finished, this,
            &OperationAttached::onChildFinished);

    dispatchChildren();
}

void OperationAttached::dispatchChildren()
{
    OperationAttached::Status childStatus = childrenStatus();

    debug() << node() << "children status is" << childStatus;

    switch (childStatus) {
    case OperationAttached::Running:
        // we'll get run again when the next child finishes
        break;

    case OperationAttached::Idle:
    case OperationAttached::None:
        // continue running children
        debug() << "Running children for" << node();
        runChildren();
        break;

    case OperationAttached::Error:
        emit finished(this);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        switch (node()->dependencyOrder()) {
        case Node::InputsChildrenSelf:
            dispatch();
            break;

        case Node::InputsSelfChildren:
            emit finished(this);
            break;

        default:
            Q_ASSERT(false);
            break;
        }
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void OperationAttached::dispatch()
{
    debug() << node() << "status is" << m_status;

    switch (m_status) {
    case OperationAttached::Running:
        break;

    case OperationAttached::None:
        // should never get here, since our status is set to idle before we're called for the first time
        Q_ASSERT(false);
        break;

    case OperationAttached::Idle:
        debug() << "Running" << node();
        setStatus(OperationAttached::Running);
        run();
        break;

    case OperationAttached::Error:
        emit finished(this);
        break;

    case OperationAttached::Invalid:
    case OperationAttached::Finished:
        switch (node()->dependencyOrder()) {
        case Node::InputsChildrenSelf:
            emit finished(this);
            break;

        case Node::InputsSelfChildren:
            dispatchChildren();
            break;

        default:
            Q_ASSERT(false);
            break;
        }
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}


void OperationAttached::setContext(const QVariantMap context)
{
    if (m_context != context) {
        m_context = context;
        emit contextChanged(context);
    }
}

Operation::Operation(QObject *parent) :
        PugItem(parent),
        m_node(),
        m_status(OperationAttached::Invalid),
        m_targets(OperationAttached::Inputs|OperationAttached::Children|OperationAttached::Self)
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

const QList<const Operation *>& Operation::dependencies() const
{
    return reinterpret_cast<QList<const Operation *> const&>(m_dependencies);
}

const QList<Operation *>& Operation::dependencies()
{
    return m_dependencies;
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
    Q_ASSERT(node);
    trace() << ".resetAll(" << node << "," << context << ")";
    // TODO: cycle detection

    m_context = context;

    // reset dependencies
    foreach (Operation *op, m_dependencies) {
        op->resetAll(node, context);
    }

    QSet<const OperationAttached* > visited;

    OperationAttached *attached = node->attachedPropertiesObject<OperationAttached>(metaObject());
    Q_ASSERT(attached);
    attached->resetAll(context, visited);
}

void Operation::resetAllStatus(Node *node)
{
    trace() << ".resetAllStatus(" << node << ")";
    // TODO: cycle detection

    // reset dependencies statuses
    foreach (Operation *op, m_dependencies) {
        op->resetAllStatus(node);
    }

    // reset the node's statuses
    OperationAttached *attached = node->attachedPropertiesObject<OperationAttached>(metaObject());
    Q_ASSERT(attached);
    attached->resetAllStatus();
    setStatus(OperationAttached::None);
}

void Operation::run(Node *node, const QVariant context, bool reset, OperationAttached::Targets targets)
{
    debug() << "Running" << name() << "on" << node->path();
    trace() << ".run(" << node << "," << context << "," << reset << ")";
    if (!node) {
        emit finished(OperationAttached::Error);
        return;
    }

    // ready...
    if (reset) {
        resetAll(node, context.toMap());

        // reset all reachable OperationAttached statuses
        resetAllStatus(node);
    }

    Q_ASSERT(m_status == OperationAttached::None);

    // onward!
    startRunning(node, targets);
}

void Operation::startRunning(Node *node, OperationAttached::Targets targets)
{
    trace() << ".startRunning(" << node << ")";
    Q_ASSERT(node);
    Q_ASSERT(m_status == OperationAttached::None);

    m_node = node;
    m_targets = targets;

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
    // TODO: check for cycles in dependencies

    foreach (Operation * op, m_dependencies)
    {
        OperationAttached::Status status = op->status();

        if (status == OperationAttached::None
                || status == OperationAttached::Running) {
            connect(op, &Operation::finished,
                    this, &Operation::onDependencyFinished);
        }

        if (status == OperationAttached::None)
            op->startRunning(m_node, m_targets);
    }
}

void Operation::continueRunning()
{
    trace() << ".continueRunning()";
    OperationAttached::Status depStatus = dependenciesStatus();

    debug() << "-- depStatus" << depStatus;

    OperationAttached *attached = m_node->attachedPropertiesObject<OperationAttached>(metaObject());

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

            connect(attached, &OperationAttached::finished,
                    this, &Operation::onFinished);
            attached->run(this, m_targets);
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
            m_node = 0;
            m_context.clear();
            setStatus(OperationAttached::Finished);
            emit finished(OperationAttached::Finished);
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
        disconnect(op, &Operation::finished,
                   this, &Operation::onDependencyFinished);

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
        disconnect(op, &Operation::finished,
                   this, &Operation::onTriggerFinished);

        continueRunning();
    } else {
        warning() << "WTF";
    }
}

void Operation::onFinished(OperationAttached *attached)
{
    trace() << ".onFinished(" << attached << ")";
    disconnect(attached, &OperationAttached::finished,
               this, &Operation::onFinished);

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
