#include "updateoperation.h"
#include "nodebase.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_updatableFlag(false)
{
    bool haveUpdateSignal = node()->hasSignal(SIGNAL(update(const QVariant)));
    bool haveUpdatedSignal = node()->hasSignal(SIGNAL(updated(int)));

    if (haveUpdateSignal && haveUpdatedSignal) {
        connect(this, SIGNAL(update(const QVariant)),
                node(), SIGNAL(update(const QVariant)));
        connect(node(), SIGNAL(updated(int)),
                this, SLOT(onUpdated(int)));
        m_updatableFlag = true;
        debug() << node() << "is updatable";
    } else if (!haveUpdateSignal && haveUpdatedSignal) {
        error() << node() << "is missing the update signal";
    } else if (haveUpdateSignal && !haveUpdatedSignal) {
        error() << node() << "is missing the updated signal";
    } else {
        debug() << node() << "is not updatable";
    }
}

bool UpdateOperationAttached::isUpdatable() const
{
    return m_updatableFlag;
}

void UpdateOperationAttached::setUpdatable(bool c)
{
    if (m_updatableFlag != c) {
        m_updatableFlag = c;
        emit updatableChanged(m_updatableFlag);
    }
}

void UpdateOperationAttached::run()
{
    info() << "Updating" << node() << "with" << context();
    trace() << node() << ".update()";

    if (m_updatableFlag) {
        emit update(context());
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void UpdateOperationAttached::onUpdated(int s)
{
    Q_ASSERT(operation());
    setStatus(static_cast<OperationAttached::Status>(s));
    continueRunning();
}

const QMetaObject *UpdateOperationAttached::operationMetaObject() const
{
    return &UpdateOperation::staticMetaObject;
}

UpdateOperation::UpdateOperation(QObject *parent) :
    Operation(parent)
{
}

UpdateOperationAttached *UpdateOperation::qmlAttachedProperties(QObject *object)
{
    return new UpdateOperationAttached(object);
}
