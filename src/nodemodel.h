#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QAbstractItemModel>

#include "root.h"

class NodeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(Root *root READ root WRITE setRoot NOTIFY rootChanged)
    Q_ENUMS(Roles)
public:
    NodeModel(QObject *parent = 0);

    enum Roles {
        NodeRole = Qt::UserRole
    };

    Root *root();
    void setRoot(Root *);

    virtual Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const;
    virtual Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    virtual Q_INVOKABLE QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual Q_INVOKABLE QModelIndex parent(const QModelIndex &index) const;
    virtual Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

signals:
    void rootChanged(Root *root);

private:
    Root *m_root;
};

#endif
