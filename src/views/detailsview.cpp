#include "detailsview.h"

DetailsView::DetailsView(QObject *parent, Node *node) :
    PugItem(parent),
    m_node(node)
{
}

const Node *DetailsView::node() const
{
    return m_node;
}

Node *DetailsView::node()
{
    return m_node;
}
