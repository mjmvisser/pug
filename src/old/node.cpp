#include <QDebug>
#include <QQmlProperty>
#include <QJSValue>

#include "node.h"
#include "branch.h"

Node::Node(QObject *parent) :
    NodeBase(parent)
{
}

const QString Node::temporaryDir() const
{
    if (m_temporaryDir.isNull()) {
        // construct a temporary dir
        const Branch *parentBranch = firstParent<Branch>();
        if (parentBranch) {
            const Element *element = parentBranch->element(index());

            if (element) {
                return QDir(element->directory()).absoluteFilePath("temp");
            }
        }
        return QString();

    } else {
        return m_temporaryDir;
    }
}

void Node::setTemporaryDir(const QString dir)
{
    if (m_temporaryDir != dir) {
        m_temporaryDir = dir;
        emit temporaryDirChanged(dir);
    }
}

const QString Node::temporaryFile(const QString templateName) const
{
    return QDir(temporaryDir()).absoluteFilePath(templateName);
}
