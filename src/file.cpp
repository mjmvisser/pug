#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QQmlProperty>

#include <unistd.h>
#include <errno.h>

#include "file.h"
#include "elementsview.h"
#include "elementview.h"
#include "frameview.h"
#include "filepattern.h"
#include "releaseoperation.h"

File::File(QObject *parent) :
    Branch(parent),
    m_input(),
    m_linkType(File::Hard),
    m_frames(0),
    m_queue(new FileOpQueue(this))
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cook, this, &File::onCook);

    // release connections
    connect(this, &File::release, this, &File::onRelease);
    connect(m_queue, &FileOpQueue::finished, this, &File::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &File::onFileOpQueueError);

    Input *input = addInput(this, "input");
    input->setDependency(Input::Frame);

    addParam(this, "linkType");
}

Node *File::input()
{
    return m_input;
}
const Node *File::input() const
{
    return m_input;
}

void File::setInput(Node *node)
{
    if (m_input != node) {
        m_input = node;

        emit inputChanged(m_input);
    }
}

File::LinkType File::linkType() const
{
    return m_linkType;
}

void File::setLinkType(File::LinkType lt)
{
    if (m_linkType != lt) {
        m_linkType = lt;
        emit linkTypeChanged(lt);
    }
}

FrameList *File::frames()
{
    return m_frames;
}

void File::setFrames(FrameList *f)
{
    // forward framesChanged signal from FrameList to ourself
    if (!m_frames || !f || m_frames->list() != f->list()) {
        if (m_frames)
            disconnect(m_frames, &FrameList::framesChanged, this, &File::framesChanged);

        if (f)
            connect(f, &FrameList::framesChanged, this, &File::framesChanged);

        m_frames = f;

        emit framesChanged(f);
    }
}

void File::onUpdate(const QVariant context)
{
    trace() << ".onUpdate(" << context << ")";

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    OperationAttached::Status status = OperationAttached::Finished;

    clearDetails();

    if (m_input) {
        setCount(m_input->count());

        debug() << "input count is" << m_input->count();
        debug() << "element manager count is" << elementsView->elementCount();

        for (int index = 0; index < elementsView->elementCount(); index++) {
            QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));
            ElementView *inputElement = inputElementsView->elementAt(index);
            Q_ASSERT(inputElement);

            const QVariantMap elementContext = mergeContexts(context.toMap(),
                    m_input->details().property(index).property("context").toVariant().toMap());

            setContext(index, elementContext);

            if (fieldsComplete(elementContext)) {
                const FilePattern elementPattern(map(elementContext));
                debug() << "elementPattern is" << elementPattern.pattern();

                QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(elementContext);
                debug() << "matched patterns" << matches.keys();

                // TODO: handle possibility where matches.count() > 1
                Q_ASSERT(matches.count() <= 1);

                if (matches.count() == 1) {
                    Q_ASSERT(matches.contains(elementPattern.pattern()));

                    QFileInfoList framesInfoList = matches[elementPattern.pattern()].toList();

                    ElementView *element = elementsView->elementAt(index);

                    element->setPattern(elementPattern.pattern());

                    if (m_frames) {
                        if (m_frames->list() != inputElement->frameList()) {
                            error() << "frames [" << m_frames->pattern() << "] don't match input frames [" << inputElement->framePattern() << "]";
                            status = OperationAttached::Error;
                        } else {
                            FramePattern fp(m_frames->list());
                            element->scan(framesInfoList, fp);
                        }
                    } else {
                        element->scan(framesInfoList);
                    }
                }
            }
        }
    } else {
        QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(context.toMap());
        debug() << "matched patterns" << matches.keys();

        setCount(matches.size());

        info() << "matched" << matches.count() << "patterns";

        if (matches.size() > 0) {
            int index = 0;
            QMapIterator<QString, QSet<QFileInfo> > i(matches);
            while (i.hasNext()) {
                i.next();

                if (i.value().toList().length() > 0) {
                    ElementView *element = elementsView->elementAt(index);
                    element->setPattern(i.key());

                    if (m_frames) {
                        FramePattern fp(m_frames->list());
                        element->scan(i.value().toList(), fp);
                    } else {
                        element->scan(i.value().toList());
                    }

                    const QVariantMap elementContext = parse(i.key()).toMap();

                    setContext(index, mergeContexts(context.toMap(), elementContext));
                }

                index++;
            }
        }
    }

    emit updateFinished(status);
}

