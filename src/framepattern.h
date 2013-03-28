#ifndef FRAMEPATTERN_H
#define FRAMEPATTERN_H

#include <QString>
#include <QList>

class FramePattern
{
public:
    FramePattern(int, int, int);
    FramePattern(const QString &);
    FramePattern(const QList<int> &);

    const QString pattern() const;
    const QList<int> list() const;
    int start() const;
    int end() const;
    int by() const;

private:
    void setPattern(const QString &);
    void setList(const QList<int> &);
    void setRange(int, int, int);

    static const QString listToPattern(const QList<int> &);
    static const QList<int> patternToList(const QString &);
    static const QList<int> rangeToList(int, int, int);


    QList<int> m_frames;
};

#endif
