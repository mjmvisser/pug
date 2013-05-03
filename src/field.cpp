#include <QDebug>
#include <QDate>
#include <QProcessEnvironment>
#include "field.h"

static const QString DEFAULT_REGEXP("\\w+");

Field::Field(QObject *parent) :
    PugItem(parent),
    m_regexp(DEFAULT_REGEXP),
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

void Field::setRegexp(const QString regexp)
{
    if (m_regexp != regexp) {
        if (!QRegularExpression(regexp).isValid()) {
            error() << "regexp" << regexp << "is not a valid regular expression";
            return;
        }

        m_regexp = regexp;
        emit regexpChanged(regexp);
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

void Field::setWidth(int width)
{
    if (m_width != width) {
        if (width < 1) {
            error() << "invalid width" << width;
            return;
        }

        m_width = width;
        emit widthChanged(m_width);
    }
}

const QStringList Field::values() const
{
    return m_values;
}

void Field::setValues(const QStringList values)
{
    if (m_values != values) {
        if (values.isEmpty()) {
            // reset our regular expression
            setRegexp(DEFAULT_REGEXP);
            m_values = values;
            emit valuesChanged(values);
        } else {
            // if we have a default value, it better be in the new values
            if (!m_defaultValue.isEmpty() && !values.contains(m_defaultValue)) {
                error() << "defaultValue doesn't match values";
                return;
            }

            // build the new regular expression
            QStringList regexpParts;
            foreach (const QString value, values) {
                regexpParts << ("(?:" + QRegularExpression::escape(value) + ")");
            }
            setRegexp(regexpParts.join("|"));

            m_values = values;
            emit valuesChanged(m_values);

            // if it's a single value, it's also the new default value
            if (values.length() == 1)
                setDefaultValue(values[0]);
        }
    }
}

const QString Field::defaultValue() const
{
    return m_defaultValue;
}

void Field::setDefaultValue(const QString defaultValue)
{
    if (m_defaultValue != defaultValue) {
        // TODO: can't do this here, since values may be set AFTER defaultValue
//        // it better match our regexp
//        QRegularExpression regexp("^" + m_regexp + "$");
//        if (!regexp.match(defaultValue).hasMatch()) {
//            error() << "defaultValue" << defaultValue << "does not match regexp " << regexp;
//            return;
//        }

        m_defaultValue = defaultValue;
        emit defaultValueChanged(defaultValue);
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

const QVariant Field::value(const QVariantMap context) const
{
    QVariant value;
    if (context.contains(name())) {
        value = context.value(name());
        if (value.canConvert<QString>())
            value = parse(value.toString());
    } else if (!m_env.isEmpty() && QProcessEnvironment::systemEnvironment().contains(m_env)) {
        value = parse(QProcessEnvironment::systemEnvironment().value(m_env));
    } else if (!m_defaultValue.isEmpty()) {
        value = m_defaultValue;
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

const QStringList Field::fieldNames(const QString pattern)
{
    QStringList results;
    static const QRegularExpression re("\\{(\\w+)\\}");

    QRegularExpressionMatchIterator it = re.globalMatch(pattern);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        results.append(match.captured(1));
    }

    return results;
}


