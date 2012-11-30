#ifndef LISTOPERATION_H
#define LISTOPERATION_H

#include "operation.h"

class ListOperationAttached : public OperationAttached
{
    Q_OBJECT
public:
    explicit ListOperationAttached(QObject *parent = 0);

    Q_INVOKABLE virtual void run();

protected:
    virtual const QMetaObject *operationMetaObject() const;
};

class ListOperation : public Operation
{
    Q_OBJECT
public:
    explicit ListOperation(QObject *parent = 0);

    static ListOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(ListOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
