#include <QRegularExpression>

const QString FrameRange::pattern()
{
    if (m_by != 0)
        return QString("%d-%dx%d").arg(m_start).arg(m_end).arg(m_by);
    else
        return QString("%d-%d").arg(m_start).arg(m_end);
}

const QList<int> FrameRange::list()
{
    QList<int> frames;
    for (int f = m_start; f <= m_end; f += m_by) {
        frames << f;
    }
    return frames;
}
