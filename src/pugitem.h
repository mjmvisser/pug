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
    Q_PROPERTY(QQmlListProperty<PugItem> children READ children_ STORED false CONSTANT)
    Q_PROPERTY(PugItem* parent READ parentItem STORED false CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString className READ className STORED false CONSTANT FINAL)
    Q_PROPERTY(Log::MessageType logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged STORED false)
    Q_PROPERTY(Log log READ log STORED false CONSTANT)
    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "children")
    Q_ENUMS(Log::MessageType)
public:
    explicit PugItem(QObject *parent = 0);

    QQmlListProperty<PugItem> children_();

    template <class T>
    const T *parent() const { return qobject_cast<const T *>(QObject::parent()); }

    template <class T>
    T *parent() { return qobject_cast<T *>(QObject::parent()); }

    const PugItem *parentItem() const;
    PugItem *parentItem();

    // TODO: not sure if this is the best way
    virtual bool isRoot() const;

    const QString name() const;
    void setName(const QString);

    const QString className() const;

    Q_INVOKABLE const QString path() const;

    Log::MessageType logLevel() const;
    void setLogLevel(Log::MessageType);

    Log *log() const;
    void setLog(Log *);

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

    Logger trace() const;
    Logger debug() const;
    Logger info() const;
    Logger warning() const;
    Logger error() const;

    Q_INVOKABLE void addTrace(const QString message) const;
    Q_INVOKABLE void addDebug(const QString message) const;
    Q_INVOKABLE void addInfo(const QString message) const;
    Q_INVOKABLE void addWarning(const QString message) const;
    Q_INVOKABLE void addError(const QString message) const;

signals:
    void parentChanged(PugItem *parent);
    void nameChanged(const QString name);
    void logLevelChanged(Log::MessageType logLevel);

protected:
    virtual void classBegin() {};
    virtual void componentComplete() {};

private:
    // children property
    static void children_append(QQmlListProperty<PugItem> *, PugItem *);
    static int children_count(QQmlListProperty<PugItem> *);
    static PugItem *child_at(QQmlListProperty<PugItem> *, int);
    static void children_clear(QQmlListProperty<PugItem> *);

    // TODO: ugly
    mutable Log *m_log;
    Log::MessageType m_logLevel;
};

inline QDebug operator<<(QDebug dbg, const PugItem *item)
{
    if (!item)
        return dbg << "PugItem(0x0) ";
    dbg.nospace() << item->metaObject()->className() << '(' << (void *)item << ", path = " << item->path() << ')';
    return dbg.space();
}

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

uint qHash(const QFileInfo &fileInfo);

Q_DECLARE_METATYPE(PugItem*) // makes available to QVariant

#endif
