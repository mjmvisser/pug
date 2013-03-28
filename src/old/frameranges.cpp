FrameRanges::FrameRanges(const QString &pattern)
{
    setPattern(pattern);
}

FrameRanges::FrameRanges(const QList<int> &list) :
    m_list(list)
{
    setList(list):
}

const QString FrameRanges::pattern()
{

}

void FrameRanges::setPattern(const QString &)
{

}

const QList<int> FrameRanges::list()
{
}

void FrameRanges::setList(const QList<int> &l)
{
    m_frames = l;
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

const QString FrameListView::listToPattern(const QList<int>& list)
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
            if (fabs(f - lastF - by) < 1.0e-7) {
                end = f;
            } else if (fabs(f - start - by - 1) < 1.0e-7) {
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

const QVariantList FrameListView::patternToFrames(const QString& pattern)
{
    QVariantList frames;
    QStringList patterns = pattern.split(',');
    foreach (QString pattern, patterns) {
        static const QRegularExpression parseRange("(?:(?<start>-?\\d+(?:[.]\\d+)?)(?:-(?<end>-?\\d+(?:[.]\\d+)?)(?:x(?<by>-?\\d+(?:[.]\\d+)?))?)?)");
        QRegularExpressionMatch match = parseRange.match(pattern);
        if (match.hasMatch()) {
            int start = match.captured("start");
            if (match.captured("end").isNull()) {
                frames << start;
            } else {
                int end = match.captured("end");
                if (match.captured("by").isNull()) {
                    for (int frame = start; frame <= end; frame += 1) {
                        frames << frame;
                    }
                } else {
                    int by = match.captured("by");
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

const QVariantList FrameListView::sortAndRemoveDuplicates(const QVariantList &frames)
{
    // convert to int
    QList<int> intFrames;
    foreach (QVariant v, frames) {
        Q_ASSERT(v.canConvert<int>());
        intFrames << v;
    }

    QList<int> sortedFrames = intFrames;
    qSort(sortedFrames);

    QVariantList noDuplicateFrames;
    int lastF = -1e10;
    foreach (int f, sortedFrames) {
        if (fabs(f-lastF) > 1e-7) {
            noDuplicateFrames << f;
            lastF = f;
        }
    }

    return noDuplicateFrames;
}