void File::onCook(const QVariant context)
{
    trace() << ".onCook(" << context << ")";

    if (m_input) {
        setCount(m_input->count());

        QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));

        for (int index = 0; index < count(); index++) {
            setIndex(index);
            ElementView *inputElement = inputElementsView->elementAt(index);
            Q_ASSERT(inputElement);

            FilePattern inputPattern = FilePattern(inputElement->pattern());

            QScopedPointer<ElementsView> elementsView(new ElementsView(this));
            ElementView *element = elementsView->elementAt(index);
            Q_ASSERT(element);

            const QVariantMap elementContext = mergeContexts(context.toMap(),
                    m_input->details().property(index).property("context").toVariant().toMap());

            setContext(index, elementContext);

            if (fieldsComplete(elementContext)) {
                const FilePattern destPattern(map(elementContext));

                element->setPattern(destPattern.pattern());

                if (inputPattern.isSequence()) {
                    if (destPattern.isSequence()) {
                        // input and this are both sequences
                        if (!m_frames || inputElement->frameList() == m_frames->list()) {
                            // frames match
                            element->setFrameCount(inputElement->frameCount());
                            for (int frameIndex = 0; frameIndex < inputElement->frameCount(); frameIndex++) {
                                debug() << "cooking frameIndex" << frameIndex;
                                FrameView *frame = element->frameAt(frameIndex);
                                FrameView *inputFrame = inputElement->frameAt(frameIndex);
                                Q_ASSERT(frame);
                                Q_ASSERT(inputFrame);

                                frame->setFrame(inputFrame->frame());

                                const QString srcPath = inputPattern.path(inputFrame->frame());
                                const QString destPath = destPattern.path(inputFrame->frame());

                                // TODO: dependency check
                                if (!makeLink(srcPath, destPath)) {
                                    emit cook(OperationAttached::Error);
                                    return;
                                }
                            }
                        } else {
                            error() << "frames [" << m_frames->pattern() << "] do not match input element frames [" << inputElement->framePattern() << "]";
                            emit cookFinished(OperationAttached::Error);
                            return;
                        }
                    } else {
                        error() << "input is a sequence, but this node is not";
                        debug() << "inputPattern is" << inputPattern.pattern();
                        debug() << "destPattern is" << destPattern.pattern();
                        emit cookFinished(OperationAttached::Error);
                        return;
                    }
                } else {
                    if (destPattern.isSequence()) {
                        // TODO: at some point we want to support this
                        // how? need a frame range for this node tho
                        error() << "this node is a sequence, but input node is not";
                        emit cookFinished(OperationAttached::Error);
                    } else {
                        debug() << "cooking element";
                        const QString srcPath = inputPattern.path();
                        const QString destPath = destPattern.path();

                        // TODO: dependency check
                        if (!makeLink(srcPath, destPath)) {
                            emit cookFinished(OperationAttached::Error);
                            return;
                        }
                    }
                }
            } else {
                error() << "fields incomplete for input context" << elementContext;
                emit cookFinished(OperationAttached::Error);
                return;
            }
        }
    }

    emit cookFinished(OperationAttached::Finished);
}

bool File::makeLink(const QString &src, const QString &dest) const
{
    debug() << "linking" << src << "to" << dest;

    bool success = true;

    // make sure the destination directory exists
    QFileInfo destInfo(dest);
    QDir().mkpath(destInfo.dir().absolutePath());

    if (QFileInfo(dest).exists()) {
        debug() << "removing existing file" << dest;
        if (!QFile(dest).remove()) {
            error() << "couldn't remove existing file" << dest;
            return false;
        }
    }

    switch (m_linkType) {
    case File::Hard:
        // META TODO: no cross-platform hardlink support in Qt
        if (link(src.toUtf8().constData(), dest.toUtf8().constData()) != 0) {
            error() << "link failed:" << dest << "->" << src;
            error() << "error is:" << strerror(errno);
            success = false;
        }
        break;
    case File::Symbolic:
        if (symlink(destInfo.dir().relativeFilePath(src).toUtf8().constData(), dest.toUtf8().constData()) != 0) {
            error() << "symlink failed:" << dest << "->" << src;
            error() << "error is:" << strerror(errno);
            success = false;
        }
        break;

    default:
        Q_ASSERT(false);
        success = false;
        break;
    }

    return success;
}

