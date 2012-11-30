#ifndef TESTOPERATION_H
#define TESTOPERATION_H

#include "operation.h"

class TestOperationAttached : public OperationAttached
{
    Q_OBJECT
public:
    explicit TestOperationAttached(QObject *parent = 0);

    Q_INVOKABLE virtual void run();

protected:
    virtual const QMetaObject *operationMetaObject() const;
};

class TestOperation : public Operation
{
    Q_OBJECT
public:
    explicit TestOperation(QObject *parent = 0);

    static TestOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(TestOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
