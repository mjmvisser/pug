#include <iostream>

#include "listoperation.h"
#include "branch.h"
#include "elementsview.h"

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
        QScopedPointer<ElementsView> elementsView(new ElementsView(node()));


        for (int elementIndex = 0; elementIndex < elementsView->elementCount(); elementIndex++) {
            const ElementView *element = elementsView->elementAt(elementIndex);
            if (element)
                // TODO: print frames
                std::cout << element->pattern().toUtf8().constData() << std::endl;
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
