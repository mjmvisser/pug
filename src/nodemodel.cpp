#include <QVariant>

#include "nodebase.h"

#include "nodemodel.h"


NodeModel::NodeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

Root *NodeModel::root()
{
    return m_root;
}

void NodeModel::setRoot(Root *root)
{
    if (m_root != root) {
        m_root = root;
        emit rootChanged(root);
    }
}

QModelIndex NodeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    NodeBase *parentNode;

    if (!parent.isValid())
        parentNode = m_root;
    else
        parentNode = static_cast<NodeBase*>(parent.internalPointer());

    NodeBase *childNode = parentNode->child(row);
    if (childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex NodeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    NodeBase *childNode = static_cast<NodeBase*>(index.internalPointer());
    NodeBase *parentNode = childNode->parent<NodeBase>();

    if (parentNode == m_root)
        return QModelIndex();

    return createIndex(parentNode->index(), 0, parentNode);
}

int NodeModel::rowCount(const QModelIndex &parent) const
{
    NodeBase *parentNode;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = m_root;
    else
        parentNode = static_cast<NodeBase*>(parent.internalPointer());

    return parentNode->childCount();
}

int NodeModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
//    if (parent.isValid())
//        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
//    else
//        return rootItem->columnCount();
}

QVariant NodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    NodeBase *node = static_cast<NodeBase*>(index.internalPointer());

    switch(role) {
    case Qt::DisplayRole:
        return node->objectName();
    case NodeModel::NodeRole:
        return QVariant::fromValue(node);
    default:
        return QVariant();
    }
}

Qt::ItemFlags NodeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant NodeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                               int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        return "Name";
    case NodeModel::NodeRole:
        return "Node";
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> NodeModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(Qt::DisplayRole, "name");
    names.insert(NodeModel::NodeRole, "node");
    return names;
}
