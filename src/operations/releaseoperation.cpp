#include <QFileInfo>

#include "releaseoperation.h"
#include "elementsview.h"
#include "filepattern.h"

ReleaseOperationAttached::ReleaseOperationAttached(QObject *parent) :
    OperationAttached(parent),
    m_target(),
    m_version(-1),
    m_mode(ReleaseOperationAttached::Copy),
    m_queue(new FileOpQueue(this))
{
    connect(m_queue, &FileOpQueue::finished, this, &ReleaseOperationAttached::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &ReleaseOperationAttached::onFileOpQueueError);
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

void ReleaseOperationAttached::reset()
{
    OperationAttached::reset();
    m_version = -1;
    if (m_target) {
        // we hook here to calculate the current version
        ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);
        targetAttached->resetVersion(context());
    }
}

void ReleaseOperationAttached::resetVersion(const QVariantMap context)
{
    trace() << node() << ".resetVersion()";

    Branch *branch = qobject_cast<Branch *>(node());
    if (branch && !m_versionFieldName.isEmpty()) {
        // we're attached to a branch with a version field name specified
        m_version = findLastVersion(context) + 1;
    } else if (branch && branch->root()) {
        // recurse
        ReleaseOperationAttached *rootAttached = branch->root()->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(rootAttached);
        rootAttached->resetVersion(context);
        m_version = rootAttached->m_version;
    } else {
        error() << "No version field found on target.";
        m_version = -1;
    }
}

int ReleaseOperationAttached::findLastVersion(const QVariantMap context) const
{
    trace() << node() << ".findLastVersion(" << context << ")";

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
        error() << "versionField" << m_versionFieldName << "on" << node() << "is not valid";
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
        error() << "No version field found on target.";
        return QString();
    }
}

void ReleaseOperationAttached::run()
{
    trace() << node() << ".run()";
    Q_ASSERT(operation());
    Q_ASSERT(node());

    if (m_target) {
        ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
        Q_ASSERT(targetAttached);

        QString versionFieldName = targetAttached->findVersionFieldName();

        if (!versionFieldName.isEmpty()) {
            m_queue->setSudo(operation<ReleaseOperation>()->sudo());

            info() << "Releasing" << node() << "to" << m_target;

            m_target->setCount(node()->count());

            QScopedPointer<ElementsView> elementsView(new ElementsView(node()));
            QScopedPointer<ElementsView> targetElementsView(new ElementsView(m_target));

            for (int index = 0; index < elementsView->elementCount(); index++) {
                ElementView *element = elementsView->elementAt(index);
                ElementView *targetElement = targetElementsView->elementAt(index);

                QVariantMap targetContext = node()->context(index);
                // override with calling context
                QMapIterator<QString, QVariant> i(context());
                while (i.hasNext()) {
                    i.next();
                    targetContext.insert(i.key(), i.value());
                }

                // set version
                targetContext.insert(versionFieldName, targetAttached->m_version);

                FilePattern srcPattern = FilePattern(element->pattern());
                FilePattern targetPattern = FilePattern(m_target->map(targetContext));

                targetElement->setPattern(targetPattern.pattern());
                m_target->setContext(index, targetContext);

                if (srcPattern.isSequence()) {
                    if (targetPattern.isSequence()) {
                        // this and target are both sequences
                        targetElement->setFrameCount(element->frameCount());

                        for (int frameIndex = 0; frameIndex < element->frameCount(); frameIndex++) {
                            FrameView *frame = element->frameAt(frameIndex);
                            Q_ASSERT(frame);

                            const QString srcPath = srcPattern.path(frame->frame());
                            const QString targetPath = targetPattern.path(frame->frame());

                            releaseFile(srcPath, targetPath);
                        }
                    } else {
                        error() << "this node is a sequence, but target is not";
                        setStatus(OperationAttached::Error);
                        break;
                    }
                } else {
                    if (targetPattern.isSequence()) {
                        error() << "target is a sequence, but this node is not";
                        setStatus(OperationAttached::Error);
                        break;
                    } else {
                        const QString srcPath = srcPattern.path();
                        const QString targetPath = targetPattern.path();

                        releaseFile(srcPath, targetPath);
                    }
                }
            }
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
    ReleaseOperationAttached *targetAttached = m_target->attachedPropertiesObject<ReleaseOperationAttached>(operationMetaObject());
    Q_ASSERT(targetAttached);

    QScopedPointer<ElementsView> targetElementsView(new ElementsView(m_target));

    for (int index = 0; index < targetElementsView->elementCount(); index++) {
        ElementView *targetElement = targetElementsView->elementAt(index);

        QVariantMap targetContext = m_target->context(index);

        QMap<QString, QFileInfoList> matches = m_target->listMatchingPatterns(targetContext);
        Q_ASSERT(matches.count() == 1);

        targetElement->scan(matches.values()[0]);
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
