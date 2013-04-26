#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QRegularExpression>

#include "elementview.h"
#include "elementsview.h"
#include "framepattern.h"

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

    connect(node, &Node::detailsChanged, this, &ElementView::sync);
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

void ElementView::setFrames(ElementView *that)
{
    setFrameCount(that->frameCount());
    for (int i = 0; i < frameCount(); i++)
        m_frames[i]->setFrame(that->m_frames[i]->frame());
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
    // tricky... we need to make sure the frames array is created (or removed, in the case of a count of 0) and
    // resized appropriately, hence the assertions
    Q_ASSERT(!node()->details().property(m_index).isUndefined());
    Q_ASSERT(!node()->details().property(m_index).property("element").isUndefined());
    if (count == 0) {
        node()->details().property(m_index).property("element").deleteProperty("frames");
        Q_ASSERT(node()->details().property(m_index).property("element").property("frames").isUndefined());
    } else {
        if (node()->details().property(m_index).property("element").property("frames").isUndefined())
            node()->details().property(m_index).property("element").setProperty("frames", node()->newArray(count));
        else
            node()->details().property(m_index).property("element").property("frames").setProperty("length", count);
        Q_ASSERT(!node()->details().property(m_index).property("element").property("frames").isUndefined());
        Q_ASSERT(node()->details().property(m_index).property("element").property("frames").property("length").toInt() == count);
    }
    emit node()->detailsChanged();
    Q_ASSERT(m_frames.length() == count);
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

    setFrameCount(0);

    FilePattern fp(pattern());

    QMap<int, QFileInfo> entriesByFrame;
    foreach (QFileInfo info, entries) {
        QRegularExpressionMatch match = fp.match(info.filePath());
        if (match.hasMatch()) {
            QString frameStr = match.captured("frame");
            if (!frameStr.isNull()) {
                entriesByFrame.insert(frameStr.toInt(), info);
            }
        }
    }

    // QMap keeps its keys in sorted order
    foreach (int frame, entriesByFrame.keys()) {
        append(entriesByFrame[frame]);
    }
}

void ElementView::scan(const QFileInfoList &entries, const FramePattern &framePattern)
{
    trace() << ".scan(" << ")";

    setFrameCount(0);

    FilePattern filePattern(pattern());

    QMap<int, QFileInfo> entriesByFrame;
    foreach (QFileInfo info, entries) {
        QRegularExpressionMatch match = filePattern.match(info.filePath());
        if (match.hasMatch()) {
            QString frameStr = match.captured("frame");
            if (!frameStr.isNull()) {
                entriesByFrame.insert(frameStr.toInt(), info);
            }
        }
    }

    const QList<int> frames = framePattern.list();
    foreach (int frame, frames) {
        if (entriesByFrame.contains(frame)) {
            append(entriesByFrame[frame]);
        } else {
            append(QFileInfo());
        }
    }
}

void ElementView::scan()
{
    trace() << ".scan()";

    FilePattern filePattern(pattern());

    QDir parentDir = QDir(filePattern.directory());
    Q_ASSERT(!parentDir.isRoot());

    info() << "matching patterns in" << parentDir << "with" << filePattern.pattern();

    QFileInfoList entries = parentDir.entryInfoList();

    scan(entries);
}

const QList<int> ElementView::frameList() const
{
    QList<int> result;
    for (int i = 0; i < m_frames.length(); i++)
        result.append(m_frames[i]->frame());
    return result;
}

const QString ElementView::framePattern() const
{
    FramePattern fp(frameList());
    return fp.pattern();
}

QVariant ElementView::frameStart() const
{
    FramePattern fp(frameList());
    if (fp.by() != 0)
        return fp.start();
    else
        return QVariant();
}

QVariant ElementView::frameEnd() const
{
    FramePattern fp(frameList());
    if (fp.by() != 0)
        return fp.end();
    else
        return QVariant();
}

QVariant ElementView::frameBy() const
{
    FramePattern fp(frameList());
    return fp.by();
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