void File::onRelease(const QVariant context)
{
    trace() << ".onRelease(" << context << ")";

    ReleaseOperationAttached *attached = attachedPropertiesObject<ReleaseOperationAttached>(&ReleaseOperation::staticMetaObject);

    if (attached->source()) {
        clearDetails();

        OperationAttached::Status status = OperationAttached::Running;

        m_queue->setSudo(attached->sudo());

        QScopedPointer<ElementsView> elementsView(new ElementsView(this));
        QScopedPointer<ElementsView> sourceElementsView(new ElementsView(attached->source()));

        int index = 0;
        setCount(sourceElementsView->elementCount());

        for (int sourceIndex = 0; sourceIndex < sourceElementsView->elementCount(); sourceIndex++) {
            ElementView *sourceElement = sourceElementsView->elementAt(sourceIndex);
            ElementView *element = elementsView->elementAt(index);

            QVariantMap destContext = mergeContexts(context.toMap(),
                    attached->source()->details().property(index).property("context").toVariant().toMap());

            FilePattern srcPattern = FilePattern(sourceElement->pattern());
            FilePattern destPattern = FilePattern(map(destContext));

            element->setPattern(destPattern.pattern());
            setContext(index, destContext);

            if (srcPattern.isSequence()) {
                if (destPattern.isSequence()) {
                    // source and destination are both sequences
                    if (!frames() || sourceElement->frameList() == frames()->list()) {
                        element->setFrameCount(element->frameCount());

                        for (int frameIndex = 0; frameIndex < sourceElement->frameCount(); frameIndex++) {
                            int frame = sourceElement->frameAt(frameIndex)->frame();

                            const QString srcPath = srcPattern.path(frame);
                            const QString destPath = destPattern.path(frame);

                            releaseFile(srcPath, destPath, attached->mode());
                        }
                    } else {
                        error() << "dest frames [" << frames()->list() << "] doesn't match source element [" << sourceElement->frameList() << "]";
                        status = OperationAttached::Error;
                    }
                } else {
                    error() << "source is a sequence, but dest is not";
                    status = OperationAttached::Error;
                    break;
                }
            } else {
                if (destPattern.isSequence()) {
                    error() << "dest is a sequence, but source is not";
                    status = OperationAttached::Error;
                    break;
                } else {
                    const QString srcPath = srcPattern.path();
                    const QString destPath = destPattern.path();

                    releaseFile(srcPath, destPath, attached->mode());
                }
            }

            index++;
        }

        if (status == OperationAttached::Error)
            emit releaseFinished(status);
        else
            m_queue->run();

    } else {
        emit releaseFinished(OperationAttached::Finished);
    }
}

void File::releaseFile(const QString srcPath, const QString destPath, int mode) const
{
    trace() << ".releaseFile(" << srcPath << "," << destPath << ")";
    if (!destPath.isEmpty()) {
        QFileInfo destInfo(destPath);
        if (!destInfo.absoluteDir().exists())
            m_queue->mkdir(destInfo.absoluteDir().absolutePath());

        switch (mode) {
        case ReleaseOperationAttached::Copy:
            m_queue->copy(srcPath, destPath);
            break;

        case ReleaseOperationAttached::Move:
            m_queue->move(srcPath, destPath);
            break;

        default:
            error() << this << "unknown file mode " << mode;
            break;
        }
    }
}

void File::onFileOpQueueFinished()
{
    trace() << ".onFileOpQueueFinished()";
    emit releaseFinished(OperationAttached::Finished);
}

void File::onFileOpQueueError()
{
    trace() << ".onFileOpQueueError()";
    emit releaseFinished(OperationAttached::Error);
}
