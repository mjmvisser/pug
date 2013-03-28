#ifndef FRAMERANGES_H
#define FRAMERANGES_H

#include "framerange.h"

class FrameRanges
{
public:
    FrameRanges(const QString &);
    FrameRanges(const QList<int> &);

    const QString pattern();
    void setPattern(const QString &);

    const QList<int> list();
    void setList(const QList<int> &);

private:
    QList<FrameRange> m_ranges;
};

#endif
