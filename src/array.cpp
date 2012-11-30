#include "array.h"
#include "node.h"
#include "property.h"

Array::Array(QObject *parent) :
    PropertyBase(parent)
{
}

QQmlListProperty<Property> Array::contents()
{
    return QQmlListProperty<Property>(this, m_contents);
}

int Array::index() const {
    return 0;
}
