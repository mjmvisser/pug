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

    connect(this, &ReleaseOperationAttached::targetChanged, this, &ReleaseOperationAttached::regenerateDetails);
    connect(node(), &NodeBase::detailsChanged, this, &ReleaseOperationAttached::regenerateDetails);
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

QJSValue ReleaseOperationAttached::details()
{
    return m_details;
}

const QJSValue ReleaseOperationAttached::details() const
{
    return m_details;
}

void ReleaseOperationAttached::reset()
{
    OperationAttached::reset();
    m_details = newArray();
}

void ReleaseOperationAttached::regenerateDetails()
{
    // regenerate our details
    const BranchBase *branch = qobject_cast<const BranchBase *>(node());

    if (branch && branch->details().isArray() && m_releasableFlag && m_target) {
        m_details = newArray();

        const ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());

        const QString versionFieldName = targetAttached->findVersionFieldName();
        int nextVersion = targetAttached->findLastVersion(env()) + 1;

        for (int index = 0; index < branch->details().property("length").toInt(); index++) {
            const Element *srcElement = qjsvalue_cast<Element *>(branch->details().property(index).property("element"));
            QVariantMap srcEnv = qjsvalue_cast<QVariantMap>(branch->details().property(index).property("env"));

            if (!srcElement) {
                error() << "index" << index << "of" << branch << "has no element";
                continue;
            }

            srcEnv[versionFieldName] = nextVersion;

            Element *destElement = new Element;
            destElement->setPattern(m_target->map(srcEnv));
            destElement->setFrames(srcElement->frames());

            QJSValue detail = newObject();
            detail.setProperty("element", newQObject(destElement));
            detail.setProperty("env", toScriptValue(srcEnv));
            m_details.setProperty(index, detail);
        }
        emit detailsChanged();
    }
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
        debug() << "findLastVersion got field" << versionField;
        debug() << "findLastVersion got branch" << branch;

        QStringList releasedPaths = branch->listMatchingPaths(data.toMap());

        debug() << "matched paths" << releasedPaths;

        int lastVersion = 0;

        foreach (QString path, releasedPaths) {
            QVariant releaseEnv = branch->parse(path);
            copious() << "parse got" << releaseEnv;
            if (releaseEnv.isValid() && releaseEnv.toMap().contains(versionField->name()))
            {
                int version = versionField->get(releaseEnv.toMap()).toInt();
                copious() << "got version" << version;
                if (version > lastVersion) {
                    lastVersion = version;
                }
            } else {
                error() << branch << "can't parse" << path;
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
        // we can safely assume that our details are up-to-date
        Q_ASSERT(m_details.isArray());
        Q_ASSERT(branch->details().isArray());
        Q_ASSERT(m_details.property("length").toInt() == branch->details().property("length").toInt());

        m_target->setCount(m_details.property("length").toInt());
        for (int i = 0; i < m_details.property("length").toInt(); i++)
            m_target->details().setProperty(i, m_details.property(i));
        emit m_target->detailsChanged();

        for (int index = 0; index < branch->details().property("length").toInt(); index++) {
            const Element *srcElement = qjsvalue_cast<Element *>(branch->details().property(index).property("element"));
            const Element *destElement = qjsvalue_cast<Element *>(details().property(index).property("element"));

            releaseElement(srcElement, destElement);
        }

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

void ReleaseOperationAttached::releaseElement(const Element *srcElement, const Element *destElement)
{
    if (srcElement->frameList()) {
        const QStringList srcPaths = srcElement->paths();
        const QStringList destPaths = destElement->paths();

        Q_ASSERT(srcPaths.length() == destPaths.length());

        for (int i = 0; i < srcPaths.length(); i++) {
            const QString srcPath = srcPaths.at(i);
            const QString destPath = destPaths.at(i);

            releaseFile(srcPath, destPath);
        }
    } else {
        releaseFile(srcElement->path(), destElement->path());
    }
}

void ReleaseOperationAttached::releaseFile(const QString srcPath, const QString destPath)
{
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
    }
}

void ReleaseOperationAttached::onFileOpQueueFinished()
{
    debug() << ".onFileOpQueueFinished";
    const ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
    const QString versionFieldName = targetAttached->findVersionFieldName();
    int version = targetAttached->findLastVersion(env());

    QVariantMap targetEnv = env();
    targetEnv[versionFieldName] = version;

    // update the parents
    BranchBase *p = m_target->firstParent<BranchBase>();
    while (p && !p->isRoot()) {
        debug() << "updating" << p;
        debug() << "target env is" << targetEnv;
        QStringList paths = p->listMatchingPaths(targetEnv);
        debug() << "got paths" << paths;
        p->setPaths(paths, targetEnv);

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
