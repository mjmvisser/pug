#include <QFileInfo>

#include "releaseoperation.h"
#include "elementsview.h"
#include "filepattern.h"
#include "file.h"
#include "branch.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_source(),
    m_version(-1),
    m_mode(ReleaseOperationAttached::Copy),
    m_queue(new FileOpQueue(this))
{
    setObjectName("release");
    connect(m_queue, &FileOpQueue::finished, this, &ReleaseOperationAttached::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &ReleaseOperationAttached::onFileOpQueueError);

    File *file = qobject_cast<File *>(node());
    if (file) {
        file->addInput(this, "source");
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

void ReleaseOperationAttached::reset()
{
    OperationAttached::reset();
    m_version = -1;
    if (m_source) {
        // we hook here to calculate the current version
        resetVersion();
    }
}

void ReleaseOperationAttached::resetVersion()
{
    trace() << node() << ".resetVersion()";

    Branch *branch = qobject_cast<Branch *>(node());
    if (branch && !m_versionFieldName.isEmpty()) {
        // we're attached to a file with a version field name specified
        m_version = findLastVersion(context()) + 1;
    } else if (branch && branch->root()) {
        // recurse
        ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(rootAttached);
        rootAttached->resetVersion();
        m_version = rootAttached->m_version;
    } else {
        m_version = -1;
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
        QMap<QString, QFileInfoList> released = branch->listMatchingPatterns(context);
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
    Q_ASSERT(branch);
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
}

void ReleaseOperationAttached::run()
{
    trace() << node() << ".run() [update mode is" << m_mode << "]";
    Q_ASSERT(operation());
    Q_ASSERT(node());

    File *file = qobject_cast<File *>(node());

    if (m_source && file) {
        info() << "Releasing" << m_source << "to" << file << "with" << context();

        QString versionFieldName = findVersionFieldName();

        if (!versionFieldName.isEmpty()) {
            m_queue->setSudo(operation<ReleaseOperation>()->sudo());

            info() << "Releasing" << file << "to" << m_source;

            QScopedPointer<ElementsView> elementsView(new ElementsView(file));
            QScopedPointer<ElementsView> sourceElementsView(new ElementsView(m_source));

            for (int index = 0; index < sourceElementsView->elementCount(); index++) {
                ElementView *sourceElement = sourceElementsView->elementAt(index);
                ElementView *element = elementsView->elementAt(index);

                QVariantMap destContext = Node::mergeContexts(context(),
                        m_source->details().property(index).property("context").toVariant().toMap());

                // set version
                destContext.insert(versionFieldName, m_version);

                FilePattern srcPattern = FilePattern(sourceElement->pattern());
                FilePattern destPattern = FilePattern(file->map(destContext));

                element->setPattern(destPattern.pattern());
                file->setContext(index, destContext);

                if (srcPattern.isSequence()) {
                    if (destPattern.isSequence()) {
                        // source and destination are both sequences
                        if (!file->frames() || sourceElement->frameList() == file->frames()->list()) {
                            element->setFrameCount(element->frameCount());

                            for (int frameIndex = 0; frameIndex < sourceElement->frameCount(); frameIndex++) {
                                int frame = sourceElement->frameAt(frameIndex)->frame();

                                const QString srcPath = srcPattern.path(frame);
                                const QString destPath = destPattern.path(frame);

                                releaseFile(srcPath, destPath);
                            }
                        } else {
                            error() << "dest frames [" << file->frames()->list() << "] doesn't match source element [" << sourceElement->frameList() << "]";
                            setStatus(OperationAttached::Error);
                        }
                    } else {
                        error() << "source is a sequence, but dest is not";
                        setStatus(OperationAttached::Error);
                        break;
                    }
                } else {
                    if (destPattern.isSequence()) {
                        error() << "dest is a sequence, but source is not";
                        setStatus(OperationAttached::Error);
                        break;
                    } else {
                        const QString srcPath = srcPattern.path();
                        const QString destPath = destPattern.path();

                        releaseFile(srcPath, destPath);
                    }
                }
            }
        } else {
            error() << file << "Can't release, no versionField found";
            setStatus(OperationAttached::Error);
        }

        if (status() != OperationAttached::Error)
            m_queue->run();
        else
            continueRunning();

    } else {
        setStatus(OperationAttached::Finished);
        continueRunning();
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
