#ifndef SHOTGUNFIELD_H
#define SHOTGUNFIELD_H

#include <QString>
#include <QtQml>

#include "node.h"

class Shotgun;
class Node;
class Field;
class Branch;

class ShotgunField : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString shotgunField READ shotgunFieldName WRITE setShotgunFieldName NOTIFY shotgunFieldNameChanged)
    Q_PROPERTY(QString field READ fieldName WRITE setFieldName NOTIFY fieldNameChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool required READ isRequired WRITE setRequired NOTIFY requiredChanged)
    Q_PROPERTY(Node *link READ link WRITE setLink NOTIFY linksChanged)
    Q_PROPERTY(QQmlListProperty<Node> links READ links_ NOTIFY linksChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Node *source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(UrlType urlType READ urlType WRITE setUrlType NOTIFY urlTypeChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QVariant localStorage READ localStorage WRITE setLocalStorage NOTIFY localStorageChanged)
    Q_ENUMS(Type UrlType)
public:
    enum Type { String, Number, Path, Link, MultiLink, Url };
    enum UrlType { Upload, Local, Web };
    explicit ShotgunField(QObject *parent = 0);

    const QString shotgunFieldName() const;
    void setShotgunFieldName(const QString);

    const QString fieldName() const;
    void setFieldName(const QString);

    Type type() const;
    void setType(Type);

    bool isRequired() const;
    void setRequired(bool);

    Node *link();
    const Node *link() const;
    void setLink(Node *);

    QQmlListProperty<Node> links_();

    const QString pattern() const;
    void setPattern(const QString);

    const QVariant value() const;
    void setValue(const QVariant);

    const Node *source() const;
    Node *source();
    void setSource(Node *);

    const QString displayName() const;
    void setDisplayName(const QString);

    UrlType urlType() const;
    void setUrlType(UrlType);

    const QString contentType() const;
    void setContentType(const QString);

    const QVariant localStorage() const;
    void setLocalStorage(const QVariant);

    Q_INVOKABLE const QVariant buildValue(int, const QVariantMap);

signals:
    void shotgunFieldNameChanged(const QString shotgunField);
    void fieldNameChanged(const QString field);
    void typeChanged(Type type);
    void requiredChanged(bool required);
    void linksChanged();
    void patternChanged(const QString pattern);
    void valueChanged(const QVariant value);
    void sourceChanged(Node *source);
    void displayNameChanged(const QString displayName);
    void urlTypeChanged(UrlType urlType);
    void contentTypeChanged(const QString contentType);
    void localStorageChanged(const QVariant localStorage);

    void update(const QVariant context);
    void updateFinished(int status);
    void release(const QVariant context);
    void releaseFinished(int status);

protected slots:
    void onUpdate(const QVariant context);
    void onRelease(const QVariant context);

private:
    // links property
    static void links_append(QQmlListProperty<Node> *, Node *);
    static int links_count(QQmlListProperty<Node> *);
    static Node *link_at(QQmlListProperty<Node> *, int);
    static void links_clear(QQmlListProperty<Node> *);

    const QVariantMap valueContext(int, const QVariantMap) const;
    const QString buildPatternValue(int, const QVariantMap);

    const QVariant buildStringValue(int, const QVariantMap);
    const QVariant buildNumberValue(int, const QVariantMap);
    const QVariant buildPathValue(int, const QVariantMap);
    const QVariant buildLinkValue(int, const QVariantMap);
    const QVariant buildMultiLinkValue(int, const QVariantMap);
    const QVariant buildUrlValue(int, const QVariantMap);

    const Field *field() const;

    QString m_shotgunFieldName;
    QString m_fieldName;
    Type m_type;
    bool m_requiredFlag;
    QList<Node *> m_links;
    QString m_pattern;
    QVariant m_value;
    Node *m_source;
    QString m_displayName;
    UrlType m_urlType;
    QString m_contentType;
    QVariant m_localStorage;
};

#endif
