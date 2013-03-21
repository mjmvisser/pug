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

    template <class TA>
    const TA *attachedPropertiesObject(const QMetaObject *mo, bool create = true) const
    {
        int idx = -1;
        return qobject_cast<const TA *>(qmlAttachedPropertiesObject(&idx, this, mo, create));
    }

    template <class TA>
    TA *attachedPropertiesObject(const QMetaObject *mo, bool create = true)
    {
        int idx = -1;
        return qobject_cast<TA *>(qmlAttachedPropertiesObject(&idx, this, mo, create));
    }

    // TODO: if T::Attached is defined, we only need T
    template <class T, class TA>
    const TA *attachedPropertiesObject(bool create = true) const
    {
        int idx = -1;
        return qobject_cast<const TA *>(qmlAttachedPropertiesObject(&idx, this, &T::staticMetaObject, create));
    }

    template <class T, class TA>
    TA *attachedPropertiesObject(bool create = true)
    {
        int idx = -1;
        return qobject_cast<TA *>(qmlAttachedPropertiesObject(&idx, this, &T::staticMetaObject, create));
    }

    Q_INVOKABLE void trace(const QString message) const;
    Q_INVOKABLE void debug(const QString message) const;
    Q_INVOKABLE void info(const QString message) const;
    Q_INVOKABLE void warning(const QString message) const;
    Q_INVOKABLE void error(const QString message) const;

    Q_INVOKABLE QObject *sender();

signals:
    void parentChanged(PugItem *parent);
    void nameChanged(const QString name);
    void logLevelChanged(Log::MessageType logLevel);

protected:
    virtual void classBegin() {};
    virtual void componentComplete() {};

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

// http://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
// http://stackoverflow.com/questions/6483422/qt-qlist-const-correctness
template<class T>
inline const QList<T *>& unConstList(const QList<const T*>& list)
{
    return reinterpret_cast<const QList<T*>&>(const_cast<const QList<const T*>&>(list));
}

template<class T>
inline const QList<const T *>& constList(const QList<T*>& list)
{
    return reinterpret_cast<const QList<const T*>&>(const_cast<const QList<T*>&>(list));
}

Q_DECLARE_METATYPE(PugItem*) // makes available to QVariant

#endif
