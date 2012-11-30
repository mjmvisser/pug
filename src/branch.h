#ifndef BRANCH_H
#define BRANCH_H

#include "branchbase.h"

class Branch : public BranchBase
{
    Q_OBJECT
public:
    explicit Branch(QObject *parent = 0);

protected slots:
    void onUpdate(const QVariant env);
};

Q_DECLARE_METATYPE(Branch*) // makes available to QVariant

#endif // BRANCH_H
