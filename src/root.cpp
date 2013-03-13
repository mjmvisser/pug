#include <QDebug>

#include "root.h"
#include "folder.h"
#include "operation.h"

Root::Root(QObject *parent) :
        Branch(parent)
{
    setLogLevel(Log::Warning);
}

const QVariant Root::parse(const QString nodeName, const QString path)
{
    Folder *branch = qobject_cast<Folder *>(node(nodeName));
    if (branch)
        return branch->parse(path);
    else
        return QVariant();
}

const QString Root::map(const QString nodeName, const QVariant fields)
{
    Folder *branch = qobject_cast<Folder *>(node(nodeName));
    if (branch)
        return branch->map(fields);
    else
        return QString();
}

bool Root::isRoot() const
{
    return true;
}


QQmlListProperty<Operation> Root::operations_()
{
    return QQmlListProperty<Operation>(this, 0,
            Root::operations_append,
            Root::operations_count,
            Root::operation_at,
            Root::operations_clear);
}

// operations property
void Root::operations_append(QQmlListProperty<Operation> *prop, Operation *op)
{
    Root *that = static_cast<Root *>(prop->object);

    op->setParent(that);
    that->m_operations.append(op);
    emit that->operationsChanged();
}

int Root::operations_count(QQmlListProperty<Operation> *prop)
{
    Root *that = static_cast<Root *>(prop->object);
    return that->m_operations.count();
}

Operation *Root::operation_at(QQmlListProperty<Operation> *prop, int i)
{
    Root *that = static_cast<Root *>(prop->object);
    if (i < that->m_operations.count())
        return that->m_operations[i];
    else
        return 0;
}

void Root::operations_clear(QQmlListProperty<Operation> *prop)
{
    Root *that = static_cast<Root *>(prop->object);
    foreach (Operation *op, that->m_operations) {
        op->setParent(0);
    }
    that->m_operations.clear();
    emit that->operationsChanged();
}


Branch *Root::findBranch(const QString path)
{
    trace() << ".findBranch(" << path << ")";
    return findBranch(this, path);
}

Branch *Root::findBranch(Branch* branch, const QString path)
{
    trace() << ".findBranch(" << branch << "," << path << ")";
    Branch *result = 0;
    foreach (QObject *o, branch->children()) {
        Branch *child = qobject_cast<Branch *>(o);
        if (child) {
            debug() << "checking" << child;
            QVariant fields = child->parse(path);
            debug() << ".findBranch got" << fields;
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
    foreach (Operation *op, m_operations) {
        if (op->name() == name) {
            return op;
        }
    }

    return 0;
}
