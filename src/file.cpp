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
#include "updateoperation.h"
#include "cookoperation.h"
#include "releaseoperation.h"
#include "input.h"
#include "output.h"


File::File(QObject *parent) :
    Branch(parent),
    m_input(),
    m_linkType(File::Hard),
    m_queue(new FileOpQueue(this))
{
    setExactMatch(true);

    connect(m_queue, &FileOpQueue::finished, this, &File::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &File::onFileOpQueueError);

    /*Input *input = */addInput(this, "input");
    //input->setDependency(Input::Frame);

    addParam(this, "linkType");
}

void File::componentComplete()
{
    Branch::componentComplete();

    connect(elementsView(), &ElementsView::elementsChanged, this, &File::elementsChanged);
    connect(updateOperationAttached(), &UpdateOperationAttached::cook, this, &File::update_onCook);
    connect(cookOperationAttached(), &CookOperationAttached::cook, this, &File::cook_onCook);
    connect(releaseOperationAttached(), &ReleaseOperationAttached::cook, this, &File::release_onCook);
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

        emit inputChanged(node);
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

QQmlListProperty<ElementView> File::elements_()
{
    return elementsView()->elements_();
}

void File::update_onCook()
{
    trace() << "UpdateOperation.onCook()";

    clearDetails();

    if (m_input) {
        setCount(m_input->count());

        debug() << "input count is" << m_input->count();
        debug() << "element manager count is" << elementsView()->elementCount();

        for (int index = 0; index < elementsView()->elementCount(); index++) {
            QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));
            ElementView *inputElement = inputElementsView->elementAt(index);
            Q_ASSERT(inputElement);

            const QVariantMap elementContext = mergeContexts(context(),
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

                    ElementView *element = elementsView()->elementAt(index);

                    element->setPattern(elementPattern.pattern());
                    element->scan(framesInfoList);
                }
            }
        }
    } else {
        QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(context());
        debug() << "matched patterns" << matches.keys();

        setCount(matches.size());

        info() << "matched" << matches.count() << "patterns";

        if (matches.size() > 0) {
            int index = 0;
            QMapIterator<QString, QSet<QFileInfo> > i(matches);
            while (i.hasNext()) {
                i.next();

                if (i.value().toList().length() > 0) {
                    ElementView *element = elementsView()->elementAt(index);
                    element->setPattern(i.key());
                    element->scan(i.value().toList());

                    const QVariantMap elementContext = parse(i.key()).toMap();

                    setContext(index, mergeContexts(context(), elementContext));
                }

                index++;
            }
        }
    }

    emit updateOperationAttached()->cookFinished();
}

void File::cook_onCook()
{
    trace() << "CookOperation.onCook()";

    if (m_input) {
        setCount(m_input->count());

        QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));

        for (int index = 0; index < count(); index++) {
            setIndex(index);
            ElementView *inputElement = inputElementsView->elementAt(index);
            Q_ASSERT(inputElement);

            FilePattern inputPattern = FilePattern(inputElement->pattern());

            ElementView *element = elementsView()->elementAt(index);
            Q_ASSERT(element);

            const QVariantMap elementContext = mergeContexts(context(),
                    m_input->details().property(index).property("context").toVariant().toMap());

            setContext(index, elementContext);

            if (fieldsComplete(elementContext)) {
                const FilePattern destPattern(map(elementContext));

                element->setPattern(destPattern.pattern());

                if (inputPattern.isSequence()) {
                    if (destPattern.isSequence()) {
                        // input and this are both sequences
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
                                emit cookOperationAttached()->cookFinished();
                                return;
                            }
                        }
                    } else {
                        error() << "input is a sequence, but this node is not";
                        debug() << "inputPattern is" << inputPattern.pattern();
                        debug() << "destPattern is" << destPattern.pattern();
                        emit cookOperationAttached()->cookFinished();
                        return;
                    }
                } else {
                    if (destPattern.isSequence()) {
                        // TODO: at some point we want to support this
                        // how? need a frame range for this node tho
                        error() << "this node is a sequence, but input node is not";
                        emit cookOperationAttached()->cookFinished();
                    } else {
                        debug() << "cooking element";
                        const QString srcPath = inputPattern.path();
                        const QString destPath = destPattern.path();

                        // TODO: dependency check
                        if (!makeLink(srcPath, destPath)) {
                            emit cookOperationAttached()->cookFinished();
                            return;
                        }
                    }
                }
            } else {
                error() << "fields incomplete for input context" << elementContext;
                emit cookOperationAttached()->cookFinished();
                return;
            }
        }
    }

    emit cookOperationAttached()->cookFinished();
}

