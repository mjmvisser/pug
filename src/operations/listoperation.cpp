#include <iostream>

#include "listoperation.h"
#include "branchbase.h"

ListOperationAttached::ListOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
}

void ListOperationAttached::run()
{
    BranchBase *branch = qobject_cast<BranchBase *>(node());

    if (branch) {
        foreach (const Element *element, branch->elements()) {
            std::cout << element->toString().toUtf8().constData() << std::endl;
        }
    }

    setStatus(OperationAttached::Finished);
    continueRunning();
}

const QMetaObject *ListOperationAttached::operationMetaObject() const
{
    return &ListOperation::staticMetaObject;
}

ListOperation::ListOperation(QObject *parent) :
    Operation(parent)
{
}

ListOperationAttached *ListOperation::qmlAttachedProperties(QObject *object)
{
    return new ListOperationAttached(object);
}
