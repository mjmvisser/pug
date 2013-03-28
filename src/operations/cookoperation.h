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
    void cookAtIndex(int index, const QVariant env);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onCooked(int);
    void onCookedAtIndex(int, int);

private:
    enum Mode { Skip, Cook, CookAtIndex };
    friend QDebug operator<<(QDebug dbg, Mode m);

    Mode m_mode;
    OperationStatusList m_indexStatus;
    int m_cookedAtIndexCount;
};

inline QDebug operator<<(QDebug dbg, CookOperationAttached::Mode m)
{
    switch (m) {
    case CookOperationAttached::Skip:
        dbg.nospace() << "Skip";
        break;
    case CookOperationAttached::Cook:
        dbg.nospace() << "Cook";
        break;
    case CookOperationAttached::CookAtIndex:
        dbg.nospace() << "CookAtIndex";
        break;
    default:
        dbg.nospace() << "Unknown!";
        break;
    }

    return dbg.maybeSpace();
}

class CookOperation : public Operation
{
    Q_OBJECT
public:
    explicit CookOperation(QObject *parent = 0);

    static CookOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(CookOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
