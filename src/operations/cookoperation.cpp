#include "cookoperation.h"
#include "nodebase.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(CookOperationAttached::Skip)
{
    bool haveCookAtIndexSignal = node()->hasSignal(SIGNAL(cookAtIndex(int, const QVariant)));
    bool haveCookedAtIndexSignal = node()->hasSignal(SIGNAL(cookedAtIndex(int, int)));

    bool haveCookSignal = node()->hasSignal(SIGNAL(cook(const QVariant)));
    bool haveCookedSignal = node()->hasSignal(SIGNAL(cooked(int)));

    if (haveCookAtIndexSignal && haveCookedAtIndexSignal) {
        connect(this, SIGNAL(cookAtIndex(int, const QVariant)),
                node(), SIGNAL(cookAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(cookedAtIndex(int, int)),
                this, SLOT(onCookedAtIndex(int, int)));
        m_mode = CookOperationAttached::CookAtIndex;
        debug() << node() << "found .cookAtIndex";
    } else if (!haveCookAtIndexSignal && haveCookedAtIndexSignal) {
        error() << node() << "is missing the cookAtIndex signal";
    } else if (haveCookAtIndexSignal && !haveCookedAtIndexSignal) {
        error() << node() << "is missing the cookedAtIndex signal";
    } else if (haveCookSignal && haveCookedSignal) {
        connect(this, SIGNAL(cook(const QVariant)),
                node(), SIGNAL(cook(const QVariant)));
        connect(node(), SIGNAL(cooked(int)),
                this, SLOT(onCooked(int)));
        m_mode = CookOperationAttached::Cook;
        debug() << node() << "found .cook";
    } else if (!haveCookSignal && haveCookedSignal) {
        error() << node() << "is missing the cook signal";
    } else if (haveCookSignal && !haveCookedSignal) {
        error() << node() << "is missing the cooked signal";
    } else {
        debug() << node() << "is not cookable";
    }
}

CookOperationAttached::Mode CookOperationAttached::mode() const
{
    return m_mode;
}

void CookOperationAttached::run()
{
    info() << "Cooking" << node() << "with" << context();
    trace() << node() << ".run() [cook mode is" << m_mode << "]";

    m_indexStatus.clear();
    if (m_mode == CookOperationAttached::Cook) {
        emit cook(context());
    } else if (m_mode == CookOperationAttached::CookAtIndex) {
        if (node()->count() > 0) {
            debug() << "cooking" << node()->count() << "details";
            for (int i = 0; i < node()->count(); i++) {
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
    continueRunning();
}

void CookOperationAttached::onCookedAtIndex(int index, int s)
{
    trace() << node() << ".onCookedAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    if (m_indexStatus.length() == node()->count()) {
        onCooked(m_indexStatus.status());
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
