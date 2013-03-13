#ifndef FILEPATTERN_H
#define FILEPATTERN_H

#include <QRegularExpression>
#include <QStringList>

#include "pugitem.h"
#include "framelist.h"

class Element : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(FrameList *frameList READ frameList WRITE setFrameList NOTIFY frameListChanged)
public:
    explicit Element(QObject *parent = 0);

    const QString pattern() const;
    void setPattern(const QString);

    FrameList *frameList();
    const FrameList *frameList() const;
    void setFrameList(FrameList *);

    Q_INVOKABLE const QStringList paths() const;
    Q_INVOKABLE const QString path() const;

    Q_INVOKABLE const QVariant frames() const;
    Q_INVOKABLE void setFrames(const QVariant);

    Q_INVOKABLE const QVariant firstFrame() const;
    Q_INVOKABLE const QVariant lastFrame() const;

    Q_INVOKABLE bool hasFrames() const;
    Q_INVOKABLE const QString directory() const;
    Q_INVOKABLE const QString baseName() const;
    Q_INVOKABLE const QString frameSpec() const;
    Q_INVOKABLE const QString extension() const;

    Q_INVOKABLE const QString path(QVariant frame) const;
    Q_INVOKABLE const QVariant frame(const QString path) const;

    Q_INVOKABLE bool matches(const QString path) const;
    Q_INVOKABLE void append(const QString path);
    Q_INVOKABLE void scan();

    const QString toString() const;

signals:
    void patternChanged(const QString pattern);
    void frameListChanged(const FrameList *frameList);

private:
    static const QString patternFromPath(const QString path);
    static const QRegularExpression regExpFromPattern(const QString pattern);

    QString m_pattern;
    FrameList *m_frameList;

    QRegularExpression m_patternRegExp;
};
Q_DECLARE_METATYPE(Element*) // makes available to QVariant

QDebug &operator<<(QDebug &, const Element *);

QDataStream &operator<<(QDataStream &, const Element *);
QDataStream &operator>>(QDataStream &, Element *);

#endif
