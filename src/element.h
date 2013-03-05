#ifndef FILEPATTERN_H
#define FILEPATTERN_H

#include <QRegularExpression>
#include <QStringList>

#include "pugitem.h"
#include "framelist.h"

class Element : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY elementChanged)
    Q_PROPERTY(FrameList *frameList READ frameList WRITE setFrameList NOTIFY elementChanged)
    Q_PROPERTY(QVariant frames READ frames WRITE setFrames NOTIFY elementChanged)
    Q_PROPERTY(QVariant firstFrame READ firstFrame NOTIFY elementChanged)
    Q_PROPERTY(QVariant lastFrame READ lastFrame NOTIFY elementChanged)
    Q_PROPERTY(QStringList paths READ paths NOTIFY elementChanged)
    Q_PROPERTY(QString path READ path NOTIFY elementChanged)
    Q_PROPERTY(QString directory READ directory NOTIFY elementChanged)
    Q_PROPERTY(QString baseName READ baseName NOTIFY elementChanged)
    Q_PROPERTY(QString frameSpec READ frameSpec NOTIFY elementChanged)
    Q_PROPERTY(QString extension READ extension NOTIFY elementChanged)
public:
    explicit Element(QObject *parent = 0);

    const QString pattern() const;
    void setPattern(const QString);

    const QStringList paths() const;
    const QString path() const;

    FrameList *frameList();
    const FrameList *frameList() const;
    void setFrameList(FrameList *);

    QVariant frames();
    const QVariant frames() const;
    void setFrames(const QVariant);

    const QVariant firstFrame() const;
    const QVariant lastFrame() const;

    const QString directory() const;
    const QString baseName() const;
    const QString frameSpec() const;
    const QString extension() const;
    bool hasFrames() const;

    const QString path(QVariant frame) const;
    const QVariant frame(const QString path) const;

    Q_INVOKABLE bool matches(const QString path) const;
    Q_INVOKABLE void append(const QString path);
    Q_INVOKABLE void scan();

    const QString toString() const;

signals:
    void elementChanged();

private:
    static const QString patternFromPath(const QString path);
    static const QRegularExpression regExpFromPattern(const QString pattern);

    QString m_pattern;
    QRegularExpression m_patternRegExp;
    FrameList *m_frameList;
};
Q_DECLARE_METATYPE(Element*) // makes available to QVariant

QDebug &operator<<(QDebug &, const Element *);

QDataStream &operator<<(QDataStream &, const Element *);
QDataStream &operator>>(QDataStream &, Element *);

#endif
