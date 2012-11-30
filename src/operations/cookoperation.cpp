#include "cookoperation.h"
#include "nodebase.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_cookableFlag(false)
{
    if (node()->hasSignal(SIGNAL(cook(const QVariant))) &&
        node()->hasSignal(SIGNAL(cooked(int))))
    {
        connect(this, SIGNAL(cook(const QVariant)),
                node(), SIGNAL(cook(const QVariant)));
        connect(node(), SIGNAL(cooked(int)),
                this, SLOT(onCooked(int)));
        m_cookableFlag = true;
    }
}

bool CookOperationAttached::isCookable() const
{
    return m_cookableFlag;
}

void CookOperationAttached::setCookable(bool c)
{
    if (m_cookableFlag != c) {
        m_cookableFlag = c;
        emit cookableChanged(m_cookableFlag);
    }
}

void CookOperationAttached::run()
{
    debug() << ".run, cookable is" << m_cookableFlag;
    if (m_cookableFlag) {
        debug() << node() << ".run";
        emit cook(env());
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
