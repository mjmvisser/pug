#ifndef FRAMEVIEW_H
#define FRAMEVIEW_H

#include <QObject>
#include <QVariant>
#include <QDateTime>

#include "detailsview.h"
#include "node.h"

class FrameView : public DetailsView
{
    Q_OBJECT
    Q_PROPERTY(int frame READ frame WRITE setFrame NOTIFY frameChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY frameChanged)
public:
    explicit FrameView(QObject *parent, Node *node, int elementIndex, int frameIndex);

    int frame() const;
    void setFrame(int);

    const QDateTime timestamp() const;
    void setTimestamp(const QDateTime);

    Q_INVOKABLE const QString path() const;

    Node::Status status() const;

public slots:
    void sync();

signals:
    void frameChanged();

private:
    int m_elementIndex;
    int m_frameIndex;
};

#endif
