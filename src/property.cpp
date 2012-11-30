#include "property.h"

Property::Property(QObject *parent) :
    PropertyBase(parent),
    m_input(false),
    m_output(false)
{
}

/*!
    Returns the property's value. This is the default value for properties
    in component nodes.

    \sa setValue()
*/
QVariant Property::value() const
{
    return m_value;
}

/*!
    Sets the property's value.

    \sa value()
 */
void Property::setValue(const QVariant &v)
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
QString Property::text() const
{
    return m_text;
}

void Property::setText(const QString t)
{
    if (m_text != t) {
        m_text = t;
        emit textChanged(t);
    }
}

bool Property::isInput() const
{
    return m_input;
}

void Property::setInput(bool i)
{
    if (m_input != i) {
        m_input = i;
        emit inputChanged(i);
    }
}

bool Property::isOutput() const
{
    return m_output;
}

void Property::setOutput(bool o)
{
    if (m_output != o) {
        m_output = o;
        emit outputChanged(o);
    }
}

