#ifndef SHOTGUNFIELD_H
#define SHOTGUNFIELD_H

#include <QString>
#include <QtQml>

#include "pugitem.h"

class ShotgunEntity;
class Field;
class BranchBase;

class ShotgunField : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QString field READ fieldName WRITE setFieldName NOTIFY fieldNameChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(BranchBase *link READ link WRITE setLink NOTIFY linksChanged)
    Q_PROPERTY(QString linkType READ linkType WRITE setLinkType NOTIFY linkTypesChanged)
    Q_PROPERTY(QQmlListProperty<BranchBase> links READ links_ NOTIFY linksChanged)
    Q_PROPERTY(QStringList linkTypes READ linkTypes WRITE setLinkTypes NOTIFY linkTypesChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_ENUMS(Type)
public:
    enum Type { String, Number, Path, Pattern, Link, MultiLink };
    explicit ShotgunField(QObject *parent = 0);

    const QString fieldName() const;
    void setFieldName(const QString);

    Type type() const;
    void setType(Type);

    BranchBase *link();
    const BranchBase *link() const;
    void setLink(BranchBase *);

    const QString linkType() const;
    void setLinkType(const QString);

    QQmlListProperty<BranchBase> links_();

    const QStringList linkTypes() const;
    void setLinkTypes(const QStringList);

    const QString pattern() const;
    void setPattern(const QString);

    const QVariant value() const;
    void setValue(const QVariant);

    virtual const QVariant buildValue(const BranchBase *branch, const QVariantMap fields) const;

signals:
    void fieldNameChanged(const QString field);
    void typeChanged(Type type);
    void linksChanged();
    void linkTypesChanged();
    void patternChanged(const QString pattern);
    void valueChanged(const QVariant value);

private:
    QString m_fieldName;
    Type m_type;
    QList<BranchBase *> m_links;
    QStringList m_linkTypes;
    QString m_pattern;
    QVariant m_value;
};

class ShotgunUrlField : public ShotgunField
{
    Q_OBJECT
    Q_PROPERTY(LinkType linkType READ linkType WRITE setLinkType NOTIFY linkTypeChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QVariant localStorage READ localStorage WRITE setLocalStorage NOTIFY localStorageChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_ENUMS(LinkType)
public:
    enum LinkType { Upload, Local, Web };

    explicit ShotgunUrlField(QObject *parent = 0);

    LinkType linkType() const;
    void setLinkType(LinkType);

    const QString contentType() const;
    void setContentType(const QString);

    const QVariant localStorage() const;
    void setLocalStorage(const QVariant);

    const QUrl url() const;
    void setUrl(const QUrl);

    virtual const QVariant buildValue(const BranchBase *branch, const QVariantMap fields) const;

signals:
    void linkTypeChanged(LinkType linkType);
    void contentTypeChanged(const QString contentType);
    void localStorageChanged(const QVariant localStorage);
    void urlChanged(const QUrl url);

private:
    LinkType m_linkType;
    QString m_contentType;
    QVariant m_localStorage;
    QUrl m_url;
};

#endif
