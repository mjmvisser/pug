#include <QDebug>

#include "root.h"
#include "branch.h"
#include "operation.h"

Root::Root(QObject *parent) :
        BranchBase(parent)
{
    setLogLevel(Log::Warning);
}

const QVariant Root::parse(const QString nodeName, const QString path)
{
    Branch *branch = qobject_cast<Branch *>(node(nodeName));
    if (branch)
        return branch->parse(path);
    else
        return QVariant();
}

const QString Root::map(const QString nodeName, const QVariant fields)
{
    Branch *branch = qobject_cast<Branch *>(node(nodeName));
    if (branch)
        return branch->map(fields);
    else
        return QString();
}

bool Root::isRoot() const
{
    return true;
}

BranchBase *Root::findBranch(const QString path)
{
    copious() << ".findBranch" << path;
    return findBranch(this, path);
}

BranchBase *Root::findBranch(BranchBase* branch, const QString path)
{
    copious() << ".findBranch" << branch << path;
    BranchBase *result = 0;
    foreach (QObject *o, branch->children()) {
        BranchBase *child = qobject_cast<BranchBase *>(o);
        if (child) {
            copious() << "checking" << child;
            QVariant fields = child->parse(path);
            copious() << ".findBranch got" << fields;
            if (fields.isValid()) {
                if (fields.toMap().contains("_")) {
                    result = findBranch(child, path);
                    if (result)
                        return result;
                } else {
                    return child;
                }
            }
        }
    }

    return 0;
}


Operation *Root::findOperation(const QString name)
{
    foreach (QObject *o, children()) {
        Operation *operation = qobject_cast<Operation *>(o);
        if (operation && operation->name() == name) {
            return operation;
        }
    }

    return 0;
}
