#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>

#include "propertybase.h"

class Node : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PropertyBase> properties READ properties)
    Q_CLASSINFO("DefaultProperty", "properties")
public:
    explicit Node(QObject *parent = 0);
    
    QQmlListProperty<PropertyBase> properties();
    int propertyCount() const;
    PropertyBase* property(int index) const;

signals:
    
public slots:

private:
    QList<PropertyBase *> m_properties;
    
};
//Q_DECLARE_METATYPE(Node*) // makes available to QVariant

#endif // NODE_H
