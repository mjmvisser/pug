#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>

#include "elementview.h"
#include "elementsview.h"

/*!
    \class ElementView
    \inmodule Pug
*/

/*!
    \qmltype ElementView
    \instantiates ElementView
    \inqmlmodule Pug
    \brief
*/

ElementView::ElementView(Node *node, int index, QObject *parent) :
    DetailsView(node, parent),
    m_index(index)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(node);

    connect(node, &Node::detailsChanged, this, &ElementView::elementChanged);
    sync();
}

const QString ElementView::pattern() const
{
    return node()->details()
            .property(m_index)
            .property("element")
            .property("pattern").toString();
}

void ElementView::setPattern(const QString p)
{
    trace() << ".setPattern(" << p << ")";
    FilePattern fp(p);
    if (pattern() != fp.pattern()) {
        debug() << "setting pattern to" << fp.pattern();
        node()->setDetail(m_index, "element", "pattern", toScriptValue(fp.pattern()));
    }
}

QQmlListProperty<FrameView> ElementView::frames_()
{
    return QQmlListProperty<FrameView>(this, 0,
                                       ElementView::frame_count,
                                       ElementView::frame_at);
}

int ElementView::frame_count(QQmlListProperty<FrameView> *prop)
{
    ElementView *that = static_cast<ElementView *>(prop->object);
    return that->m_frames.count();
}

FrameView *ElementView::frame_at(QQmlListProperty<FrameView> *prop, int i)
{
    ElementView *that = static_cast<ElementView *>(prop->object);
    if (i >= 0 && i < that->m_frames.count())
        return that->m_frames[i];
    else
        return 0;
}

const FrameView *ElementView::frameAt(int index) const
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_frames.length());
    return m_frames[index];
}

FrameView *ElementView::frameAt(int index)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_frames.length());
    return m_frames[index];
}

int ElementView::frameCount() const
{
    return m_frames.length();
}

void ElementView::setFrameCount(int count)
{
    for (int i = m_frames.length(); i < count; i++)
        m_frames.append(new FrameView(node(), m_index, i, this));
}

const QDateTime ElementView::timestamp() const
{
    FilePattern fp(pattern());

    if (fp.isSequence()) {
        QDateTime result;
        return result;
    } else {
        return node()->details()
                .property(m_index)
                .property("element")
                .property("timestamp").toDateTime();
    }
}

void ElementView::setTimestamp(const QDateTime ts)
{
    FilePattern fp(pattern());

    if (fp.isSequence()) {
        error() << "Can't set timestamp on element with frames";
    } else {
        if (timestamp() != ts) {
            node()->setDetail(m_index, "element", "timestamp", toScriptValue(ts));
        }
    }
}

const QString ElementView::path() const
{
    FilePattern fp(pattern());

    if (!fp.isSequence())
        return fp.path();
    else
        return QString();
}

const QString ElementView::directory() const
{
    FilePattern fp(pattern());

    return fp.directory();
}

const QString ElementView::baseName() const
{
    FilePattern fp(pattern());

    return fp.baseName();
}

const QString ElementView::frameSpec() const
{
    FilePattern fp(pattern());

    return fp.frameSpec();
}

const QString ElementView::extension() const
{
    FilePattern fp(pattern());

    return fp.extension();
}

bool ElementView::isSequence() const
{
    FilePattern fp(pattern());

    return fp.isSequence();
}

void ElementView::append(const QFileInfo &info)
{
    FilePattern fp(pattern());

    if (fp.isSequence()) {
        QRegularExpressionMatch match = fp.match(info.filePath());
        if (match.hasMatch()) {
            QString frameStr = match.captured("frame");
            int frame;
            if (!frameStr.isNull()) {
                bool ok;
                frame = frameStr.toInt(&ok);
                if (!ok) {
                    error() << "Can't parse frame" << frameStr;
                    return;
                }
            } else {
                error() << ".append got path with no frame" << info.filePath();
                return;
            }


            // add the new frame
            FrameView *newFrame = new FrameView(node(), m_index, m_frames.length(), this);
            m_frames.append(newFrame);
            newFrame->setFrame(frame);
            newFrame->setTimestamp(info.lastModified());
        } else {
            error() << this << ".append" << "path" << "does not match" << pattern();
        }
    } else {
        node()->setDetail(m_index, "element", "timestamp", toScriptValue(info.lastModified()));
    }
}

void ElementView::scan(const QFileInfoList &entries)
{
    trace() << ".scan(" << ")";

    FilePattern fp(pattern());

    foreach (QFileInfo info, entries) {
        if (!info.isHidden() && fp.match(info.filePath()).hasMatch()) {
            append(info);
        }
    }
}

const QList<int> ElementView::framesList() const
{
    QList<int> result;
    for (int i = 0; i < m_frames.length(); i++)
        result.append(m_frames[i]->frame());
    qSort(result);
    return result;
}

int ElementView::firstFrame() const
{
    const QList<int> frames = framesList();
    return !frames.empty() ? frames.first() : -1;
}

int ElementView::lastFrame() const
{
    const QList<int> frames = framesList();
    return !frames.empty() > 0 ? frames.last() : -1;
}

const QString ElementView::framesPattern() const
{
    Q_ASSERT(false); // not implemented
}

Node::Status ElementView::status() const
{
    Node::Status result = Node::Invalid;

    foreach (const Input *in, node()->inputs()) {
        foreach (const Node *upn, node()->upstream(in)) {
            const QScopedPointer<const ElementsView> inputView(new ElementsView(const_cast<Node*>(upn)));

            Node::Status inputStatus = Node::Invalid;

            switch (in->dependency()) {
            case Input::None:
                inputStatus = Node::UpToDate;
                break;

            case Input::Node:
                if (inputView->timestamp() > timestamp())
                    inputStatus = Node::OutOfDate;
                else
                    inputStatus = Node::UpToDate;
                break;

            case Input::Element:
            case Input::Frame: {
                const ElementView *inputElementView = inputView->elementAt(m_index);

                if (inputElementView) {
                    if (inputElementView->timestamp() > timestamp())
                        inputStatus = Node::OutOfDate;
                    else
                        inputStatus = Node::UpToDate;
                } else {
                    inputStatus = Node::Missing;
                }
                break;
            }

            default:
                break;
            }

            if (inputStatus > result)
                result = inputStatus;
        }
    }

    return result;
}

void ElementView::sync()
{
    int frameCount = node()->details().property(m_index).property("element").property("frames").property("length").toInt();

    // remove excess frames
    for (int i = m_frames.length(); i > frameCount; i--)
        m_frames.takeLast()->deleteLater();

    // add missing frames
    for (int i = m_frames.length(); i < frameCount; i++)
        m_frames.append(new FrameView(node(), m_index, i, this));

    emit elementChanged();

    // sync frames
    for (int i = 0; i < m_frames.length(); i++)
        m_frames.at(i)->sync();
}

QDebug &operator<<(QDebug &dbg, const ElementView *element)
{
    if (!element)
        return dbg.nospace() << "ElementView(0x0)";

    dbg.nospace() << "ElementView(" << (void *)element;
    if (!element->objectName().isEmpty())
        dbg.nospace() << "name = " << element->objectName() << ", ";
    dbg.nospace() << ", value = " << element->pattern() << " [";
    bool first = true;
    for (int i = 0; i < element->frameCount(); i++) {
        if (!first)
            dbg.nospace() << ", ";
        else
            first = false;

        dbg.nospace() << element->frameAt(i)->frame();
    }
    dbg.nospace() << "])";

    return dbg.space();
}
