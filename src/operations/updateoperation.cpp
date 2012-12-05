#include "updateoperation.h"
#include "nodebase.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_updatableFlag(false)
{
    if (node()->hasSignal(SIGNAL(update(const QVariant))) &&
        node()->hasSignal(SIGNAL(updated(int))))
    {
        connect(this, SIGNAL(update(const QVariant)),
                node(), SIGNAL(update(const QVariant)));
        connect(node(), SIGNAL(updated(int)),
                this, SLOT(onUpdated(int)));
        m_updatableFlag = true;
        debug() << node() << "is updatable";
    } else if (node()->hasSignal(SIGNAL(update(const QVariant))) ||
               node()->hasSignal(SIGNAL(updated(int)))) {
        error() << node() << "is missing the update or updated signal";
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
    if (m_updatableFlag) {
        emit update(env());
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
