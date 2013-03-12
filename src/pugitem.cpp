#include <QtQml>

#include "pugitem.h"

/*!
    \class PugItem
    \inmodule Pug
*/

/*!
    \qmltype PugItem
    \instantiates PugItem
    \inqmlmodule Pug
    \brief The base class of all classes in the Pug module.

    PugItem is the base class of all classes in the Pug module. It has properties and
    methods for instance naming, child Pug items, the parent Pug item and logging that
    are available to all Pug items.
*/

PugItem::PugItem(QObject *parent) :
    QObject(parent),
    m_log(new Log(this)),
    m_logLevel(Log::Invalid)
{
}

QQmlListProperty<PugItem> PugItem::data_()
{
    return QQmlListProperty<PugItem>(this, 0, PugItem::data_append,
                                        PugItem::data_count,
                                        PugItem::data_at,
                                        PugItem::data_clear);
}

// data property
void PugItem::data_append(QQmlListProperty<PugItem> *prop, PugItem *item)
{
    if (!item)
        return;

    PugItem *that = static_cast<PugItem *>(prop->object);

    item->setParent(that);
}

int PugItem::data_count(QQmlListProperty<PugItem> *prop)
{
    Q_UNUSED(prop);
    return 0;
}

PugItem *PugItem::data_at(QQmlListProperty<PugItem> *prop, int i)
{
    Q_UNUSED(prop);
    Q_UNUSED(i);
    return 0;
}

void PugItem::data_clear(QQmlListProperty<PugItem> *prop)
{
    Q_UNUSED(prop)
}

PugItem *PugItem::parentItem()
{
    return qobject_cast<PugItem*>(QObject::parent());
}

const PugItem *PugItem::parentItem() const
{
    return qobject_cast<PugItem*>(QObject::parent());
}

const QString PugItem::name() const
{
    return objectName();
}

void PugItem::setName(const QString name)
{
    if (objectName() != name) {
        setObjectName(name);
        emit nameChanged(name);
    }
}

bool PugItem::inherits(const QString className) const
{
    return QObject::inherits(className.toUtf8());
}

const QString PugItem::className() const
{
    return metaObject()->className();
}

Log::MessageType PugItem::logLevel() const
{
    // return first set logLevel in hierarchy
    const PugItem *p = this;
    while (p && p->m_logLevel == Log::Invalid) {
        p = p->firstParent<PugItem>();
    }

    return p ? p->m_logLevel : Log::Invalid;
}

void PugItem::setLogLevel(Log::MessageType l)
{
    if (m_logLevel != l) {
        m_logLevel = l;
        emit logLevelChanged(l);
    }
}

Log *PugItem::log()
{
    return m_log;
}

const Log *PugItem::log() const
{
    return m_log;
}

bool PugItem::hasMethod(const char *method) const
{
    Q_ASSERT(method[0] == '0');
    // the +1 is to skip the code prepended by the METHOD macro
    if (metaObject()->indexOfMethod(QMetaObject::normalizedSignature(method+1)) >= 0)
        return true;
    else
        return false;
}

bool PugItem::hasSignal(const char *signal) const
{
    Q_ASSERT(signal[0] == '2');
    // the +1 is to skip the code prepended by the SIGNAL macro
    if (metaObject()->indexOfSignal(QMetaObject::normalizedSignature(signal+1)) >= 0)
        return true;
    else
        return false;
}

bool PugItem::hasProperty(const char *prop) const
{
    if (metaObject()->indexOfProperty(prop) >= 0)
        return true;
    else
        return false;
}

Logger PugItem::trace() const
{
    Q_ASSERT(m_log);
    m_log->setLevel(logLevel());
    return Logger(m_log, Log::Trace);
}

Logger PugItem::debug() const
{
    Q_ASSERT(m_log);
    m_log->setLevel(logLevel());
    return Logger(m_log, Log::Debug);
}

Logger PugItem::info() const
{
    Q_ASSERT(m_log);
    m_log->setLevel(logLevel());
    return Logger(m_log, Log::Info);
}

Logger PugItem::warning() const
{
    Q_ASSERT(m_log);
    m_log->setLevel(logLevel());
    return Logger(m_log, Log::Warning);
}

Logger PugItem::error() const
{
    Q_ASSERT(m_log);
    m_log->setLevel(logLevel());
    return Logger(m_log, Log::Error);
}

void PugItem::trace(const QString message) const
{
    trace() << message.toUtf8().constData();
}

void PugItem::debug(const QString message) const
{
    debug() << message.toUtf8().constData();
}

void PugItem::info(const QString message) const
{
    info() << message.toUtf8().constData();
}

void PugItem::warning(const QString message) const
{
    warning() << message.toUtf8().constData();
}

void PugItem::error(const QString message) const
{
    error() << message.toUtf8().constData();
}

QObject *PugItem::sender()
{
    return QObject::sender();
}

const QString PugItem::relativePath(const QString path)
{
    QQmlContext *context = QQmlEngine::contextForObject(this);
    Q_ASSERT(context);

    return context->resolvedUrl(path).toLocalFile();
}
