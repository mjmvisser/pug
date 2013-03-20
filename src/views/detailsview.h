#ifndef DETAILSVIEW_H
#define DETAILSVIEW_H

#include "pugitem.h"
#include "node.h"

class DetailsView : public PugItem
{
    Q_OBJECT
public:
    explicit DetailsView(Node *node, QObject *parent);

    const Node *node() const;
    Node *node();

protected:
    template <typename T>
    QJSValue toScriptValue(T value) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(m_node);
        Q_ASSERT(context);

        return context->engine()->toScriptValue(value);
    }

    QJSValue newArray(uint length = 0) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(m_node);
        Q_ASSERT(context);

        return context->engine()->newArray(length);
    }

    QJSValue newObject() const
    {
        QQmlContext *context = QQmlEngine::contextForObject(m_node);
        Q_ASSERT(context);

        return context->engine()->newObject();
    }

    QJSValue newQObject(QObject *object) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(m_node);
        Q_ASSERT(context);

        return context->engine()->newQObject(object);
    }

private:
    Node *m_node;
};

#endif
