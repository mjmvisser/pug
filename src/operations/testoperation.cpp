#include "testoperation.h"

TestOperationAttached::TestOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
}

void TestOperationAttached::run()
{
    debug() << this << node() << operation() << ".run";
    debug() << "finished!";
    setStatus(OperationAttached::Finished);
    continueRunning();
}

const QMetaObject *TestOperationAttached::operationMetaObject() const
{
    return &TestOperation::staticMetaObject;
}

TestOperation::TestOperation(QObject *parent) :
    Operation(parent)
{
}

TestOperationAttached *TestOperation::qmlAttachedProperties(QObject *object)
{
    return new TestOperationAttached(object);
}
