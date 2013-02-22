#include "graphconnection.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>

GraphConnection::GraphConnection(QQuickItem *parent) :
    QQuickItem(parent),
    m_source(),
    m_sink()
{
    setFlag(ItemHasContents, true);
    setAntialiasing(true);
}

QPointF GraphConnection::source()
{
    return m_source;
}

void GraphConnection::setSource(QPointF source)
{
    if (m_source != source) {
        m_source = source;
        emit sourceChanged(source);
        update();
    }
}

QPointF GraphConnection::sink()
{
    return m_sink;
}

void GraphConnection::setSink(QPointF sink)
{
    if (m_sink != sink) {
        m_sink = sink;
        emit sinkChanged(sink);
        update();
    }
}

QSGNode *GraphConnection::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (m_source == m_sink)
        return oldNode;

    QSGGeometryNode *node = 0;
    QSGGeometry *geometry = 0;

    QPointF p1, p2, p3, p4;

    p1.setX(m_source.x());
    p1.setY(m_source.y());
    p4.setX(m_sink.x());
    p4.setY(m_sink.y());

    p2.setX(p1.x());
    p3.setX(p4.x());
    if (p1.y() < p4.y()) {
        p2.setY(p1.y() * 0.75 + p4.y() * 0.25);
        p3.setY(p4.y() * 0.75 + p1.y() * 0.25);
    } else {
        p2.setY(p1.y() - (p4.y()-p1.y())*0.25);
        p3.setY(p4.y() + (p4.y()-p1.y())*0.25);
    }

    // quick 'n' dirty estimation of bezier length
    // http://steve.hollasch.net/cgindex/curves/cbezarclen.html
    qreal l1 = (QVector2D(p2)-QVector2D(p1)).length() +
               (QVector2D(p3)-QVector2D(p2)).length() +
               (QVector2D(p4)-QVector2D(p3)).length();
    qreal l0 = (QVector2D(p4)-QVector2D(p1)).length();
    qreal length = 0.5*l0 + 0.5*l1;

    int segmentCount = length;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), segmentCount);
        geometry->setLineWidth(2);
        geometry->setDrawingMode(GL_LINE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(0, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(segmentCount);
    }

    QRectF bounds = boundingRect();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0; i < segmentCount; ++i) {
        qreal t = i / qreal(segmentCount - 1);
        qreal invt = 1 - t;

        // Bezier
        QPointF pos = invt * invt * invt * p1
                    + 3 * invt * invt * t * p2
                    + 3 * invt * t * t * p3
                    + t * t * t * p4;

        float x = pos.x();
        float y = pos.y();

        vertices[i].set(x, y);
    }

    return node;
}
