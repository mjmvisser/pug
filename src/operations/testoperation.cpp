#include "testoperation.h"

TestOperationAttached::TestOperationAttached(QObject *parent) :
        OperationAttached(parent) {
}

void TestOperationAttached::run() {
    trace() << node() << operation() << ".run()";
    if (context().value("error").toBool()) {
        error() << "intentionally generating an error for test purposes";
        setStatus(OperationAttached::Error);
    } else {
        debug() << "finished";
        setStatus(OperationAttached::Finished);
    }

    continueRunning();
}

const QMetaObject *TestOperationAttached::operationMetaObject() const {
    return &TestOperation::staticMetaObject;
}

TestOperation::TestOperation(QObject *parent) :
        Operation(parent) {
}

TestOperationAttached *TestOperation::qmlAttachedProperties(QObject *object) {
    return new TestOperationAttached(object);
}
