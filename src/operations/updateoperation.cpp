#include "updateoperation.h"
#include "node.h"

UpdateOperationAttached::UpdateOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
    setObjectName("update");
}

const QMetaObject *UpdateOperationAttached::operationMetaObject() const
{
    return &UpdateOperation::staticMetaObject;
}

UpdateOperation::UpdateOperation(QObject *parent) :
    Operation(parent)
{
    setObjectName("update");
}

UpdateOperationAttached *UpdateOperation::qmlAttachedProperties(QObject *object)
{
    return new UpdateOperationAttached(object);
}
