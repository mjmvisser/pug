#include "param.h"

Param::Param(QObject *parent) :
    PugItem(parent)
{
}

/*!
    Returns the property's value. This is the default value for properties
    in component nodes.

    \sa setValue()
*/
QVariant Param::value() const
{
    return m_value;
}

/*!
    Sets the property's default value.

    \sa value()
 */
void Param::setValue(const QVariant &v)
{
    if (m_value != v) {
        m_value = v;
        emit valueChanged(v);
    }
}

/*!
    The property's label.

    \sa setText()
 */
QString Param::text() const
{
    return m_text;
}

void Param::setText(const QString t)
{
    if (m_text != t) {
        m_text = t;
        emit textChanged(t);
    }
}
