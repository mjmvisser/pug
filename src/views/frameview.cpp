#include "frameview.h"
#include "elementsview.h"
#include "filepattern.h"

FrameView::FrameView(Node *node, int elementIndex, int frameIndex, QObject *parent) :
    DetailsView(node, parent),
    m_elementIndex(elementIndex),
    m_frameIndex(frameIndex)
{
    Q_ASSERT(node);
    Q_ASSERT(elementIndex >= 0);
    Q_ASSERT(frameIndex >= 0);

    connect(node, &Node::detailsChanged, this, &FrameView::frameChanged);
    sync();
}

int FrameView::frame() const
{
    return node()->details()
            .property(m_elementIndex)
            .property("element")
            .property("frames")
            .property(m_frameIndex)
            .property("frame").toInt();
}

void FrameView::setFrame(int f)
{
    if (frame() != f) {
        trace() << ".setFrame(" << f << ")";
        node()->setDetail(m_elementIndex, "element", "frames", m_frameIndex, "frame", toScriptValue(f));
        Q_ASSERT(frame() == f);
    }
}

const QDateTime FrameView::timestamp() const
{
    return node()->details()
            .property(m_elementIndex)
            .property("element")
            .property("frames")
            .property(m_frameIndex)
            .property("timestamp").toDateTime();
}

void FrameView::setTimestamp(const QDateTime ts)
{
    if (timestamp() != ts) {
        trace() << ".setTimestamp(" << ts << ")";
        node()->setDetail(m_elementIndex, "element", "frames", m_frameIndex, "timestamp", toScriptValue(ts));
        Q_ASSERT(timestamp() == ts);
    }
}

const QString FrameView::path() const
{
    QScopedPointer<ElementsView> elementsView(new ElementsView(const_cast<Node*>(node())));

    const ElementView *element = elementsView->elementAt(m_elementIndex);
    Q_ASSERT(element);

    FilePattern fp(element->pattern());
    Q_ASSERT(fp.isSequence());

    debug() << "formatting frame" << frame() << "with pattern" << fp.pattern() << "gets" << fp.path(frame());

    return fp.path(frame());
}

void FrameView::sync()
{
    emit frameChanged();
}
