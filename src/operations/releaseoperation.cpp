#include "releaseoperation.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_releasableFlag(false),
    m_target(),
    m_mode(ReleaseOperationAttached::Copy),
    m_queue(new FileOpQueue(this))
{
    connect(m_queue, &FileOpQueue::finished, this, &ReleaseOperationAttached::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &ReleaseOperationAttached::onFileOpQueueError);
}

bool ReleaseOperationAttached::isReleasable() const
{
    return m_releasableFlag;
}

void ReleaseOperationAttached::setReleasable(bool r)
{
    if (m_releasableFlag != r) {
        m_releasableFlag = r;
        emit releasableChanged(r);
    }
}

BranchBase *ReleaseOperationAttached::target()
{
    return m_target;
}

const BranchBase *ReleaseOperationAttached::target() const
{
    return m_target;
}

void ReleaseOperationAttached::setTarget(BranchBase *branch)
{
    if (m_target != branch) {
        m_target = branch;
        emit targetChanged(m_target);
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

QString ReleaseOperationAttached::modeString() const
{
    switch (m_mode) {
    case ReleaseOperationAttached::Copy:
        return "copy";
    case ReleaseOperationAttached::Move:
        return "move";
    default:
        return "unknown";
    }
}

void ReleaseOperationAttached::reset()
{
    OperationAttached::reset();
    m_paths.clear();
}

const QString ReleaseOperationAttached::findVersionFieldName() const
{
    const BranchBase *branch = qobject_cast<const BranchBase *>(node());
    const ReleaseOperationAttached *branchAttached = branch ? branch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject()) : 0;
    QString versionFieldName = branchAttached ? branchAttached->m_versionFieldName : QString();

    while (branch && branchAttached && versionFieldName.isEmpty()) {
        branch = branch->root();
        branchAttached = branch ? branch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject()) : 0;
        versionFieldName = branchAttached ? branchAttached->m_versionFieldName : QString();
    }

    return versionFieldName;
}

int ReleaseOperationAttached::findLastVersion(const QVariant data) const
{
    // find branch parent with version field
    const BranchBase *branch = qobject_cast<const BranchBase *>(node());

    const ReleaseOperationAttached *branchAttached = branch ? branch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject()) : 0;
    QString versionFieldName = branchAttached ? branchAttached->m_versionFieldName : QString();
    const Field *versionField = !versionFieldName.isEmpty() ? branch->findField(versionFieldName) : 0;

    while (branch && branchAttached && !versionField) {
        branch = branch->root();
        branchAttached = branch ? branch->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject()) : 0;
        versionFieldName = branchAttached ? branchAttached->m_versionFieldName : QString();
        versionField = !versionFieldName.isEmpty() ? branch->findField(versionFieldName) : 0;
    }

    if (branchAttached && versionField) {
        QStringList releasedPaths = branch->listMatchingPaths(data);

        int lastVersion = 0;

        foreach (QString path, releasedPaths) {
            QVariant data = branch->parse(path);
            if (data.isValid() && data.type() == QVariant::Map &&
                    data.toMap().contains(versionField->name()) &&
                    data.toMap().value(versionField->name()).canConvert<int>())
            {
                int version = data.toMap().value(versionField->name()).toInt();
                if (version > lastVersion) {
                    lastVersion = version;
                }
            } else {
                error() << this << "for node" << node() << "can't parse" << path;
            }
        }

        return lastVersion;
    } else {
        error() << this << ".lastVersion for" << node() << "no VERSION field found";
        return 0;
    }
}


void ReleaseOperationAttached::run()
{
    Q_ASSERT(operation());

    const BranchBase *branch = qobject_cast<const BranchBase *>(node());

    if (branch && m_releasableFlag && m_target) {
        const ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());

        QStringList destPaths;
        const QString versionFieldName = targetAttached->findVersionFieldName();
        int nextVersion = targetAttached->findLastVersion(env()) + 1;

        foreach (const Element *srcFile, branch->elements()) {
            if (srcFile->frameList()) {
                foreach (QString srcPath, srcFile->paths()) {
                    const QString destPath = release(srcPath, versionFieldName, nextVersion);
                    if (!destPath.isEmpty())
                        destPaths << destPath;
                }
            } else {
                const QString destPath = release(srcFile->path(), versionFieldName, nextVersion);
                if (!destPath.isEmpty())
                    destPaths << destPath;
            }
        }

        // stash the destination path
        m_target->setPaths(destPaths);

        debug() << node() << ".run set paths on" << m_target << "to" << destPaths;

        debug() << node() << "elements.length is" << m_target->elements().length();
        debug() << node() << "firstFrame is" << m_target->elements().at(0)->firstFrame();
        debug() << node() << "lastFrame is" << m_target->elements().at(0)->lastFrame();

        m_queue->run(env());

    } else if (m_releasableFlag && !m_target) {
        error() << this << "no target specified";
        setStatus(OperationAttached::Error);
        continueRunning();
    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

const QString ReleaseOperationAttached::release(const QString srcPath, const QString versionFieldName, int nextVersion)
{
    const BranchBase *branch = qobject_cast<const BranchBase *>(node());

    QVariant srcData = branch->parse(srcPath);

    if (srcData.isValid()) {
        // create a new env based off our default
        QVariantMap destData(env());

        // merge srcData
        QMapIterator<QString, QVariant> i(srcData.toMap());
        while (i.hasNext()) {
            i.next();
            // don't overwrite
            destData.insert(i.key(), i.value());
        }

        // add our next version
        destData[versionFieldName] = nextVersion;

        QString destPath = m_target->map(destData);

        if (!destPath.isEmpty()) {
            QFileInfo destInfo(destPath);
            if (!destInfo.absoluteDir().exists())
                m_queue->mkdir(destInfo.absoluteDir().absolutePath());

            switch (m_mode) {
            case ReleaseOperationAttached::Copy:
                m_queue->copy(srcPath, destPath);
                break;

            case ReleaseOperationAttached::Move:
                m_queue->move(srcPath, destPath);
                break;

            default:
                error() << this << "unknown file mode " << m_mode;
                break;
            }

            return destPath;
        }
    }

    return QString();
}

void ReleaseOperationAttached::onFileOpQueueFinished()
{
    debug() << ".onFileOpQueueFinished";
    const ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
    const QString versionFieldName = targetAttached->findVersionFieldName();
    int version = targetAttached->findLastVersion(env());

    QVariantMap fields = env();
    fields[versionFieldName] = version;

    // make sure parents are updated
    BranchBase *p = m_target->firstParent<BranchBase>();
    while (p && !p->isRoot()) {
        emit p->update(fields);
        p = p->firstParent<BranchBase>();
    }

    setStatus(OperationAttached::Finished);
    continueRunning();
}

void ReleaseOperationAttached::onFileOpQueueError()
{
    debug() << this << "error";
    setStatus(OperationAttached::Error);
    continueRunning();
}

const QMetaObject *ReleaseOperationAttached::operationMetaObject() const
{
    return &ReleaseOperation::staticMetaObject;
}

ReleaseOperation::ReleaseOperation(QObject *parent) :
    Operation(parent)
{
}

const QString ReleaseOperation::user() const
{
    return m_user;
}

void ReleaseOperation::setUser(const QString u)
{
    m_user = u;
}

const QString ReleaseOperation::group() const
{
    return m_group;
}

void ReleaseOperation::setGroup(const QString g)
{
    m_group = g;
}

ReleaseOperationAttached *ReleaseOperation::qmlAttachedProperties(QObject *object)
{
    return new ReleaseOperationAttached(object);
}
