#include "cookoperation.h"
#include "nodebase.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_mode(CookOperationAttached::Skip)
{
    if (node()->hasSignal(SIGNAL(cookAtIndex(int, const QVariant))) &&
        node()->hasSignal(SIGNAL(cookedAtIndex(int, int))))
    {
        connect(this, SIGNAL(cookAtIndex(int, const QVariant)),
                node(), SIGNAL(cookAtIndex(int, const QVariant)));
        connect(node(), SIGNAL(cookedAtIndex(int, int)),
                this, SLOT(onCookedAtIndex(int, int)));
        m_mode = CookOperationAttached::CookAtIndex;
        debug() << node() << "found .cookAtIndex";
    } else if (node()->hasSignal(SIGNAL(cookAtIndex(int index, const QVariant))) ||
               node()->hasSignal(SIGNAL(cookedAtIndex(int index, int status))))
    {
        error() << node() << "is missing the cook or cooked signal";
    } else if (node()->hasSignal(SIGNAL(cook(const QVariant))) &&
               node()->hasSignal(SIGNAL(cooked(int))))
    {
        connect(this, SIGNAL(cook(const QVariant)),
                node(), SIGNAL(cook(const QVariant)));
        connect(node(), SIGNAL(cooked(int)),
                this, SLOT(onCooked(int)));
        m_mode = CookOperationAttached::Cook;
        debug() << node() << "found .cook";
    } else if (node()->hasSignal(SIGNAL(cook(const QVariant))) ||
               node()->hasSignal(SIGNAL(cooked(int))))
    {
        error() << node() << "is missing the cook or cooked signal";
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
    debug() << ".run, cook mode is" << m_mode;
    m_indexStatus.clear();
    if (m_mode == CookOperationAttached::Cook) {
        emit cook(env());
    } else if (m_mode == CookOperationAttached::CookAtIndex) {
        for (int i = 0; i < node()->count(); i++) {
            emit cookAtIndex(i, env());
        }
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void CookOperationAttached::onCooked(int s)
{
    debug() << node() << "onCooked" << static_cast<OperationAttached::Status>(s);
    setStatus(static_cast<OperationAttached::Status>(s));
    continueRunning();
}

void CookOperationAttached::onCookedAtIndex(int index, int s)
{
    debug() << node() << "onCookedAtIndex" << index << static_cast<OperationAttached::Status>(s);
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
