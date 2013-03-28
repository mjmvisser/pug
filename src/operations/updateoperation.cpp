#include "updateoperation.h"
#include "node.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(UpdateOperationAttached::Skip)
{
    setObjectName("update");
    bool haveUpdateAtIndexSignal = node()->hasSignal(SIGNAL(updateAtIndex(int, const QVariant)));
    bool haveUpdatedAtIndexSignal = node()->hasSignal(SIGNAL(updatedAtIndex(int, int)));

    bool haveUpdateSignal = node()->hasSignal(SIGNAL(update(const QVariant)));
    bool haveUpdatedSignal = node()->hasSignal(SIGNAL(updated(int)));

    if (haveUpdateAtIndexSignal && haveUpdatedAtIndexSignal) {
        m_mode = UpdateOperationAttached::UpdateAtIndex;
        debug() << node() << "found updateAtIndex and updatedAtIndex signals";

        connect(this, SIGNAL(updateAtIndex(int, const QVariant)),
                node(), SIGNAL(updateAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(updatedAtIndex(int, int)),
                this, SLOT(onUpdatedAtIndex(int, int)));

        if (haveUpdateSignal) {
            debug() << node() << "found update signal";
            connect(this, SIGNAL(update(const QVariant)),
                    node(), SIGNAL(update(const QVariant)));
        }

        if (haveUpdatedSignal) {
            warning() << node() << "ignoring updated signal";
        }

    } else if (!haveUpdateAtIndexSignal && haveUpdatedAtIndexSignal) {
        error() << node() << "is missing the updateAtIndex signal";
    } else if (haveUpdateAtIndexSignal && !haveUpdatedAtIndexSignal) {
        error() << node() << "is missing the updatedAtIndex signal";
    } else if (haveUpdateSignal && haveUpdatedSignal) {
        m_mode = UpdateOperationAttached::Update;
        debug() << node() << "found update and updated signals";

        connect(this, SIGNAL(update(const QVariant)),
                node(), SIGNAL(update(const QVariant)));
        connect(node(), SIGNAL(updated(int)),
                this, SLOT(onUpdated(int)));
    } else if (!haveUpdateSignal && haveUpdatedSignal) {
        error() << node() << "is missing the update signal";
    } else if (haveUpdateSignal && !haveUpdatedSignal) {
        error() << node() << "is missing the updated signal";
    } else {
        debug() << node() << "is not updateable";
    }
}

void UpdateOperationAttached::reset()
{
    node()->clearDetails();
}

void UpdateOperationAttached::run()
{
    trace() << node() << ".run() [update mode is" << m_mode << "]";

    m_indexStatus.clear();
    m_updatedAtIndexCount = 0;
    if (m_mode == UpdateOperationAttached::Update) {
        Q_ASSERT(receivers(SIGNAL(update(QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(updated(int))) > 0);
        info() << "Updating" << node() << "with" << context();
        emit update(context());
    } else if (m_mode == UpdateOperationAttached::UpdateAtIndex) {
        Q_ASSERT(receivers(SIGNAL(updateAtIndex(int, QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(updatedAtIndex(int, int))) > 0);
        debug() << node() << "updating" << node()->count() << "details";
        if (node()->count() > 0) {
            emit update(context());
            for (int i = 0; i < node()->count(); i++) {
                info() << "Updating" << node() << "at index" << i << "with" << context();
                debug() << node() << "emitting updateAtIndex" << i << context();
                emit updateAtIndex(i, context());
            }
        } else {
            warning() << node() << "has nothing to update";
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
    } else {
        debug() << node() << "skipping";
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void UpdateOperationAttached::onUpdated(int s)
{
    trace() << node() << ".onUpdated(" << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Updated" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void UpdateOperationAttached::onUpdatedAtIndex(int index, int s)
{
    trace() << node() << ".onUpdatedAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Updated" << node() << "at index" << m_updatedAtIndexCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_updatedAtIndexCount++;

    if (m_updatedAtIndexCount == node()->count() * receivers(SIGNAL(updateAtIndex(int,QVariant)))) {
        info() << "Updated" << node() << "with status" << status();
        info() << "Result is" << node()->details().toVariant();
        setStatus(m_indexStatus.status());
        continueRunning();
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
