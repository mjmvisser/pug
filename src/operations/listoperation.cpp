#include <iostream>

#include "listoperation.h"
#include "branch.h"

ListOperationAttached::ListOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
    setObjectName("list");
}

void ListOperationAttached::run()
{
    info() << "Listing" << node();
    trace() << node() << ".run()";
    Branch *branch = qobject_cast<Branch *>(node());

    if (branch) {
        for (int index = 0; index < branch->count(); index++) {
            // TODO: print frames
            std::cout << node()->details().property(index).property("element").property("pattern").toString().toUtf8().constData() << std::endl;
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
    setObjectName("list");
}

ListOperationAttached *ListOperation::qmlAttachedProperties(QObject *object)
{
    return new ListOperationAttached(object);
}
