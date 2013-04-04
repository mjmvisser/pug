#ifndef UPDATEOPERATION_H
#define UPDATEOPERATION_H

#include "operation.h"

class UpdateOperationAttached : public OperationAttached
{
    Q_OBJECT
public:
    explicit UpdateOperationAttached(QObject *parent = 0);

    virtual void reset();
    virtual void run();

signals:
    void update(const QVariant env);
    void updateAtIndex(int, const QVariant env);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onUpdateFinished(int);
    void onUpdateAtIndexFinished(int, int);

private:
    enum SignalMode { Skip, Update, UpdateAtIndex };
    friend QDebug operator<<(QDebug dbg, SignalMode m);

    SignalMode m_mode;
    OperationStatusList m_indexStatus;
    int m_updateAtIndexFinishedCount;
};

inline QDebug operator<<(QDebug dbg, UpdateOperationAttached::SignalMode m)
{
    switch (m) {
    case UpdateOperationAttached::Skip:
        dbg.nospace() << "Skip";
        break;
    case UpdateOperationAttached::Update:
        dbg.nospace() << "Update";
        break;
    case UpdateOperationAttached::UpdateAtIndex:
        dbg.nospace() << "UpdateAtIndex";
        break;
    default:
        dbg.nospace() << "Unknown!";
        break;
    }

    return dbg.maybeSpace();
}

class UpdateOperation : public Operation
{
    Q_OBJECT
public:
    explicit UpdateOperation(QObject *parent = 0);

    static UpdateOperationAttached *qmlAttachedProperties(QObject *);
};

QML_DECLARE_TYPEINFO(UpdateOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
