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

File::File(QObject *parent) :
    Branch(parent),
    m_input(),
    m_linkType(File::Hard),
    m_frames(0)
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cookAtIndex, this, &File::onCookAtIndex);

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

    if (m_input && !isLocked()) {
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

        debug() << "matched" << matches.count() << "patterns";

        if (matches.size() > 0) {
            int index = 0;
            QMapIterator<QString, QSet<QFileInfo> > i(matches);
            while (i.hasNext()) {
                i.next();
                ElementView *element = elementsView->elementAt(index);
                element->setPattern(i.key());

                if (m_frames) {
                    FramePattern fp(m_frames->list());
                    element->scan(i.value().toList(), fp);
                } else {
                    element->scan(i.value().toList());
                }

                QStringList files;
                foreach (const QFileInfo info, i.value()) {
                    files << info.filePath();
                }

                debug() << "matched files" << files;

                const QVariantMap elementContext = parse(i.key()).toMap();

                setContext(index, mergeContexts(context.toMap(), elementContext));

                index++;
            }
        }
    }

    emit updateFinished(status);
}

void File::onCookAtIndex(int index, const QVariant context)
{
    trace() << ".onCookAtIndex(" << index << "," << context << ")";

    if (m_input && !isLocked()) {
        setCount(m_input->count());

        QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));
        ElementView *inputElement = inputElementsView->elementAt(index);
        Q_ASSERT(inputElement);

        FilePattern inputPattern = FilePattern(inputElement->pattern());

        QScopedPointer<ElementsView> elementsView(new ElementsView(this));
        ElementView *element = elementsView->elementAt(index);
        Q_ASSERT(element);

        const QVariantMap inputContext = mergeContexts(context.toMap(),
                m_input->details().property(index).property("context").toVariant().toMap());

        setContext(index, inputContext);

        if (fieldsComplete(inputContext)) {
            const FilePattern destPattern(map(inputContext));

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
                                emit cookAtIndexFinished(index, OperationAttached::Error);
                                return;
                            }
                        }
                    } else {
                        error() << "frames [" << m_frames->pattern() << "] do not match input element frames [" << inputElement->framePattern() << "]";
                        emit cookAtIndexFinished(index, OperationAttached::Error);
                        return;
                    }
                } else {
                    error() << "input is a sequence, but this node is not";
                    debug() << "inputPattern is" << inputPattern.pattern();
                    debug() << "destPattern is" << destPattern.pattern();
                    emit cookAtIndexFinished(index, OperationAttached::Error);
                    return;
                }
            } else {
                if (destPattern.isSequence()) {
                    // TODO: at some point we want to support this
                    // how? need a frame range for this node tho
                    error() << "this node is a sequence, but input node is not";
                    emit cookAtIndexFinished(index, OperationAttached::Error);
                } else {
                    debug() << "cooking element";
                    const QString srcPath = inputPattern.path();
                    const QString destPath = destPattern.path();

                    // TODO: dependency check
                    if (!makeLink(srcPath, destPath)) {
                        emit cookAtIndexFinished(index, OperationAttached::Error);
                        return;
                    }
                }
            }
        } else {
            error() << "fields incomplete for input context" << inputContext;
            emit cookAtIndexFinished(index, OperationAttached::Error);
            return;
        }
    }

    emit cookAtIndexFinished(index, OperationAttached::Finished);
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
