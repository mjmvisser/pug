#include "updateoperation.h"
#include "node.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(UpdateOperationAttached::Skip)
{
    setObjectName("update");
    bool haveUpdateAtIndexSignal = node()->hasSignal(SIGNAL(updateAtIndex(int, const QVariant)));
    bool haveUpdateAtIndexFinishedSignal = node()->hasSignal(SIGNAL(updateAtIndexFinished(int, int)));

    bool haveUpdateSignal = node()->hasSignal(SIGNAL(update(const QVariant)));
    bool haveUpdateFinishedSignal = node()->hasSignal(SIGNAL(updateFinished(int)));

    if (haveUpdateAtIndexSignal && haveUpdateAtIndexFinishedSignal) {
        m_mode = UpdateOperationAttached::UpdateAtIndex;
        debug() << node() << "found updateAtIndex and updateAtIndexFinished signals";

        connect(this, SIGNAL(updateAtIndex(int, const QVariant)),
                node(), SIGNAL(updateAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(updateAtIndexFinished(int, int)),
                this, SLOT(onUpdateAtIndexFinished(int, int)));

        if (haveUpdateSignal) {
            debug() << node() << "found update signal";
            connect(this, SIGNAL(update(const QVariant)),
                    node(), SIGNAL(update(const QVariant)));
        }

        if (haveUpdateFinishedSignal) {
            warning() << node() << "ignoring updateFinished signal";
        }

    } else if (!haveUpdateAtIndexSignal && haveUpdateAtIndexFinishedSignal) {
        error() << node() << "is missing the updateAtIndex signal";
    } else if (haveUpdateAtIndexSignal && !haveUpdateAtIndexFinishedSignal) {
        error() << node() << "is missing the updateAtIndexFinished signal";
    } else if (haveUpdateSignal && haveUpdateFinishedSignal) {
        m_mode = UpdateOperationAttached::Update;
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
    m_updateAtIndexFinishedCount = 0;
    if (m_mode == UpdateOperationAttached::Update) {
        Q_ASSERT(receivers(SIGNAL(update(QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(updateFinished(int))) > 0);
        info() << "Updating" << node() << "with" << context();
        emit update(context());
    } else if (m_mode == UpdateOperationAttached::UpdateAtIndex) {
        Q_ASSERT(receivers(SIGNAL(updateAtIndex(int, QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(updateAtIndexFinished(int, int))) > 0);
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

void UpdateOperationAttached::onUpdateFinished(int s)
{
    trace() << node() << ".onUpdateFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "UpdateFinished" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void UpdateOperationAttached::onUpdateAtIndexFinished(int index, int s)
{
    trace() << node() << ".onUpdateAtIndexFinished(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "UpdateFinished" << node() << "at index" << m_updateAtIndexFinishedCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_updateAtIndexFinishedCount++;

    if (m_updateAtIndexFinishedCount == node()->count() * receivers(SIGNAL(updateAtIndex(int,QVariant)))) {
        info() << "UpdateFinished" << node() << "with status" << status();
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
