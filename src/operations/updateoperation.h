#ifndef UPDATEOPERATION_H
#define UPDATEOPERATION_H

#include "operation.h"

class UpdateOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_PROPERTY(bool updatable READ isUpdatable WRITE setUpdatable NOTIFY updatableChanged)
public:
    explicit UpdateOperationAttached(QObject *parent = 0);

    bool isUpdatable() const;
    void setUpdatable(bool);

    Q_INVOKABLE virtual void run();

signals:
    void update(const QVariant env);
    void updatableChanged(bool updatable);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onUpdated(int);

private:
    bool m_updatableFlag;
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
