#include "cookoperation.h"
#include "node.h"

CookOperationAttached::CookOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
    setObjectName("cook");
}

const QMetaObject *CookOperationAttached::operationMetaObject() const
{
    return &CookOperation::staticMetaObject;
}

CookOperation::CookOperation(QObject *parent) :
    Operation(parent)
{
    setObjectName("cook");
}

CookOperationAttached *CookOperation::qmlAttachedProperties(QObject *object)
{
    return new CookOperationAttached(object);
}
