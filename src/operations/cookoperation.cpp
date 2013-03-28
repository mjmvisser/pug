#include "cookoperation.h"
#include "node.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(CookOperationAttached::Skip)
{
    setObjectName("cook");
    bool haveCookAtIndexSignal = node()->hasSignal(SIGNAL(cookAtIndex(int, const QVariant)));
    bool haveCookedAtIndexSignal = node()->hasSignal(SIGNAL(cookedAtIndex(int, int)));

    bool haveCookSignal = node()->hasSignal(SIGNAL(cook(const QVariant)));
    bool haveCookedSignal = node()->hasSignal(SIGNAL(cooked(int)));

    if (haveCookAtIndexSignal && haveCookedAtIndexSignal) {
        m_mode = CookOperationAttached::CookAtIndex;
        debug() << node() << "found cookAtIndex and cookedAtIndex signals";

        connect(this, SIGNAL(cookAtIndex(int, const QVariant)),
                node(), SIGNAL(cookAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(cookedAtIndex(int, int)),
                this, SLOT(onCookedAtIndex(int, int)));

        if (haveCookSignal) {
            debug() << node() << "found cook signal";
            connect(this, SIGNAL(cook(const QVariant)),
                    node(), SIGNAL(cook(const QVariant)));
        }

        if (haveCookedSignal) {
            warning() << node() << "ignoring cooked signal";
        }

    } else if (!haveCookAtIndexSignal && haveCookedAtIndexSignal) {
        error() << node() << "is missing the cookAtIndex signal";
    } else if (haveCookAtIndexSignal && !haveCookedAtIndexSignal) {
        error() << node() << "is missing the cookedAtIndex signal";
    } else if (haveCookSignal && haveCookedSignal) {
        m_mode = CookOperationAttached::Cook;
        debug() << node() << "found cook and cooked signals";

        connect(this, SIGNAL(cook(const QVariant)),
                node(), SIGNAL(cook(const QVariant)));
        connect(node(), SIGNAL(cooked(int)),
                this, SLOT(onCooked(int)));
    } else if (!haveCookSignal && haveCookedSignal) {
        error() << node() << "is missing the cook signal";
    } else if (haveCookSignal && !haveCookedSignal) {
        error() << node() << "is missing the cooked signal";
    } else {
        debug() << node() << "is not cookable";
    }
}

void CookOperationAttached::run()
{
    trace() << node() << ".run() [cook mode is" << m_mode << "]";

    m_indexStatus.clear();
    m_cookedAtIndexCount = 0;
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
            warning() << node() << "has nothing to cook";
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void CookOperationAttached::onCooked(int s)
{
    trace() << node() << ".onCooked(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Cooked" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void CookOperationAttached::onCookedAtIndex(int index, int s)
{
    trace() << node() << ".onCookedAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Cooked" << node() << "at index" << m_cookedAtIndexCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_cookedAtIndexCount++;

    if (m_cookedAtIndexCount == node()->count() * receivers(SIGNAL(cookAtIndex(int,QVariant)))) {
        info() << "Cooked" << node() << "with status" << static_cast<OperationAttached::Status>(s);
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
