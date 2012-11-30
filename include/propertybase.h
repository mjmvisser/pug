#ifndef PROPERTYBASE_H
#define PROPERTYBASE_H

#include <QObject>

class Node;

class PropertyBase : public QObject
{
    Q_OBJECT
public:
    explicit PropertyBase(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // PROPERTYBASE_H
