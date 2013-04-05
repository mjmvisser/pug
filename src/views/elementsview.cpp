#include <QScopedPointer>

#include "elementsview.h"

ElementsView::ElementsView(Node *node, QObject *parent) :
    DetailsView(node, parent)
{
    Q_ASSERT(node);
    connect(node, &Node::detailsChanged, this, &ElementsView::sync);
    connect(node, &Node::detailsChanged, this, &ElementsView::elementsChanged);
    sync();
}

QQmlListProperty<ElementView> ElementsView::elements_()
{
    return QQmlListProperty<ElementView>(this, 0,
                                         ElementsView::elements_count,
                                         ElementsView::element_at);
}

int ElementsView::elements_count(QQmlListProperty<ElementView> *prop)
{
    ElementsView *that = static_cast<ElementsView *>(prop->object);
    return that->m_elements.count();
}

ElementView *ElementsView::element_at(QQmlListProperty<ElementView> *prop, int i)
{
    ElementsView *that = static_cast<ElementsView *>(prop->object);
    if (i >= 0 && i < that->m_elements.count())
        return that->m_elements[i];
    else
        return 0;
}

const ElementView *ElementsView::elementAt(int index) const
{
    if (index >= 0 && index < m_elements.length())
        return m_elements[index];
    else
        return 0;
}

ElementView *ElementsView::elementAt(int index)
{
    if (index >= 0 && index < m_elements.length())
        return m_elements[index];
    else
        return 0;
}

int ElementsView::elementCount() const
{
    return m_elements.length();
}

void ElementsView::sync()
{
    if (!node())
        return;

    int count = node()->count();
    // remove excess elements
    for (int i = m_elements.length(); i > count; i--)
        m_elements.takeLast()->deleteLater();

    // add missing elements
    for (int i = m_elements.length(); i < count; i++)
        m_elements.append(new ElementView(node(), i, this));

    emit elementsChanged();

    // sync frames
    for (int i = 0; i < m_elements.length(); i++)
        m_elements.at(i)->sync();

    Q_ASSERT(m_elements.length() == count);
}

const QDateTime ElementsView::timestamp() const
{
    QDateTime result;
    foreach (const ElementView *element, m_elements) {
        if (!result.isValid() || element->timestamp() < result)
            result = element->timestamp();
    }

    return result;
}

Node::Status ElementsView::status() const
{
    Node::Status result = Node::Invalid;

    foreach (const Input *in, node()->inputs()) {
        foreach (const Node *upn, node()->upstream(in)) {
            const QScopedPointer<const ElementsView> inputView(new ElementsView(const_cast<Node*>(upn)));
            Node::Status inputStatus = Node::Invalid;

            switch (in->dependency()) {
            case Input::None:
                inputStatus = Node::UpToDate;
                break;

            case Input::Node:
                if (inputView->timestamp() < timestamp())
                    inputStatus = Node::OutOfDate;
                else
                    inputStatus = Node::UpToDate;
                break;

            case Input::Element:
            case Input::Frame:
                Q_ASSERT(false);
                break;

            default:
                break;
            }

            if (inputStatus > result)
                result = inputStatus;
        }
    }

    return result;
}
