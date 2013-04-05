#ifndef COOKOPERATION_H
#define COOKOPERATION_H

#include "operation.h"

#include <QList>

class CookOperationAttached : public OperationAttached
{
    Q_OBJECT
public:
    explicit CookOperationAttached(QObject *parent = 0);

    Q_INVOKABLE virtual void run();

signals:
    void cook(const QVariant env);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onCookFinished(int);

private:
    bool m_cookable;
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
