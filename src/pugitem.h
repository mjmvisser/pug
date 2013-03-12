#ifndef PUGITEM_H
#define PUGITEM_H

#include <QtQml>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QMetaType>
#include <QObject>

#include "logger.h"

class PugItem : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PugItem> data READ data_ STORED false CONSTANT)
    Q_PROPERTY(PugItem* parent READ parentItem STORED false CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString className READ className STORED false CONSTANT FINAL)
    Q_PROPERTY(Log::MessageType logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged STORED false)
    Q_PROPERTY(Log log READ log STORED false CONSTANT)
    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "data")
    Q_ENUMS(Log::MessageType)
public:
    explicit PugItem(QObject *parent = 0);

    QQmlListProperty<PugItem> data_();

    template <class T>
    const T *parent() const { return qobject_cast<const T *>(QObject::parent()); }

    template <class T>
    T *parent() { return qobject_cast<T *>(QObject::parent()); }

    const PugItem *parentItem() const;
    PugItem *parentItem();

    const QString name() const;
    void setName(const QString);

    const QString className() const;

    Log::MessageType logLevel() const;
    void setLogLevel(Log::MessageType);

    Log *log();
    const Log *log() const;

    Q_INVOKABLE bool inherits(const QString) const;

    bool hasMethod(const char *) const;
    bool hasSignal(const char *) const;
    bool hasProperty(const char *) const;

    template <class T>
    const T *firstParent() const
    {
        // find parent until we have a T (or we run out of parents)
        const QObject *p = QObject::parent();
        while (p && !qobject_cast<const T *>(p)) {
            p = p->QObject::parent();
        }
        return qobject_cast<const T *>(p);
    }

    template <class T>
    T *firstParent()
    {
        // find parent until we have a T (or we run out of parents)
        QObject *p = QObject::parent();
        while (p && !qobject_cast<T *>(p)) {
            p = p->QObject::parent();
        }
        return qobject_cast<T *>(p);
    }

    template <class T>
    const T *attachedPropertiesObject(const QMetaObject *mo, bool create = true) const
    {
        int idx = -1;
        return qobject_cast<const T *>(qmlAttachedPropertiesObject(&idx, this, mo, create));
    }

    template <class T>
    T *attachedPropertiesObject(const QMetaObject *mo, bool create = true)
    {
        int idx = -1;
        return qobject_cast<T *>(qmlAttachedPropertiesObject(&idx, this, mo, create));
    }

    Q_INVOKABLE void trace(const QString message) const;
    Q_INVOKABLE void debug(const QString message) const;
    Q_INVOKABLE void info(const QString message) const;
    Q_INVOKABLE void warning(const QString message) const;
    Q_INVOKABLE void error(const QString message) const;

    Q_INVOKABLE QObject *sender();

    Q_INVOKABLE const QString relativePath(const QString path);

signals:
    void parentChanged(PugItem *parent);
    void nameChanged(const QString name);
    void logLevelChanged(Log::MessageType logLevel);

protected:
    virtual void classBegin() {};
    virtual void componentComplete() {};

    template <typename T>
    QJSValue toScriptValue(T value) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->toScriptValue(value);
    }

    QJSValue newArray(uint length = 0) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newArray(length);
    }

    QJSValue newObject() const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newObject();
    }

    QJSValue newQObject(QObject *object) const
    {
        QQmlContext *context = QQmlEngine::contextForObject(this);
        Q_ASSERT(context);

        return context->engine()->newQObject(object);
    }

    Logger trace() const;
    Logger debug() const;
    Logger info() const;
    Logger warning() const;
    Logger error() const;

private:
    // data property
    static void data_append(QQmlListProperty<PugItem> *, PugItem *);
    static int data_count(QQmlListProperty<PugItem> *);
    static PugItem *data_at(QQmlListProperty<PugItem> *, int);
    static void data_clear(QQmlListProperty<PugItem> *);

    // TODO: ugly
    mutable Log *m_log;
    Log::MessageType m_logLevel;
};

Q_DECLARE_METATYPE(PugItem*) // makes available to QVariant

#endif
