#include <QDebug>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QSet>

#include "framelistview.h"

/*!
    \class FrameListView
    \inmodule Pug
*/

/*!
    \qmltype FrameListView
    \instantiates FrameListView
    \inqmlmodule Pug
    \brief
*/

FrameListView::FrameListView(QObject *parent, Node *node, int index) :
    PugItem(parent),
    m_node(node),
    m_index(index)
{
    Q_ASSERT(node);

    connect(node, &Node::detailsChanged, this, &FrameListView::frameListChanged);
}

const QVariantList FrameListView::frames() const
{
     return parent<Node>()->detail(m_index, "element", "frames").toVariant().toList();
}

void FrameListView::setFrames(const QVariantList fl)
{
    if (frames() != fl) {
        // check that we can convert each frame to a float
        foreach (QVariant v, fl) {
            if (!v.canConvert<float>()) {
                qWarning() << this << "invalid frame:" << v;
                return;
            }
        }

        QVariantList cfl = sortAndRemoveDuplicates(fl);

        parent<Node>()->setDetail(m_index, "element", "frames", toScriptValue(cfl));
    }
}

void FrameListView::clear()
{
    parent<Node>()->setDetail(m_index, "element", "frames", newArray());
}

const QVariant FrameListView::firstFrame() const
{
    return frames().isEmpty() ? QVariant() : frames().first();
}

const QVariant FrameListView::lastFrame() const
{
    return frames().isEmpty() ? QVariant() : frames().last();
}

const QString FrameListView::pattern() const
{
    return framesToPattern(frames());
}

void FrameListView::setPattern(const QString p)
{
    if (pattern() != p) {
        const QVariantList fl = patternToFrames(p);
        parent<Node>()->setDetail(m_index, "element", "frames", toScriptValue(fl));
        emit frameListChanged();
    }
}

const QString FrameListView::toString() const
{
    return pattern();
}

static const QString formatRange(float start, float end, float by)
{
    if (start == end)
        return QString("%1").arg(start);
    else if (by == 1.0f)
        return QString("%1-%2").arg(start).arg(end);
    else
        return QString("%1-%2x%3").arg(start).arg(end).arg(by);
}

const QString FrameListView::framesToPattern(const QVariantList& frames)
{
    if (frames.length() == 0) {
        return QString();
    } else if (frames.length() == 1) {
        return formatRange(frames[0].toFloat(), frames[0].toFloat(), 1.0f);
    } else {
        QStringList patterns;

        float start = frames[0].toFloat();
        float end = frames[1].toFloat();
        float by = end-start;
        float lastF = end;
        for (int index = 2; index < frames.length(); index++) {
            float f = frames[index].toFloat();
            if (fabs(f - lastF - by) < 1.0e-7) {
                end = f;
            } else if (fabs(f - start - by - 1.0f) < 1.0e-7) {
                by += 1.0f;
            } else {
                patterns << formatRange(start, end, by);
                start = end = f;
                by = 1.0f;
            }

            lastF = f;
        }

        patterns << formatRange(start, end, by);

        return patterns.join(',');
    }
}

const QVariantList FrameListView::patternToFrames(const QString& pattern)
{
    QVariantList frames;
    QStringList patterns = pattern.split(',');
    foreach (QString pattern, patterns) {
        static const QRegularExpression parseRange("(?:(?<start>-?\\d+(?:[.]\\d+)?)(?:-(?<end>-?\\d+(?:[.]\\d+)?)(?:x(?<by>-?\\d+(?:[.]\\d+)?))?)?)");
        QRegularExpressionMatch match = parseRange.match(pattern);
        if (match.hasMatch()) {
            float start = match.captured("start").toFloat();
            if (match.captured("end").isNull()) {
                frames << start;
            } else {
                float end = match.captured("end").toFloat();
                if (match.captured("by").isNull()) {
                    for (float frame = start; frame <= end; frame += 1.0f) {
                        frames << frame;
                    }
                } else {
                    float by = match.captured("by").toFloat();
                    for (float frame = start; frame <= end; frame += by) {
                        frames << frame;
                    }
                }
            }
        } else {
            qWarning() << "patternToFrames failed to match" << pattern;
        }
    }

    return sortAndRemoveDuplicates(frames);
}

const QVariantList FrameListView::sortAndRemoveDuplicates(const QVariantList &frames)
{
    // convert to float
    QList<float> floatFrames;
    foreach (QVariant v, frames) {
        Q_ASSERT(v.canConvert<float>());
        floatFrames << v.toFloat();
    }

    QList<float> sortedFrames = floatFrames;
    qSort(sortedFrames);

    QVariantList noDuplicateFrames;
    float lastF = -1e10;
    foreach (float f, sortedFrames) {
        if (fabs(f-lastF) > 1e-7) {
            noDuplicateFrames << f;
            lastF = f;
        }
    }

    return noDuplicateFrames;
}
