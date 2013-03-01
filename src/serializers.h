#ifndef SERIALIZERS_H
#define SERIALIZERS_H

#include <QDataStream>
#include <QJSValue>
#include <QJSEngine>

class JSDataStream : public QDataStream
{
    friend JSDataStream &operator<<(JSDataStream &, const QJSValue &);
    friend JSDataStream &operator>>(JSDataStream &, QJSValue &);
public:
    JSDataStream(QJSEngine *e) : QDataStream(), m_engine(e) {};
    JSDataStream(QJSEngine *e, QIODevice * d) : QDataStream(d), m_engine(e) {};
    JSDataStream(QJSEngine *e, QByteArray * a, QIODevice::OpenMode mode) : QDataStream(a, mode), m_engine(e) {};
    JSDataStream(QJSEngine *e, const QByteArray & a) : QDataStream(a), m_engine(e) {};
private:
    JSDataStream() : QDataStream() {};
    JSDataStream(QIODevice * d) : QDataStream(d) {};
    JSDataStream(QByteArray * a, QIODevice::OpenMode mode) : QDataStream(a, mode) {};
    JSDataStream(const QByteArray & a) : QDataStream(a) {};

    QJSEngine *m_engine;
};

JSDataStream &operator<<(JSDataStream &, const QJSValue &);
JSDataStream &operator>>(JSDataStream &, QJSValue &);

QDataStream &operator<<(QDataStream &, const QObject &);
QDataStream &operator>>(QDataStream &, QObject &);

#endif
