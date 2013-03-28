#ifndef FRAMERANGE_H
#define FRAMERANGE_H

class FrameRange
{
public:
    FrameRange() : m_start(0), m_end(0), m_by(0) {};
    FrameRange(int start, int end, int by) : m_start(start), m_end(end), m_by(by) {};

    int start() const { return m_start; }
    void setStart(int f) { m_start = f; }

    int end() const { return m_end; }
    void setEnd(int f) { m_end = f; }

    int by() const { return m_by; }
    void setBy(int f) { m_by = f; }

    const QString pattern();
    const QList<int> list();

private:
    int m_start;
    int m_end;
    int m_by;
};

#endif
