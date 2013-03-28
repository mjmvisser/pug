#ifndef SHOTGUNFIELD_H
#define SHOTGUNFIELD_H

#include <QString>
#include <QtQml>

#include "node.h"

class Shotgun;
class ShotgunEntity;
class Field;
class Branch;

class ShotgunField : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString shotgunField READ shotgunFieldName WRITE setShotgunFieldName NOTIFY shotgunFieldNameChanged)
    Q_PROPERTY(ShotgunEntity *entity READ entity NOTIFY entityChanged)
    Q_PROPERTY(QString field READ fieldName WRITE setFieldName NOTIFY fieldNameChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ShotgunEntity *link READ link WRITE setLink NOTIFY linksChanged)
    Q_PROPERTY(QQmlListProperty<ShotgunEntity> links READ links_ NOTIFY linksChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Node *file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(UrlType urlType READ urlType WRITE setUrlType NOTIFY urlTypeChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QVariant localStorage READ localStorage WRITE setLocalStorage NOTIFY localStorageChanged)
    Q_ENUMS(Type UrlType)
public:
    enum Type { String, Number, Path, Pattern, Link, MultiLink, Url };
    enum UrlType { Upload, Local, Web };
    explicit ShotgunField(QObject *parent = 0);

    const QString shotgunFieldName() const;
    void setShotgunFieldName(const QString);

    const ShotgunEntity *entity() const;
    ShotgunEntity *entity();

    const QString fieldName() const;
    void setFieldName(const QString);

    Type type() const;
    void setType(Type);

    ShotgunEntity *link();
    const ShotgunEntity *link() const;
    void setLink(ShotgunEntity *);

    QQmlListProperty<ShotgunEntity> links_();

    const QString pattern() const;
    void setPattern(const QString);

    const QVariant value() const;
    void setValue(const QVariant);

    const Node *file() const;
    Node *file();
    void setFile(Node *);

    const QString displayName() const;
    void setDisplayName(const QString);

    UrlType urlType() const;
    void setUrlType(UrlType);

    const QString contentType() const;
    void setContentType(const QString);

    const QVariant localStorage() const;
    void setLocalStorage(const QVariant);

    Q_INVOKABLE const QVariant buildValue(int, const QVariantMap) const;

signals:
    void shotgunFieldNameChanged(const QString shotgunField);
    void entityChanged(ShotgunEntity *entity);
    void fieldNameChanged(const QString field);
    void typeChanged(Type type);
    void linksChanged();
    void patternChanged(const QString pattern);
    void valueChanged(const QVariant value);
    void fileChanged(Node *file);
    void displayNameChanged(const QString displayName);
    void urlTypeChanged(UrlType urlType);
    void contentTypeChanged(const QString contentType);
    void localStorageChanged(const QVariant localStorage);

    void shotgunPullAtIndex(int index, const QVariant context, Shotgun *shotgun);
    void shotgunPullAtIndexFinished(int index, int status);
    void shotgunPushAtIndex(int index, const QVariant context, Shotgun *shotgun);
    void shotgunPushAtIndexFinished(int index, int status);

protected:
    virtual void componentComplete();

protected slots:
    void onShotgunPullAtIndex(int index, const QVariant context, Shotgun *shotgun);
    void onShotgunPushAtIndex(int index, const QVariant context, Shotgun *shotgun);

private slots:
    void updateCount();

private:
    // links property
    static void links_append(QQmlListProperty<ShotgunEntity> *, ShotgunEntity *);
    static int links_count(QQmlListProperty<ShotgunEntity> *);
    static ShotgunEntity *link_at(QQmlListProperty<ShotgunEntity> *, int);
    static void links_clear(QQmlListProperty<ShotgunEntity> *);

    const QVariant buildStringValue(const QVariantMap) const;
    const QVariant buildNumberValue(const QVariantMap) const;
    const QVariant buildPathValue(int) const;
    const QVariant buildPatternValue(int, const QVariantMap) const;
    const QVariant buildLinkValue(int) const;
    const QVariant buildMultiLinkValue(int) const;
    const QVariant buildUrlValue(int index) const;

    const Field *field() const;

    QString m_shotgunFieldName;
    QString m_fieldName;
    Type m_type;
    QList<ShotgunEntity *> m_links;
    QString m_pattern;
    QVariant m_value;
    Node *m_file;
    QString m_displayName;
    UrlType m_urlType;
    QString m_contentType;
    QVariant m_localStorage;
};

#endif
