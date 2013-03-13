#include <QDebug>
#include <QQmlProperty>
#include <QJSValue>

#include "deprecatednode.h"
#include "folder.h"

DeprecatedNode::DeprecatedNode(QObject *parent) :
    Node(parent)
{
}

const QString DeprecatedNode::temporaryDir() const
{
    if (m_temporaryDir.isNull()) {
        // construct a temporary dir
        const Folder *parentBranch = firstParent<Folder>();
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

void DeprecatedNode::setTemporaryDir(const QString dir)
{
    if (m_temporaryDir != dir) {
        m_temporaryDir = dir;
        emit temporaryDirChanged(dir);
    }
}

const QString DeprecatedNode::temporaryFile(const QString templateName) const
{
    return QDir(temporaryDir()).absoluteFilePath(templateName);
}
