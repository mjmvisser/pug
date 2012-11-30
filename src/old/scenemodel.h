#ifndef SCENEMODEL_H
#define SCENEMODEL_H

#include <QAbstractItemModel>

#include "scene.h"

class SceneModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum ItemRoles {
            NodeRole = Qt::UserRole + 1
        };

    explicit SceneModel(QObject *parent = 0);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent);
    virtual bool removeRows(int row, int count, const QModelIndex &parent);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    void setScene(Scene *scene);

signals:
    
public slots:

private:
    Scene *m_scene;
};

#endif // SCENEMODEL_H
