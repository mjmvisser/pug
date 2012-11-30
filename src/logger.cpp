#include <QDebug>

#include "logger.h"

Log::Log(QObject *parent) :
    QObject(parent),
    m_level(Log::Warning)
{
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

void Log::debug(const QString &message)
{
    append(Log::Debug, message);
}

void Log::copious(const QString &message)
{
    append(Log::Copious, message);
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
