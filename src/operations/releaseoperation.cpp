#include "releaseoperation.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_target(),
    m_version(-1),
    m_mode(ReleaseOperationAttached::Copy),
    m_queue(new FileOpQueue(this))
{
    connect(m_queue, &FileOpQueue::finished, this, &ReleaseOperationAttached::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &ReleaseOperationAttached::onFileOpQueueError);

    connect(this, &ReleaseOperationAttached::targetChanged, this, &ReleaseOperationAttached::regenerateDetails);
    connect(node(), &Node::detailsChanged, this, &ReleaseOperationAttached::regenerateDetails);
}

Branch *ReleaseOperationAttached::target()
{
    return m_target;
}

const Branch *ReleaseOperationAttached::target() const
{
    return m_target;
}

void ReleaseOperationAttached::setTarget(Branch *branch)
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
    m_version = -1;
    if (m_target) {
        ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);
        targetAttached->resetVersion();
    }
}

void ReleaseOperationAttached::regenerateDetails()
{
    // regenerate our details

    if (node()->details().isArray() && m_target) {
        trace() << node() << ".regenerateDetails()";
        m_details = newArray();

        ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        const QString versionFieldName = targetAttached->findVersionFieldName();

        for (int index = 0; index < node()->details().property("length").toInt(); index++) {
            const Element *srcElement = qjsvalue_cast<Element *>(node()->details().property(index).property("element"));
            QVariantMap srcContext = qjsvalue_cast<QVariantMap>(node()->details().property(index).property("context"));

            if (!srcElement) {
                error() << "index" << index << "of" << node() << "has no element";
                continue;
            }

            int version = targetAttached->version(srcContext);

            srcContext[versionFieldName] = version;

            Element *destElement = new Element;

            destElement->setPattern(m_target->map(srcContext));
            destElement->setFrames(srcElement->frames());

            QJSValue detail = newObject();
            detail.setProperty("element", newQObject(destElement));
            detail.setProperty("context", toScriptValue(srcContext));
            m_details.setProperty(index, detail);

            debug() << node() << "set detail" << index << "element to" << destElement->toString() << "context to" << srcContext;

        }
        emit detailsChanged();
    }
}

const QString ReleaseOperationAttached::findVersionFieldName() const
{
    const Branch *branch = qobject_cast<const Branch *>(node());
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
    trace() << node() << ".findLastVersion(" << data << ")";

    // find branch parent with version field
    const Branch *branch = qobject_cast<const Branch *>(node());

    if (!branch || m_versionFieldName.isEmpty()) {
        error() << node() << "can't find last version on a non-branch or branch without valid ReleaseOperation.versionField";
        return -1;
    }

    const Field *versionField = branch->findField(m_versionFieldName);

    if (versionField) {
        QStringList releasedPaths = branch->listMatchingPaths(data.toMap());
        int lastVersion = 0;

        foreach (QString path, releasedPaths) {
            QVariant releaseContext = branch->parse(path);
            if (releaseContext.isValid() && releaseContext.toMap().contains(versionField->name()))
            {
                int version = versionField->get(releaseContext.toMap()).toInt();
                if (version > lastVersion) {
                    lastVersion = version;
                }
            } else {
                error() << branch << "can't parse" << path;
            }
        }

        debug() << branch << "findLastVersion got version" << lastVersion;

        return lastVersion;
    } else {
        error() << "versionField" << m_versionFieldName << "on" << node() << "is not valid";
        return -1;
    }
}

int ReleaseOperationAttached::version(const QVariant context)
{
    trace() << node() << ".version(" << context << ")";

    Branch *branch = qobject_cast<Branch *>(node());
    if (branch && !m_versionFieldName.isEmpty()) {
        // we're attached to a branch with a version field name specified
        if (m_version == -1) {
            m_version = findLastVersion(context) + 1;
        }
        return m_version;
    } else if (branch->root()) {
        // recurse
        ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        return rootAttached->version(context);
    } else {
        error() << node() << "no parent branch found with valid versionField";
        return -1;
    }
}

void ReleaseOperationAttached::resetVersion()
{
    trace() << node() << ".resetVersion()";

    Branch *branch = qobject_cast<Branch *>(node());
    if (branch && !m_versionFieldName.isEmpty()) {
        // we're attached to a branch with a version field name specified
        m_version = -1;
    } else if (branch->root()) {
        // recurse
        ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(rootAttached);
        return rootAttached->resetVersion();
    }
}

void ReleaseOperationAttached::run()
{
    trace() << node() << ".run()";
    Q_ASSERT(operation());

    if (node() && m_target) {
        m_queue->setSudo(operation<ReleaseOperation>()->sudo());

        info() << "Releasing" << node() << "to" << m_target;
        // we can safely assume that our details are up-to-date
        Q_ASSERT(m_details.isArray());
        Q_ASSERT(node()->details().isArray());
        Q_ASSERT(m_details.property("length").toInt() == node()->details().property("length").toInt());

        // copy the local detail to the target detail
        m_target->setCount(m_details.property("length").toInt());
        for (int i = 0; i < m_details.property("length").toInt(); i++) {
            m_target->details().setProperty(i, m_details.property(i));
        }
        emit m_target->detailsChanged();

        for (int index = 0; index < node()->details().property("length").toInt(); index++) {
            const Element *srcElement = qjsvalue_cast<Element *>(node()->details().property(index).property("element"));
            const Element *destElement = qjsvalue_cast<Element *>(details().property(index).property("element"));

            releaseElement(srcElement, destElement);
        }

        m_queue->run();

    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void ReleaseOperationAttached::releaseElement(const Element *srcElement, const Element *destElement)
{
    info() << node() << "releasing" << srcElement << "to" << destElement;
    trace() << node() << ".releaseElement(" << srcElement << "," << destElement << ")";

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
    trace() << node() << ".releaseFile(" << srcPath << "," << destPath << ")";
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
    trace() << node() << ".onFileOpQueueFinished()";
    ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
    Q_ASSERT(targetAttached);
    const QString versionFieldName = targetAttached->findVersionFieldName();
    int version = targetAttached->version(context());

    QVariantMap targetContext = context();
    targetContext[versionFieldName] = version;

    // update the parents
    Branch *p = m_target->firstParent<Branch>();
    while (p && !p->isRoot()) {
        QStringList paths = p->listMatchingPaths(targetContext);
        p->setPaths(paths, targetContext);

        p = p->firstParent<Branch>();
   }

    setStatus(OperationAttached::Finished);
    continueRunning();
}

void ReleaseOperationAttached::onFileOpQueueError()
{
    trace() << node() << ".onFileOpQueueError()";
    setStatus(OperationAttached::Error);
    continueRunning();
}

const QMetaObject *ReleaseOperationAttached::operationMetaObject() const
{
    return &ReleaseOperation::staticMetaObject;
}

ReleaseOperation::ReleaseOperation(QObject *parent) :
    Operation(parent),
    m_sudo()
{
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
