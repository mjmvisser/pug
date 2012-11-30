#include <QtCore>

#include "node.h"
#include "network.h"

#include "scenemodel.h"

SceneModel::SceneModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NodeRole] = "node";
    setRoleNames(roles);
}

Qt::ItemFlags SceneModel::flags(const QModelIndex &index) const
{
    return 0;
}

QVariant SceneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Node *node = static_cast<Node *>(index.internalPointer());

    switch (role)
    {
    case Qt::DisplayRole:
        return node->objectName();
    case SceneModel::NodeRole:
        return QVariant::fromValue(node);
    default:
        return QVariant();
    }
}

bool SceneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant SceneModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool SceneModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return false;
}

int SceneModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        Network *parentNetwork = static_cast<Network *>(parent.internalPointer());
        return parentNetwork->nodeCount();
    }
    else
    {
        return 1;
    }
}

int SceneModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool SceneModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

bool SceneModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        Network *parentNetwork = static_cast<Network *>(parent.internalPointer());
        return createIndex(row, column, parentNetwork->node(row));
    }
    else
    {
        return createIndex(row, column, m_scene);
    }
}

QModelIndex SceneModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

void SceneModel::setScene(Scene *scene)
{
    beginResetModel();
    m_scene = scene;
    endResetModel();
}
