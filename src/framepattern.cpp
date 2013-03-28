#include <QStringList>
#include <QRegularExpression>
#include <QDebug>

#include "framepattern.h"

FramePattern::FramePattern(int start, int end, int by)
{
    setRange(start, end, by);
}

FramePattern::FramePattern(const QString &pattern)
{
    setPattern(pattern);
}

FramePattern::FramePattern(const QList<int> &list) :
    m_frames(list)
{
    setList(list);
}

const QString FramePattern::pattern() const
{
    return listToPattern(m_frames);
}

const QList<int> FramePattern::list() const
{
    return m_frames;
}

int FramePattern::start() const
{
    if (m_frames.length() > 1) {
        int start = m_frames.first();
        int end = m_frames.last();
        int by = m_frames[1] - m_frames[0];
        if (m_frames == rangeToList(start, end, by)) {
            return start;
        }
    } else if (m_frames.length() == 1) {
        return m_frames[0];
    }

    return 0;
}

int FramePattern::end() const
{
    if (m_frames.length() > 1) {
        int start = m_frames.first();
        int end = m_frames.last();
        int by = m_frames[1] - m_frames[0];
        if (m_frames == rangeToList(start, end, by)) {
            return end;
        }
    } else if (m_frames.length() == 1) {
        return m_frames[0];
    }

    return 0;
}

int FramePattern::by() const
{
    if (m_frames.length() > 1) {
        int start = m_frames.first();
        int end = m_frames.last();
        int by = m_frames[1] - m_frames[0];
        if (m_frames == rangeToList(start, end, by)) {
            return by;
        }
    } else if (m_frames.length() == 1) {
        return 1;
    }

    return 0;
}

void FramePattern::setPattern(const QString &pattern)
{
    m_frames = patternToList(pattern);
}

void FramePattern::setList(const QList<int> &l)
{
    m_frames = l;
}

void FramePattern::setRange(int start, int end, int by)
{
    QList<int> frames;
    for (int f = start; f <= end; f += by) {
        frames << f;
    }
    setList(frames);
}

static const QString formatRange(int start, int end, int by)
{
    if (start == end)
        return QString("%1").arg(start);
    else if (by == 1.0f)
        return QString("%1-%2").arg(start).arg(end);
    else
        return QString("%1-%2x%3").arg(start).arg(end).arg(by);
}

const QString FramePattern::listToPattern(const QList<int>& list)
{
    if (list.length() == 0) {
        return QString();
    } else if (list.length() == 1) {
        return formatRange(list[0], list[0], 1);
    } else {
        QStringList patterns;

        int start = list[0];
        int end = list[1];
        int by = end-start;
        int lastF = end;
        for (int index = 2; index < list.length(); index++) {
            int f = list[index];
            if (f - lastF == by) {
                end = f;
            } else if (f - start - by == 1) {
                by += 1;
            } else {
                patterns << formatRange(start, end, by);
                start = end = f;
                by = 1;
            }

            lastF = f;
        }

        patterns << formatRange(start, end, by);

        return patterns.join(',');
    }
}

static const QList<int> sortAndRemoveDuplicates(const QList<int> &frames)
{
    // convert to int
    QList<int> sortedFrames = frames;
    qSort(sortedFrames);

    QList<int> noDuplicateFrames;
    int lastF = 0;
    foreach (int f, sortedFrames) {
        if (f != lastF) {
            noDuplicateFrames << f;
            lastF = f;
        }
    }

    return noDuplicateFrames;
}

const QList<int> FramePattern::patternToList(const QString& pattern)
{
    QList<int> frames;
    QStringList patterns = pattern.split(',');
    foreach (QString pattern, patterns) {
        static const QRegularExpression parseRange("(?:(?<start>-?\\d+)(?:-(?<end>-?\\d+)(?:x(?<by>-?\\d+))?)?)");
        QRegularExpressionMatch match = parseRange.match(pattern);
        if (match.hasMatch()) {
            int start = match.captured("start").toInt();
            if (match.captured("end").isNull()) {
                frames << start;
            } else {
                int end = match.captured("end").toInt();
                if (match.captured("by").isNull()) {
                    for (int frame = start; frame <= end; frame += 1) {
                        frames << frame;
                    }
                } else {
                    int by = match.captured("by").toInt();
                    for (int frame = start; frame <= end; frame += by) {
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

const QList<int> FramePattern::rangeToList(int start, int end, int by)
{
    Q_ASSERT(by >= 1);
    Q_ASSERT(end > start);

    QList<int> frames;
    for (int f = start; f <= end; f += by) {
        frames << f;
    }
    return frames;
}