ElementsView *File::elementsView()
{
    return attachedPropertiesObject<File, ElementsView>();
}

UpdateOperationAttached *File::updateOperationAttached()
{
    return attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>();
}

CookOperationAttached *File::cookOperationAttached()
{
    return attachedPropertiesObject<CookOperation, CookOperationAttached>();
}

ReleaseOperationAttached *File::releaseOperationAttached()
{
    return attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>();
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

void File::release_onCook()
{
    trace() << "ReleaseOperation.onCook()";

    if (releaseOperationAttached()->source()) {
        clearDetails();

        m_queue->setSudo(releaseOperationAttached()->sudo());

        QScopedPointer<ElementsView> sourceElementsView(new ElementsView(releaseOperationAttached()->source()));

        int index = 0;
        setCount(sourceElementsView->elementCount());

        for (int sourceIndex = 0; sourceIndex < sourceElementsView->elementCount(); sourceIndex++) {
            ElementView *sourceElement = sourceElementsView->elementAt(sourceIndex);
            ElementView *element = elementsView()->elementAt(index);

            QVariantMap destContext = mergeContexts(context(),
                    releaseOperationAttached()->source()->details().property(index).property("context").toVariant().toMap());

            FilePattern srcPattern = FilePattern(sourceElement->pattern());
            FilePattern destPattern = FilePattern(map(destContext));

            element->setPattern(destPattern.pattern());
            setContext(index, destContext);

            if (srcPattern.isSequence()) {
                if (destPattern.isSequence()) {
                    // source and destination are both sequences
                    element->setFrameCount(sourceElement->frameCount());

                    for (int frameIndex = 0; frameIndex < sourceElement->frameCount(); frameIndex++) {
                        int frame = sourceElement->frameAt(frameIndex)->frame();
                        element->frameAt(frameIndex)->setFrame(frame);

                        const QString srcPath = srcPattern.path(frame);
                        const QString destPath = destPattern.path(frame);

                        // TODO: preserve timestamps?
                        releaseFile(srcPath, destPath, releaseOperationAttached()->mode());
                    }
                } else {
                    error() << "source" << srcPattern.pattern() << "is a sequence, but dest" << destPattern.pattern() << "is not";
                    break;
                }
            } else {
                if (destPattern.isSequence()) {
                    error() << "dest" << destPattern.pattern() << "is a sequence, but source" << srcPattern.pattern() << "is not";
                    break;
                } else {
                    const QString srcPath = srcPattern.path();
                    const QString destPath = destPattern.path();

                    releaseFile(srcPath, destPath, releaseOperationAttached()->mode());
                }
            }

            index++;
        }

        if (log()->maxLevel() < Log::Error)
            m_queue->run();
    } else {
        emit releaseOperationAttached()->cookFinished();
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
    emit attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>()->cookFinished();
}

void File::onFileOpQueueError()
{
    trace() << ".onFileOpQueueError()";
    emit attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>()->cookFinished();
}

ElementsView *File::qmlAttachedProperties(QObject *parent)
{
    Node *node = qobject_cast<Node *>(parent);
    Q_ASSERT(node);
    return new ElementsView(node);
}
