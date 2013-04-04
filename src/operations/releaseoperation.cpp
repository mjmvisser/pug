#include <QFileInfo>

#include "releaseoperation.h"
#include "elementsview.h"
#include "filepattern.h"
#include "file.h"
#include "branch.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_releasable(false),
    m_source(),
    m_version(-1),
    m_mode(ReleaseOperationAttached::Copy)
{
    setObjectName("release");

    File *file = qobject_cast<File *>(node());
    if (file) {
        file->addInput(this, "source");
    }

    bool haveReleaseSignal = node()->hasSignal(SIGNAL(release(const QVariant)));
    bool haveReleaseFinishedSignal = node()->hasSignal(SIGNAL(releaseFinished(int)));

    if (haveReleaseSignal && haveReleaseFinishedSignal) {
        m_releasable = true;
        debug() << node() << "found release and releaseFinished signals";

        connect(this, SIGNAL(release(const QVariant)),
                node(), SIGNAL(release(const QVariant)));
        connect(node(), SIGNAL(releaseFinished(int)),
                this, SLOT(onReleaseFinished(int)));
    } else if (!haveReleaseSignal && haveReleaseFinishedSignal) {
        error() << node() << "is missing the release signal";
    } else if (haveReleaseSignal && !haveReleaseFinishedSignal) {
        error() << node() << "is missing the releaseFinished signal";
    } else {
        debug() << node() << "is not releasable";
    }
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

void ReleaseOperationAttached::reset()
{
    OperationAttached::reset();
    m_version = -1;
    //if (m_source) {
        // we hook here to calculate the current version
        resetVersion();
    //}
}

void ReleaseOperationAttached::resetVersion()
{
    trace() << node() << ".resetVersion()";

    Branch *branch = qobject_cast<Branch *>(node());
    if (branch && !m_versionFieldName.isEmpty()) {
        // we're attached to a file with a version field name specified
        m_version = findLastVersion(context()) + 1;
        debug() << branch << "next version is" << m_version;
    } else if (branch && branch->root()) {
        // recurse
        ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(rootAttached);
        //rootAttached->resetVersion();
        m_version = rootAttached->m_version;
        debug() << branch << "next version is" << m_version;
    } else if (node()->parent<Node>()) {
        ReleaseOperationAttached *parentAttached = node()->parent<Node>()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        //parentAttached->resetVersion();
        m_version = parentAttached->m_version;
        debug() << branch << "next version is" << m_version;
    }
}

int ReleaseOperationAttached::findLastVersion(const QVariantMap context) const
{
    trace() << node() << ".findLastVersion()";

    Q_ASSERT(!m_versionFieldName.isEmpty());

    const Branch *branch = qobject_cast<const Branch *>(node());
    Q_ASSERT(branch);

    const Field *versionField = branch->findField(m_versionFieldName);

    if (versionField) {
        QMap<QString, QSet<QFileInfo> > released = branch->listMatchingPatterns(context);
        int lastVersion = 0;

        foreach (QString path, released.keys()) {
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
        error() << "versionField" << m_versionFieldName << "on" << branch << "is not valid";
        return -1;
    }
}

const QString ReleaseOperationAttached::findVersionFieldName() const
{
    const Branch *branch = qobject_cast<const Branch *>(node());
    if (branch) {
        if (!m_versionFieldName.isEmpty()) {
            // we're attached to a branch with a version field name specified
            return m_versionFieldName;
        } else if (branch->root()) {
            // recurse
            const ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
            Q_ASSERT(rootAttached);
            return rootAttached->findVersionFieldName();
        } else {
            return QString();
        }
    } else {
        return QString();
    }
}

void ReleaseOperationAttached::run()
{
    trace() << node() << ".run()";

    if (m_releasable) {
        Q_ASSERT(operation());
        Q_ASSERT(node());

        QString versionFieldName = findVersionFieldName();

        QVariantMap releaseContext = context();

        if (!versionFieldName.isEmpty() && m_version >= 0) {
            debug() << node() << "setting" << versionFieldName << "to" << m_version;
            releaseContext.insert(versionFieldName, m_version);
        } else {
            debug() << node() << "can't set version: field name is" << versionFieldName << ", version is" << m_version;
        }

        Q_ASSERT(receivers(SIGNAL(release(QVariant))) > 0);
        Q_ASSERT(node()->receivers(SIGNAL(releaseFinished(int))) > 0);
        emit release(releaseContext);
    } else {
        debug() << node() << "skipping";
        setStatus(OperationAttached::Finished);
        continueRunning();
    }
}

void ReleaseOperationAttached::onReleaseFinished(int s)
{
    trace() << node() << ".onReleaseFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "ReleaseFinished" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

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
