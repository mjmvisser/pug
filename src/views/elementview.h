#ifndef ELEMENTVIEW_H
#define ELEMENTVIEW_H

#include <QRegularExpression>
#include <QStringList>
#include <QDateTime>
#include <QFileInfo>

#include "detailsview.h"
#include "node.h"
#include "frameview.h"
#include "filepattern.h"
#include "framepattern.h"

class ElementView : public DetailsView
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY elementChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY elementChanged)
    Q_PROPERTY(QQmlListProperty<FrameView> frames READ frames_ NOTIFY elementChanged)
public:
    explicit ElementView(Node *node, int index, QObject *parent);

    const QString pattern() const;
    void setPattern(const QString);

    const QDateTime timestamp() const;
    void setTimestamp(const QDateTime);

    Q_INVOKABLE const QString path() const;
    Q_INVOKABLE const QString directory() const;
    Q_INVOKABLE const QString baseName() const;
    Q_INVOKABLE const QString frameSpec() const;
    Q_INVOKABLE const QString extension() const;

    Q_INVOKABLE bool isSequence() const;

    QQmlListProperty<FrameView> frames_();
    Q_INVOKABLE void setFrames(ElementView *that);

    const FrameView *frameAt(int index) const;
    FrameView *frameAt(int index);

    Q_INVOKABLE const QList<int> frameList() const;
    Q_INVOKABLE const QString framePattern() const;
    Q_INVOKABLE QVariant frameStart() const;
    Q_INVOKABLE QVariant frameEnd() const;
    Q_INVOKABLE QVariant frameBy() const;

    int frameCount() const;
    void setFrameCount(int);

    Q_INVOKABLE void scan();
    void scan(const QFileInfoList &);
    void scan(const QFileInfoList &, const FramePattern &);

    Node::Status status() const;

public slots:
    void sync();

signals:
    void elementChanged();

private:
    // frames property
    static int frame_count(QQmlListProperty<FrameView> *);
    static FrameView *frame_at(QQmlListProperty<FrameView> *, int);

    void append(const QFileInfo &info);

    int m_index;
    QList<FrameView *> m_frames;
};

QDebug &operator<<(QDebug &, const ElementView *);

#endif
