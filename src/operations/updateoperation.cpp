#include "updateoperation.h"
#include "node.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_updatable(false)
{
    setObjectName("update");
    bool haveUpdateSignal = node()->hasSignal(SIGNAL(update(const QVariant)));
    bool haveUpdateFinishedSignal = node()->hasSignal(SIGNAL(updateFinished(int)));

    if (haveUpdateSignal && haveUpdateFinishedSignal) {
        m_updatable = true;
        debug() << node() << "found update and updateFinished signals";

        connect(this, SIGNAL(update(const QVariant)),
                node(), SIGNAL(update(const QVariant)));
        connect(node(), SIGNAL(updateFinished(int)),
                this, SLOT(onUpdateFinished(int)));
    } else if (!haveUpdateSignal && haveUpdateFinishedSignal) {
        error() << node() << "is missing the update signal";
    } else if (haveUpdateSignal && !haveUpdateFinishedSignal) {
        error() << node() << "is missing the updateFinished signal";
    } else {
        debug() << node() << "is not updatable";
    }
}

void UpdateOperationAttached::run()
{
    trace() << node() << ".run()";

    if (m_updatable) {
        Q_ASSERT(receivers(SIGNAL(update(QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(updateFinished(int))) > 0);
        info() << "Updating" << node() << "with" << context();
        emit update(context());
    } else {
        debug() << node() << "skipping";
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void UpdateOperationAttached::onUpdateFinished(int s)
{
    trace() << node() << ".onUpdateFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    Q_ASSERT(status() == OperationAttached::Running);
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Updated" << node() << "with status" << status() << "details" << QJsonDocument::fromVariant(node()->details().toVariant()).toJson();

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
