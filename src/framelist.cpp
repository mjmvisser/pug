#include "framelist.h"
#include "framepattern.h"

FrameList::FrameList(QObject *parent) :
    QObject(parent)
{
}

const QList<int> FrameList::list() const
{
    return m_frames;
}

void FrameList::setList(const QList<int> l)
{
    if (m_frames != l) {
        m_frames = l;
        emit framesChanged(this);
    }
}

const QString FrameList::pattern() const
{
    FramePattern fp(m_frames);
    return fp.pattern();
}

void FrameList::setPattern(const QString p)
{
    if (pattern() != p) {
        FramePattern fp(p);
        m_frames = fp.list();
        emit framesChanged(this);
    }
}


const QVariant FrameList::start() const
{
    FramePattern fp(m_frames);
    if (fp.by() != 0)
        return fp.start();
    else
        return QVariant();
}

void FrameList::setStart(const QVariant f)
{
    if (f.canConvert<int>()) {
        m_start = f;

        if (m_end.canConvert<int>()) {
            if (m_by.canConvert<int>()) {
                FramePattern fp(m_start.toInt(), m_end.toInt(), m_by.toInt());
                m_frames = fp.list();
            } else {
                FramePattern fp(m_start.toInt(), m_end.toInt(), 1);
                setList(fp.list());
            }
        }
    } else {
        m_start = QVariant();
    }
}

const QVariant FrameList::end() const
{
    FramePattern fp(m_frames);
    if (fp.by() != 0)
        return fp.end();
    else
        return QVariant();
}

void FrameList::setEnd(const QVariant f)
{
    if (f.canConvert<int>()) {
        m_end = f;

        if (m_start.canConvert<int>()) {
            if (m_by.canConvert<int>()) {
                FramePattern fp(m_start.toInt(), m_end.toInt(), m_by.toInt());
                m_frames = fp.list();
            } else {
                FramePattern fp(m_start.toInt(), m_end.toInt(), 1);
                setList(fp.list());
            }
        }
    } else {
        m_end = QVariant();
    }

}

const QVariant FrameList::by() const
{
    FramePattern fp(m_frames);
    return fp.by();
}

void FrameList::setBy(const QVariant f)
{
    if (f.canConvert<int>()) {
        m_by = f;

        if (m_start.canConvert<int>() && m_end.canConvert<int>()) {
            FramePattern fp(m_start.toInt(), m_end.toInt(), m_by.toInt());
            setList(fp.list());
        }
    } else {
        m_by = QVariant();
    }
}

