#ifndef COOKOPERATION_H
#define COOKOPERATION_H

#include "operation.h"

class CookOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_PROPERTY(bool cookable READ isCookable WRITE setCookable NOTIFY cookableChanged)
public:
    explicit CookOperationAttached(QObject *parent = 0);

    bool isCookable() const;
    void setCookable(bool);

    Q_INVOKABLE virtual void run();

signals:
    void cook(const QVariant env);
    void cookableChanged(bool cookable);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onCooked(int);

private:
    bool m_cookableFlag;
};

class CookOperation : public Operation
{
    Q_OBJECT
public:
    explicit CookOperation(QObject *parent = 0);

    static CookOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(CookOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
