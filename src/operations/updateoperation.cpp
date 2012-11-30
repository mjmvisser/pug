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
        debug() << node() << "updating";
        emit update(env());
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void UpdateOperationAttached::onUpdated(int s)
{
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
