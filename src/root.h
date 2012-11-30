#ifndef ROOT_H
#define ROOT_H

#include "branchbase.h"

class Operation;

class Root : public BranchBase
{
    Q_OBJECT
public:
    explicit Root(QObject *parent = 0);

    Q_INVOKABLE const QVariant parse(const QString nodeName, const QString path);
    Q_INVOKABLE const QString map(const QString nodeName, const QVariant fields);
    Q_INVOKABLE BranchBase *findBranch(const QString path);

    Q_INVOKABLE Operation *findOperation(const QString name);

    virtual bool isRoot() const;

private:
    BranchBase *findBranch(BranchBase *branch, const QString path);
};

#endif // ROOT_H
