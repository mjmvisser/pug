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
    m_linkType(File::Hard)
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cookAtIndex, this, &File::onCookAtIndex);

    Input *input = addInput("input");
    input->setDependency(Input::Frame);
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

void File::onUpdate(const QVariant context)
{
    trace() << ".onUpdate(" << context << ")";

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    if (m_input) {
        setCount(m_input->count());

        debug() << "input count is" << m_input->count();
        debug() << "element manager count is" << elementsView->elementCount();

        for (int index = 0; index < elementsView->elementCount(); index++) {
            QVariantMap inputContext = m_input->context(index);
            // override with calling context
            QMapIterator<QString, QVariant> i(context.toMap());
            while (i.hasNext()) {
                i.next();
                inputContext.insert(i.key(), i.value());
            }

            debug() << "inputContext is" << inputContext;

            if (fieldsComplete(pattern(), inputContext)) {
                const FilePattern elementPattern(map(inputContext));
                debug() << "elementPattern is" << elementPattern.pattern();

                QMap<QString, QFileInfoList> matches = listMatchingPatterns(inputContext);
                debug() << "matched patterns" << matches.keys();

                Q_ASSERT(matches.count() == 1);
                Q_ASSERT(matches.contains(elementPattern.pattern()));

                ElementView *element = elementsView->elementAt(index);

                element->setPattern(elementPattern.pattern());
                element->scan(matches[elementPattern.pattern()]);
            }
        }
    } else {
        QMap<QString, QFileInfoList> matches = listMatchingPatterns(context.toMap());
        debug() << "matched patterns" << matches.keys();

        setCount(matches.size());

        int index = 0;
        QMapIterator<QString, QFileInfoList> i(matches);
        while (i.hasNext()) {
            i.next();
            ElementView *element = elementsView->elementAt(index);
            element->setPattern(i.key());
            element->scan(i.value());

            QStringList files;
            foreach (const QFileInfo info, i.value()) {
                files << info.filePath();
            }

            debug() << "matched files" << files;

            QVariantMap elementContext = parse(i.key()).toMap();
            // override with calling context
            QMapIterator<QString, QVariant> i(context.toMap());
            while (i.hasNext()) {
                i.next();
                elementContext.insert(i.key(), i.value());
            }
            setContext(index, elementContext);

            index++;
        }
    }

    emit updated(OperationAttached::Finished);
}

void File::onCookAtIndex(int index, const QVariant context)
{
    trace() << ".onCookAtIndex(" << index << "," << context << ")";

    if (m_input) {
        QScopedPointer<ElementsView> inputElementsView(new ElementsView(m_input));
        ElementView *inputElement = inputElementsView->elementAt(index);
        Q_ASSERT(inputElement);

        FilePattern inputPattern = FilePattern(inputElement->pattern());

        QScopedPointer<ElementsView> elementsView(new ElementsView(this));
        ElementView *element = elementsView->elementAt(index);
        Q_ASSERT(element);

        QVariantMap inputContext = m_input->context(index);
        // override with calling context
        QMapIterator<QString, QVariant> i(context.toMap());
        while (i.hasNext()) {
            i.next();
            inputContext.insert(i.key(), i.value());
        }

        debug() << "inputContext is" << inputContext;

        if (fieldsComplete(pattern(), inputContext)) {
            const FilePattern destPattern(map(inputContext));

            if (inputPattern.isSequence()) {
                if (destPattern.isSequence()) {
                    // input and this are both sequences
                    for (int frameIndex = 0; frameIndex < inputElement->frameCount(); frameIndex++) {
                        debug() << "cooking frameIndex" << frameIndex;
                        FrameView *inputFrame = inputElement->frameAt(frameIndex);
                        Q_ASSERT(inputFrame);

                        const QString srcPath = inputPattern.path(inputFrame->frame());
                        const QString destPath = destPattern.path(inputFrame->frame());

                        // TODO: dependency check
                        if (!makeLink(srcPath, destPath)) {
                            emit cookedAtIndex(index, OperationAttached::Error);
                            return;
                        }
                    }
                } else {
                    error() << "input is a sequence, but this node is not";
                    debug() << "inputPattern is" << inputPattern.pattern();
                    debug() << "destPattern is" << destPattern.pattern();
                    emit cookedAtIndex(index, OperationAttached::Error);
                    return;
                }
            } else {
                if (destPattern.isSequence()) {
                    // TODO: at some point we want to support this
                    // how? need a frame range for this node tho
                    error() << "this node is a sequence, but input node is not";
                    emit cookedAtIndex(index, OperationAttached::Error);
                } else {
                    debug() << "cooking element";
                    const QString srcPath = inputPattern.path();
                    const QString destPath = destPattern.path();

                    // TODO: dependency check
                    if (!makeLink(srcPath, destPath)) {
                        emit cookedAtIndex(index, OperationAttached::Error);
                        return;
                    }
                }
            }
        } else {
            error() << "fields incomplete for input context" << inputContext;
            emit cookedAtIndex(index, OperationAttached::Error);
            return;
        }
    }

    emit cookedAtIndex(index, OperationAttached::Finished);
}

bool File::makeLink(const QString &src, const QString &dest) const
{
    debug() << "linking" << src << "to" << dest;

    bool success = true;

    // make sure the destination directory exists
    QFileInfo destInfo(dest);
    QDir().mkpath(destInfo.dir().absolutePath());

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
