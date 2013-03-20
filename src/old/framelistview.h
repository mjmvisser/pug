#ifndef FRAMELISTVIEW_H
#define FRAMELISTVIEW_H

#include <QObject>
#include <QVariantList>

#include "pugitem.h"
#include "node.h"

class FrameListView : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList frames READ frames WRITE setFrames NOTIFY frameListChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY frameListChanged)
public:
    explicit FrameListView(QObject *parent, Node *node, int index);

    const QVariantList frames() const;
    void setFrames(const QVariantList);

    const QString pattern() const;
    void setPattern(const QString);

    Q_INVOKABLE void clear();
    Q_INVOKABLE const QVariant firstFrame() const;
    Q_INVOKABLE const QVariant lastFrame() const;

    const QString toString() const;

    DetailsManagerAttached::Status status() const;

signals:
    void frameListChanged();

private:
    static const QString framesToPattern(const QVariantList &frames);
    static const QVariantList patternToFrames(const QString &frames);
    static const QVariantList sortAndRemoveDuplicates(const QVariantList &frames);

private:
    Node *m_node;
    int m_index;
};

#endif
