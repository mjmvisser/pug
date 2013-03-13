#ifndef BRANCH_H
#define BRANCH_H

#include "branch.h"

class Folder : public Branch
{
    Q_OBJECT
public:
    explicit Folder(QObject *parent = 0);

protected slots:
    void onUpdate(const QVariant env);
};

Q_DECLARE_METATYPE(Folder*) // makes available to QVariant

#endif // BRANCH_H
