#ifndef UPDATEOPERATION_H
#define UPDATEOPERATION_H

#include "operation.h"

class UpdateOperationAttached : public OperationAttached
{
    Q_OBJECT
public:
    explicit UpdateOperationAttached(QObject *parent = 0);

    virtual void run();

signals:
    void update(const QVariant env);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onUpdateFinished(int);

private:
    bool m_updatable;
};

class UpdateOperation : public Operation
{
    Q_OBJECT
public:
    explicit UpdateOperation(QObject *parent = 0);

    static UpdateOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(UpdateOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
