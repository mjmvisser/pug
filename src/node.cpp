#include <QDebug>
#include <QQmlProperty>
#include <QJSValue>

#include "node.h"
#include "branch.h"

Node::Node(QObject *parent) :
    NodeBase(parent)
{
}

const QString Node::tempFile(const QString fileName) const
{
    const Branch *parentBranch = firstParent<Branch>();
    if (parentBranch) {
        const Element *element = parentBranch->element(0);

        if (element) {
            return QDir(element->directory()).absoluteFilePath(fileName);
        }
    }
    return QString();
}

const QString Node::tempFile(const QString baseName, const QString extension) const
{
    return tempFile(baseName) + extension;
}

const QString Node::tempFile(const QString baseName, const QString frameSpec, const QString extension) const
{
    return tempFile(baseName) + frameSpec + extension;
}
