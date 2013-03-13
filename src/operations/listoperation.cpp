#include <iostream>

#include "listoperation.h"
#include "branch.h"

ListOperationAttached::ListOperationAttached(QObject *parent) :
    OperationAttached(parent)
{
}

void ListOperationAttached::run()
{
    info() << "Listing" << node();
    trace() << node() << ".run()";
    Branch *branch = qobject_cast<Branch *>(node());

    if (branch) {
        for (int index = 0; index < branch->details().property("length").toInt(); index++) {
            QJSValue detail = branch->details().property(index);
            if (detail.isObject()) {
                const Element *e = qjsvalue_cast<Element *>(detail.property("element"));
                if (e)
                    std::cout << e->toString().toUtf8().constData() << std::endl;
            }
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
