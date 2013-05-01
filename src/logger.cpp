#include <QDebug>
#include <QJsonDocument>
#include <QJSValue>
#include <QJSValueIterator>

#include "logger.h"
#include "pugitem.h"

Log::Log(QObject *parent) :
    QObject(parent),
    m_level(Log::Warning)
{
}

const QString Log::Message::format()
{
    QString buffer;
    QTextStream ts(&buffer);
    QString typeString;
    switch (type) {
    case Trace:
        typeString = "TRACE";
        break;
    case Debug:
        typeString = "DEBUG";
        break;
    case Info:
        typeString = "INFO";
        break;
    case Warning:
        typeString = "WARNING";
        break;
    case Error:
        typeString = "ERROR";
        break;
    case Invalid:
        Q_ASSERT(false);
        break;
    }

    QString objInfo;
    {
        QDebug dbg(&objInfo);
        PugItem *item = qobject_cast<PugItem *>(object);
        if (item)
            dbg.nospace() << item;
        else
            dbg.nospace() << object;
    }

    ts << time.toString() << " " << objInfo << endl << "    [" << typeString << "] " << message;
    return buffer;
}


Log::MessageType Log::level() const
{
    return m_level;
}

void Log::setLevel(MessageType l)
{
    if (m_level != l && l != Log::Invalid) {
        m_level = l;
        emit levelChanged(l);
    }
}

void Log::trace(const QString &message)
{
    append(Log::Trace, message);
}

void Log::debug(const QString &message)
{
    append(Log::Debug, message);
}

void Log::info(const QString &message)
{
    append(Log::Info, message);
}

void Log::warning(const QString &message)
{
    append(Log::Warning, message);
}

void Log::error(const QString &message)
{
    append(Log::Error, message);
}

void Log::append(Log::MessageType t, const QString &msg)
{
    Log::Message message = Log::Message(parent(), t, msg);
    m_messages.append(message);

    if (t >= m_level)
        qDebug() << message.format().toUtf8().constData();
}

QStringList Log::messages(Log::MessageType level)
{
    QStringList result;
    foreach (Log::Message m, m_messages) {
        if (m.type >= level)
            result << m.format();
    }
    return result;
}

Log::MessageType Log::maxLevel() const
{
    Log::MessageType result = Log::Invalid;
    foreach (const Log::Message &message, m_messages) {
        if (message.type > result)
            result = message.type;
    }
    return result;
}

void Log::clear()
{
    m_messages.clear();
}

QDebug operator<<(QDebug dbg, const QJSValue jsvalue)
{
    if (jsvalue.isUndefined()) {
        dbg.nospace() << "QJSValue(QJSValue::Undefined)";
    } else if (jsvalue.isNull()) {
        dbg.nospace() << "QJSValue(QJSValue::Null)";
    } else if (jsvalue.isBool()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toBool() << ")";
    } else if (jsvalue.isNumber()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toNumber() << ")";
    } else if (jsvalue.isString()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toString() << ")";
    } else if (jsvalue.isDate()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toDateTime() << ")";
    } else if (jsvalue.isRegExp()) {
        dbg.nospace() << "QJSValue(/" << jsvalue.toString() << "/)";
    } else if (jsvalue.isArray()) {
        int length = jsvalue.property("length").toInt();
        dbg.nospace() << "QJSValue(length = " << length << "; [";
        for (int i = 0; i < length; i++) {
            dbg.nospace() << jsvalue.property(i);
            if (i != length-1)
                dbg.nospace() << ", ";
        }
        dbg.nospace() << "])";
    } else if (jsvalue.isCallable()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toString() << ")";
    } else if (jsvalue.isVariant()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toVariant() << ")";
    } else if (jsvalue.isQObject()) {
        dbg.nospace() << "QJSValue(" << jsvalue.toQObject() << ")";
    } else if (jsvalue.isObject()) {
        dbg.nospace() << "QJSValue({";
        QJSValueIterator it(jsvalue);
        while (it.hasNext()) {
            it.next();
            if (it.name() != "length" && it.name() != "prototype" && it.name() != "constructor" && it.name() != "arguments") {
                dbg.nospace() << it.name() << ": " << it.value();
                if (it.hasNext())
                    dbg.nospace() << ", ";
            }
        }
    }

    return dbg.space();
}
