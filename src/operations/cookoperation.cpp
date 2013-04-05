#include "cookoperation.h"
#include "node.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_cookable(false)
{
    setObjectName("cook");
    bool haveCookSignal = node()->hasSignal(SIGNAL(cook(const QVariant)));
    bool haveCookFinishedSignal = node()->hasSignal(SIGNAL(cookFinished(int)));

    if (haveCookSignal && haveCookFinishedSignal) {
        m_cookable = true;
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
    trace() << node() << ".run()";

    if (m_cookable) {
        info() << "Cooking" << node() << "with" << context();
        emit cook(context());
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void CookOperationAttached::onCookFinished(int s)
{
    trace() << node() << ".onCookFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Cooked" << node() << "with status" << status() << "details" << QJsonDocument::fromVariant(node()->details().toVariant()).toJson();

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
