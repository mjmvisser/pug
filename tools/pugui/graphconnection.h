#ifndef GRAPHCONNECTION_H
#define GRAPHCONNECTION_H

#include <QtQuick/QQuickItem>

class GraphConnection : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QPointF sink READ sink WRITE setSink NOTIFY sinkChanged)
public:
    GraphConnection(QQuickItem *parent = 0);

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    QPointF source();
    void setSource(QPointF );

    QPointF sink();
    void setSink(QPointF );

signals:
    void sourceChanged(QPointF source);
    void sinkChanged(QPointF sink);

private:
    QPointF m_source;
    QPointF m_sink;
};

#endif
