#include "cookoperation.h"
#include "node.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(CookOperationAttached::Skip)
{
    setObjectName("cook");
    bool haveCookAtIndexSignal = node()->hasSignal(SIGNAL(cookAtIndex(int, const QVariant)));
    bool haveCookAtIndexFinishedSignal = node()->hasSignal(SIGNAL(cookAtIndexFinished(int, int)));

    bool haveCookSignal = node()->hasSignal(SIGNAL(cook(const QVariant)));
    bool haveCookFinishedSignal = node()->hasSignal(SIGNAL(cookFinished(int)));

    if (haveCookAtIndexSignal && haveCookAtIndexFinishedSignal) {
        m_mode = CookOperationAttached::CookAtIndex;
        debug() << node() << "found cookAtIndex and cookAtIndexFinished signals";

        connect(this, SIGNAL(cookAtIndex(int, const QVariant)),
                node(), SIGNAL(cookAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(cookAtIndexFinished(int, int)),
                this, SLOT(onCookAtIndexFinished(int, int)));

        if (haveCookSignal) {
            debug() << node() << "found cook signal";
            connect(this, SIGNAL(cook(const QVariant)),
                    node(), SIGNAL(cook(const QVariant)));
        }

        if (haveCookFinishedSignal) {
            warning() << node() << "ignoring cookFinished signal";
        }

    } else if (!haveCookAtIndexSignal && haveCookAtIndexFinishedSignal) {
        error() << node() << "is missing the cookAtIndex signal";
    } else if (haveCookAtIndexSignal && !haveCookAtIndexFinishedSignal) {
        error() << node() << "is missing the cookAtIndexFinished signal";
    } else if (haveCookSignal && haveCookFinishedSignal) {
        m_mode = CookOperationAttached::Cook;
        debug() << node() << "found cook and cookFinished signals";

        connect(this, SIGNAL(cook(const QVariant)),
                node(), SIGNAL(cook(const QVariant)));
        connect(node(), SIGNAL(cookFinished(int)),
                this, SLOT(onCookFinished(int)));
    } else if (!haveCookSignal && haveCookFinishedSignal) {
        error() << node() << "is missing the cook signal";
    } else if (haveCookSignal && !haveCookFinishedSignal) {
        error() << node() << "is missing the cookFinished signal";
    } else {
        debug() << node() << "is not cookable";
    }
}

void CookOperationAttached::run()
{
    trace() << node() << ".run() [cook mode is" << m_mode << "]";

    m_indexStatus.clear();
    m_cookAtIndexFinishedCount = 0;
    if (m_mode == CookOperationAttached::Cook) {
        info() << "Cooking" << node() << "with" << context();
        emit cook(context());
    } else if (m_mode == CookOperationAttached::CookAtIndex) {
        if (node()->count() > 0) {
            debug() << "cooking" << node()->count() << "details";
            emit cook(context());
            for (int i = 0; i < node()->count(); i++) {
                info() << "Cooking" << node() << "at index" << i << "with" << context();
                debug() << node() << "emitting cookAtIndex" << i << context();
                emit cookAtIndex(i, context());
            }
        } else {
            debug() << node() << "has nothing to cook";
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void CookOperationAttached::onCookFinished(int s)
{
    trace() << node() << ".onCookFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "CookFinished" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void CookOperationAttached::onCookAtIndexFinished(int index, int s)
{
    trace() << node() << ".onCookAtIndexFinished(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "CookFinished" << node() << "at index" << m_cookAtIndexFinishedCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_cookAtIndexFinishedCount++;

    if (m_cookAtIndexFinishedCount == node()->count() * receivers(SIGNAL(cookAtIndex(int,QVariant)))) {
        info() << "CookFinished" << node() << "with status" << static_cast<OperationAttached::Status>(s);
        info() << "Result is" << node()->details().toVariant();
        setStatus(m_indexStatus.status());
        continueRunning();
    }
}

const QMetaObject *CookOperationAttached::operationMetaObject() const
{
    return &CookOperation::staticMetaObject;
}

CookOperation::CookOperation(QObject *parent) :
    Operation(parent)
{
}

CookOperationAttached *CookOperation::qmlAttachedProperties(QObject *object)
{
    return new CookOperationAttached(object);
}
