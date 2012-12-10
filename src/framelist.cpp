#include <QDebug>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QSet>

#include "framelist.h"

/*!
    \class FrameList
    \inmodule Pug
*/

/*!
    \qmltype FrameList
    \instantiates FrameList
    \inqmlmodule Pug
    \brief A FrameList represents a list of frames, which may or may not be contiguous.
*/

FrameList::FrameList(QObject *parent) :
    PugItem(parent)
{
}

QVariantList FrameList::frames()
{
    return m_frames;
}

const QVariantList FrameList::frames() const
{
    return m_frames;
}

void FrameList::setFrames(const QVariantList frames)
{
    // check that we can convert each frame to a int
    foreach (QVariant v, frames) {
        if (!v.canConvert<float>()) {
            qWarning() << this << "invalid frame:" << v;
            return;
        }
    }

    QVariantList newFrames = sortAndRemoveDuplicates(frames);

    if (m_frames != newFrames) {
        m_frames = newFrames;
        emit patternChanged(framesToPattern(m_frames));
        emit framesChanged(m_frames);
    }
}

const QVariant FrameList::firstFrame() const
{
    return m_frames.isEmpty() ? QVariant() : m_frames.first();
}

const QVariant FrameList::lastFrame() const
{
    return m_frames.isEmpty() ? QVariant() : m_frames.last();
}

const QString FrameList::pattern() const
{
    return framesToPattern(m_frames);
}

void FrameList::setPattern(const QString pattern)
{
    const QVariantList newFrames = patternToFrames(pattern);
    if (m_frames != newFrames) {
        m_frames = newFrames;
        emit patternChanged(framesToPattern(m_frames));
        emit framesChanged(m_frames);
    }
}

const QString FrameList::toString() const
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

const QString FrameList::framesToPattern(const QVariantList& frames)
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

const QVariantList FrameList::patternToFrames(const QString& pattern)
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

const QVariantList FrameList::sortAndRemoveDuplicates(const QVariantList &frames)
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
