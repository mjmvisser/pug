#include <QFileInfo>

#include "releaseoperation.h"
#include "elementsview.h"
#include "filepattern.h"
#include "file.h"
#include "branch.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_source(),
    m_versionBranch(),
    m_mode(ReleaseOperationAttached::Copy)
{
    setObjectName("release");

    File *file = qobject_cast<File *>(node());
    if (file) {
        file->addInput(this, "source");
        file->addInput(this, "versionBranch");
    }

    connect(this, &OperationAttached::prepare, this, &ReleaseOperationAttached::onPrepare);
}

Node *ReleaseOperationAttached::source()
{
    return m_source;
}

const Node *ReleaseOperationAttached::source() const
{
    return m_source;
}

void ReleaseOperationAttached::setSource(Node *n)
{
    if (m_source != n) {
        m_source = n;
        emit sourceChanged(n);
    }
}

const Branch *ReleaseOperationAttached::versionBranch() const
{
    return m_versionBranch;
}

Branch *ReleaseOperationAttached::versionBranch()
{
    return m_versionBranch;
}

void ReleaseOperationAttached::setVersionBranch(Branch *branch)
{
    if (m_versionBranch != branch) {
        m_versionBranch = branch;
        emit versionBranchChanged(branch);
    }
}

const QString ReleaseOperationAttached::versionFieldName() const
{
    return m_versionFieldName;
}

void ReleaseOperationAttached::setVersionFieldName(const QString name)
{
    if (m_versionFieldName != name) {
        m_versionFieldName = name;
        emit versionFieldChanged(m_versionFieldName);
    }
}

ReleaseOperationAttached::Mode ReleaseOperationAttached::mode() const
{
    return m_mode;
}

void ReleaseOperationAttached::setMode(ReleaseOperationAttached::Mode m)
{
    if (m_mode != m) {
        m_mode = m;
        emit modeChanged(m);
    }
}

Sudo *ReleaseOperationAttached::sudo()
{
    return operation<ReleaseOperation>()->sudo();
}

const QString ReleaseOperationAttached::findVersionFieldName() const
{
    if (m_versionBranch) {
        const ReleaseOperationAttached *versionBranchAttached = m_versionBranch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(versionBranchAttached);

        return versionBranchAttached->m_versionFieldName;
    } else if (!m_versionFieldName.isEmpty()) {
        return m_versionFieldName;
    }

    return QString();
}

int ReleaseOperationAttached::findLastVersion(const QVariantMap context) const
{
    trace() << node() << ".findLastVersion()";

    const Branch *versionBranch;
    QString versionFieldName;
    if (m_versionBranch) {
        // releasing a File with versionBranch set
        const ReleaseOperationAttached *versionBranchAttached = m_versionBranch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(versionBranchAttached);
        versionFieldName = versionBranchAttached->m_versionFieldName;
        versionBranch = m_versionBranch;
    } else if (!m_versionFieldName.isEmpty()) {
        // releasing a Folder with versionField set
        versionFieldName = m_versionFieldName;
        versionBranch = qobject_cast<const Branch *>(node());
    }

    if (versionBranch && !versionFieldName.isEmpty()) {
        const Field *versionField = versionBranch->findField(versionFieldName);
        int lastVersion = 0;
        for (int index = 0; index < versionBranch->details().property("length").toInt(); index++) {
            QVariantMap versionContext = Node::mergeContexts(context,
                    versionBranch->details().property(index).property("context").toVariant().toMap());
            QVariant version = versionField->get(versionContext);
            if (version.isValid() && version.canConvert<int>() && version.toInt() > lastVersion) {
                lastVersion = version.toInt();
            }
        }

        debug() << versionBranch << "findLastVersion got version" << lastVersion;
        return lastVersion;
    } else {
        error() << "can't find last version";
        return -1;
    }
}

void ReleaseOperationAttached::onPrepare()
{
    if (receivers(SIGNAL(cook(const QVariant))) > 0) {
        trace() << node() << "ReleaseOperation.onPrepare()";
        // if we're going to cook later...
        // TODO: maybe move this logic to Folder/File?
        Q_ASSERT(operation());
        Q_ASSERT(node());

        QVariantMap releaseContext = context();

        if  ((m_source && m_versionBranch) || !m_versionFieldName.isEmpty()) {
            int version = findLastVersion(releaseContext);
            if (version >= 0) {
                const QString versionFieldName = findVersionFieldName();

                // TODO: logic here is ugly
                // what happens is that Folder::onRelease is called first, which creates a new version
                // then the File::onRelease uses that version
                // so we need to incremented the version ONLY if we're creating the next version folder
                if (!m_versionBranch)
                    version++;  // version will be created

                debug() << node() << "setting" << versionFieldName << "to" << version;
                releaseContext.insert(versionFieldName, version);
                setContext(releaseContext);
            } else {
                error() << node() << "Can't find last version";
            }
        }
    } else {
        debug() << node() << "skipping";
    }

    emit prepareFinished();
}

const QMetaObject *ReleaseOperationAttached::operationMetaObject() const
{
    return &ReleaseOperation::staticMetaObject;
}

ReleaseOperation::ReleaseOperation(QObject *parent) :
    Operation(parent),
    m_sudo()
{
    setObjectName("release");
}

Sudo *ReleaseOperation::sudo()
{
    return m_sudo;
}

void ReleaseOperation::setSudo(Sudo *sudo)
{
    if (m_sudo != sudo) {
        m_sudo = sudo;
        emit sudoChanged(sudo);
    }
}

ReleaseOperationAttached *ReleaseOperation::qmlAttachedProperties(QObject *object)
{
    return new ReleaseOperationAttached(object);
}
