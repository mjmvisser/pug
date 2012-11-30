#include <QDebug>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVariantList>

#include "framelist.h"

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
    // check that we can convert each frame to a float
    bool failed = false;
    foreach (QVariant v, frames) {
        if (!v.canConvert<float>()) {
            qWarning() << this << "invalid frame:" << v;
            failed = true;
        }
    }

    QVariantList newFrames;
    if (failed)
        newFrames = QVariantList();
    else
        newFrames = frames;

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
    if (frames.length() == 0)
        return QString();

    QList<float> sortedFrames;
    foreach (QVariant frame, frames) {
        Q_ASSERT(frame.canConvert<float>());
        sortedFrames << frame.toFloat();
    }

    qSort(sortedFrames);
    QStringList patterns;

    float start = sortedFrames.at(0);
    float end = sortedFrames.at(0);
    float by = 1.0f;
    float lastF = start-by;
    int index;
    for (index = 0; index < sortedFrames.length(); index++) {
        float f = sortedFrames.at(index);
        if (f == lastF + by) {
            end = f;
        } else if (f == start+by+1) {
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

const QVariantList FrameList::patternToFrames(const QString& pattern)
{
    QList<float> frames;
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

    qSort(frames);
    QVariantList sortedFrames;
    foreach (float frame, frames) {
        sortedFrames << frame;
    }

    return sortedFrames;
}
