#ifndef ROOT_H
#define ROOT_H

#include "branch.h"

class Operation;

class Root : public Branch
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Operation> operations READ operations_ NOTIFY operationsChanged)
public:
    explicit Root(QObject *parent = 0);

    QQmlListProperty<Operation> operations_();

    Q_INVOKABLE const QVariant parse(const QString nodeName, const QString path);
    Q_INVOKABLE const QString map(const QString nodeName, const QVariant fields);
    Q_INVOKABLE Branch *findBranch(const QString path);

    Q_INVOKABLE Operation *findOperation(const QString name);

    virtual bool isRoot() const;

signals:
    void operationsChanged();

private:
    Branch *findBranch(Branch *branch, const QString path);

    // operations property
    static void operations_append(QQmlListProperty<Operation> *, Operation *);
    static int operations_count(QQmlListProperty<Operation> *);
    static Operation *operation_at(QQmlListProperty<Operation> *, int);
    static void operations_clear(QQmlListProperty<Operation> *);

    QList<Operation *> m_operations;
};

#endif // ROOT_H
