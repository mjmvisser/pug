#include <QDebug>
#include <QDate>
#include <QProcessEnvironment>
#include "field.h"

Field::Field(QObject *parent) :
    PugItem(parent),
    m_regexp("\\w+"),
    m_width(0),
    m_type(Field::String)
{
}

const QString Field::env() const
{
    return m_env;
}

void Field::setEnv(const QString v)
{
    if (m_env != v) {
        m_env = v;
        emit envChanged(m_env);
    }
}

const QString Field::regexp() const
{
    return m_regexp;
}

void Field::setRegexp(const QString re)
{
    if (m_regexp != re) {
        m_regexp = re;
        emit regexpChanged(m_regexp);
    }
}

Field::Type Field::type() const
{
    return m_type;
}

void Field::setType(Field::Type t)
{
    if (m_type != t) {
        m_type = t;
        emit typeChanged(m_type);
    }
}

int Field::width() const
{
    return m_width;
}

void Field::setWidth(int w)
{
    if (m_width != w) {
        m_width = w;
        emit widthChanged(m_width);
    }
}

const QStringList Field::values() const
{
    return m_values;
}

void Field::setValues(const QStringList v)
{
    if (m_values != v) {
        m_values = v;
        emit valuesChanged(m_values);
    }
}

const QString Field::formatSpec() const
{
    return m_formatSpec;
}

void Field::setFormatSpec(const QString spec)
{
    if (m_formatSpec != spec) {
        m_formatSpec = spec;
        emit formatSpecChanged(m_formatSpec);
    }
}

const QVariant Field::parse(const QString s) const
{
    bool ok;
    // convert the given string to a value of the proper type
    if (m_type == Field::Integer) {
        int n = s.toInt(&ok);
        if (ok) {
            return n;
        } else {
            qWarning() << this << " failed to convert " << s << " to an integer";
            return QVariant();
        }

    } else if (m_type == Field::String) {
        return QVariant(s);

    } else if (m_type == Field::Date) {
        QDate d = QDate::fromString(s, m_formatSpec);
        if (!d.isValid()) {
            qWarning() << this << " failed to convert " << s << " to a date";
        }
        return d;

    } else {
        return QVariant();
    }
}

const QVariant Field::get(const QVariantMap data) const
{
    QVariant value;
    if (data.contains(name())) {
        value = data.value(name());
    } else if (!m_env.isEmpty() && QProcessEnvironment::systemEnvironment().contains(m_env)) {
        value = parse(QProcessEnvironment::systemEnvironment().value(m_env));
    } else if (m_values.length() == 1) {
        value = m_values.at(0);
    }

    return value;
}

const QString Field::format(const QVariant value) const
{
    if (!value.isValid())
        return QString();

    bool ok;
    if (m_type == Field::Integer) {
        int n = value.toInt(&ok);
        if (!ok)
            qWarning() << this << " format failed to convert " << value << " to an integer";

        return QString("%1").arg(n, m_width, 10, QChar('0'));

    } else if (m_type == Field::String) {
        if (!value.canConvert<QString>())
            qWarning() << this << " format failed to convert " << value << " to string";

        return value.toString();

    } else if (m_type == Field::Date) {
        QDate d = value.toDate();
        if (!d.isValid())
            qWarning() << this << " format failed to convert " << value << " to datetime";

        return d.toString(m_formatSpec);

    } else {
        return QString();
    }
}

