#ifndef LOGGING_H
#define LOGGING_H

#include <QObject>
#include <QDebug>
#include <QMetaType>
#include <QTextStream>
#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QBitArray>
#include <QUrl>
#include <QRegularExpression>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Logger;

class Log : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MessageType level READ level WRITE setLevel NOTIFY levelChanged)
    Q_ENUMS(MessageType)
    friend class Logger;
public:
    enum MessageType { Invalid, Copious, Debug, Info, Warning, Error };

    explicit Log(QObject *parent = 0);

    MessageType level() const;
    void setLevel(MessageType);

    Q_INVOKABLE void debug(const QString &message);
    Q_INVOKABLE void copious(const QString &message);
    Q_INVOKABLE void info(const QString &message);
    Q_INVOKABLE void warning(const QString &message);
    Q_INVOKABLE void error(const QString &message);

    Q_INVOKABLE QStringList messages(MessageType level);

signals:
    void levelChanged(MessageType level);

protected:
    void append(MessageType t, const QString &message);

private:
    struct Message {
        Message(QObject *o, MessageType t, const QString& m) : object(o), time(QDateTime::currentDateTime()), type(t), message(m) {};
        QObject *object;
        QDateTime time;
        MessageType type;
        QString message;

        const QString format()
        {
            QString buffer;
            QTextStream ts(&buffer);
            QString typeString;
            switch (type) {
            case Debug:
                typeString = "DEBUG";
                break;
            case Copious:
                typeString = "COPIOUS";
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
                dbg.nospace() << object;
            }

            ts << time.toString() << " " << objInfo << "[" << typeString << "] " << message;
            return buffer;
        }
    };

    QList<Message> m_messages;
    MessageType m_level;
};

class Logger : public QDebug
{
    struct Buffer {
        Buffer(Log *l, Log::MessageType t) : log(l), type(t), ref(1) {};
        Log *log;
        Log::MessageType type;
        QString str;
        int ref;
    } *m_buffer;
public:
    inline Logger(Log *l, Log::MessageType t) : QDebug(static_cast<QString*>(0)), m_buffer(new Buffer(l, t)) { *static_cast<QDebug *>(this) = QDebug(&m_buffer->str); }
    inline Logger(const Logger &l): QDebug(l), m_buffer(l.m_buffer) { ++m_buffer->ref; }
    inline Logger &operator=(const Logger &other) {
        if (this != &other) {
            Logger copy(other);
            qSwap(m_buffer, copy.m_buffer);
        }
        return *this;
    }

    inline ~Logger() {
        if (!--m_buffer->ref) {
            m_buffer->log->append(m_buffer->type, m_buffer->str);
            delete m_buffer;
        }
    }
};

#endif // LOGGING_H
