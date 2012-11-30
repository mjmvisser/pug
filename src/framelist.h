#ifndef FRAMELIST_H
#define FRAMELIST_H

#include <QObject>
#include <QVariantList>

#include "pugitem.h"

class FrameList : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList frames READ frames WRITE setFrames NOTIFY framesChanged)
    Q_PROPERTY(QVariant firstFrame READ firstFrame NOTIFY framesChanged)
    Q_PROPERTY(QVariant lastFrame READ lastFrame NOTIFY framesChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
public:
    explicit FrameList(QObject *parent = 0);

    QVariantList frames();
    const QVariantList frames() const;
    void setFrames(const QVariantList);

    const QVariant firstFrame() const;
    const QVariant lastFrame() const;

    const QString pattern() const;
    void setPattern(const QString);

    const QString toString() const;

signals:
    void framesChanged(const QVariantList& frames);
    void patternChanged(const QString& pattern);

private:
    static const QString framesToPattern(const QVariantList& frames);
    static const QVariantList patternToFrames(const QString& frames);

private:
    QVariantList m_frames;
};

Q_DECLARE_METATYPE(FrameList*) // makes available to QVariant

#endif