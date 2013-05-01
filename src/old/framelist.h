#ifndef FRAMELIST_H
#define FRAMELIST_H

#include <QObject>
#include <QString>
#include <QVariant>

class FrameList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> list READ list WRITE setList NOTIFY framesChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY framesChanged)
    Q_PROPERTY(QVariant start READ start WRITE setStart NOTIFY framesChanged)
    Q_PROPERTY(QVariant end READ end WRITE setEnd NOTIFY framesChanged)
    Q_PROPERTY(QVariant by READ by WRITE setBy NOTIFY framesChanged)
public:
    FrameList(QObject *parent = 0);

    const QList<int> list() const;
    void setList(const QList<int>);

    const QString pattern() const;
    void setPattern(const QString);

    const QVariant start() const;
    void setStart(const QVariant);

    const QVariant end() const;
    void setEnd(const QVariant);

    const QVariant by() const;
    void setBy(const QVariant);

signals:
    void framesChanged(FrameList *frames);

private:
    QList<int> m_frames;
    QVariant m_start;
    QVariant m_end;
    QVariant m_by;
};

#endif
