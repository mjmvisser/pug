#include "array.h"

Array::Array(QObject *parent) :
    MetaProperty(parent)
{
}

QQmlListProperty<Param> Array::contents_()
{
    // TODO: fix
    return QQmlListProperty<Param>(this, m_contents);
}

int Array::index() const {
    return 0;
}
