#ifndef ARRAY_H
#define ARRAY_H

#include <QObject>
#include <QQmlListProperty>

#include "propertybase.h"
#include "property.h"

class Node;

class Array : public PropertyBase
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Property> contents READ contents)
    Q_PROPERTY(int index READ index)
    Q_CLASSINFO("DefaultProperty", "contents")
public:
    explicit Array(QObject *parent = 0);

    QQmlListProperty<Property> contents();

signals:

public slots:

private:
    int index() const;

    QList<Property *> m_contents;
};

#endif // ARRAY_H
