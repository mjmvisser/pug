#include "updateoperation.h"
#include "node.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(UpdateOperationAttached::Skip)
{
    bool haveUpdateAtIndexSignal = node()->hasSignal(SIGNAL(updateAtIndex(int, const QVariant)));
    bool haveUpdatedAtIndexSignal = node()->hasSignal(SIGNAL(updatedAtIndex(int, int)));

    bool haveUpdateSignal = node()->hasSignal(SIGNAL(update(const QVariant)));
    bool haveUpdatedSignal = node()->hasSignal(SIGNAL(updated(int)));

    if (haveUpdateAtIndexSignal && haveUpdatedAtIndexSignal) {
        connect(this, SIGNAL(updateAtIndex(int, const QVariant)),
                node(), SIGNAL(updateAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(updatedAtIndex(int, int)),
                this, SLOT(onUpdatedAtIndex(int, int)));
        m_mode = UpdateOperationAttached::UpdateAtIndex;
        debug() << node() << "found .updateAtIndex";
    } else if (!haveUpdateAtIndexSignal && haveUpdatedAtIndexSignal) {
        error() << node() << "is missing the updateAtIndex signal";
    } else if (haveUpdateAtIndexSignal && !haveUpdatedAtIndexSignal) {
        error() << node() << "is missing the updatedAtIndex signal";
    } else if (haveUpdateSignal && haveUpdatedSignal) {
        connect(this, SIGNAL(update(const QVariant)),
                node(), SIGNAL(update(const QVariant)));
        connect(node(), SIGNAL(updated(int)),
                this, SLOT(onUpdated(int)));
        m_mode = UpdateOperationAttached::Update;
        debug() << node() << "found .update";
    } else if (!haveUpdateSignal && haveUpdatedSignal) {
        error() << node() << "is missing the update signal";
    } else if (haveUpdateSignal && !haveUpdatedSignal) {
        error() << node() << "is missing the updated signal";
    } else {
        debug() << node() << "is not updateable";
    }
}

void UpdateOperationAttached::run()
{
    info() << "Updating" << node() << "with" << context();
    trace() << node() << ".run() [update mode is" << m_mode << "]";

    m_indexStatus.clear();
    if (m_mode == UpdateOperationAttached::Update) {
        emit update(context());
    } else if (m_mode == UpdateOperationAttached::UpdateAtIndex) {
        if (node()->count() > 0) {
            debug() << "updating" << node()->count() << "details";
            for (int i = 0; i < node()->count(); i++) {
                emit updateAtIndex(i, context());
            }
        } else {
            warning() << node() << "has nothing to update";
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
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

void UpdateOperationAttached::onUpdatedAtIndex(int index, int s)
{
    trace() << node() << ".onUpdatedAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    if (m_indexStatus.length() == node()->count()) {
        onUpdated(m_indexStatus.status());
    }
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
