#include "shotgunfield.h"
#include "shotgunentity.h"
#include "branchbase.h"
#include "shotgunoperation.h"

ShotgunField::ShotgunField(QObject *parent) :
    PugItem(parent)
{
}

const QString ShotgunField::fieldName() const
{
    return m_fieldName;
}

void ShotgunField::setFieldName(const QString fn)
{
    if (m_fieldName != fn) {
        m_fieldName = fn;
        emit fieldNameChanged(m_fieldName);
    }
}

ShotgunField::Type ShotgunField::type() const
{
    return m_type;
}

void ShotgunField::setType(ShotgunField::Type t)
{
    if (m_type != t) {
        m_type = t;
        emit typeChanged(m_type);
    }
}

BranchBase *ShotgunField::link()
{
    return m_links.length() == 1 ? m_links.at(0) : 0;
}

const BranchBase *ShotgunField::link() const
{
    return m_links.length() == 1 ? m_links.at(0) : 0;
}

void ShotgunField::setLink(BranchBase *b)
{
    if (m_links.length() != 1 || m_links.at(0) != b) {
        m_links.clear();
        m_links.append(b);
        emit linksChanged();
    }
}

const QString ShotgunField::linkType() const
{
    return m_linkTypes.length() == 1 ? m_linkTypes.at(0) : QString();
}

void ShotgunField::setLinkType(const QString linkType)
{
    if (m_linkTypes.length() != 1 || m_linkTypes.at(0) != linkType) {
        m_linkTypes.clear();
        m_linkTypes.append(linkType);
        emit linkTypesChanged();
    }
}

QQmlListProperty<BranchBase> ShotgunField::links_()
{
    return QQmlListProperty<BranchBase>(this, m_links);
}

const QStringList ShotgunField::linkTypes() const
{
    return m_linkTypes;
}

void ShotgunField::setLinkTypes(const QStringList l)
{
    if (m_linkTypes != l) {
        m_linkTypes = l;
        emit linkTypesChanged();
    }
}

const QString ShotgunField::pattern() const
{
    return m_pattern;
}

void ShotgunField::setPattern(const QString p)
{
    if (m_pattern != p) {
        m_pattern = p;
        emit patternChanged(m_pattern);
    }
}

const QVariant ShotgunField::value() const
{
    return m_value;
}

void ShotgunField::setValue(const QVariant v)
{
    if (m_value != v) {
        m_value = v;
        emit valueChanged(m_value);
    }
}

const QVariant ShotgunField::buildValue(const BranchBase *branch, const QVariantMap fields) const
{
    const Field *field = 0;
    if (!m_fieldName.isEmpty()) {
        // find the field
        field = branch->findField(m_fieldName);

        if (!field) {
            error() << "can't find field" << m_fieldName;
            return QVariant();
        }
    }

    QVariant result;
    debug() << "building value from" << branch << "with" << QJsonDocument::fromVariant(fields).toJson().constData();
    if (m_type == ShotgunField::String) {
        if (field) {
            QVariant value = field->get(fields);
            if (value.isValid())
                result = field->format(value);
            else
                error() << "can't find value for field" << m_fieldName;
        } else {
            result = m_value;
        }

    } else if (m_type == ShotgunField::Number) {
        if (field) {
            result = field->get(fields);
            if (!result.isValid())
                error() << "can't find value for field" << m_fieldName;
        } else {
            result = m_value;
        }

    } else if (m_type == ShotgunField::Path) {
        if (branch->elements().length() > 0) {
            int index = branch->property("ShotgunOperation.index").toInt();
            result = branch->elements().at(index)->path();
        } else {
            error() << "on" << branch<< "specifies a type of Path but" << branch << ".elements has no entries";
        }

    } else if (m_type == ShotgunField::Pattern) {
        if (branch->elements().length() > 0) {
            int index = branch->property("ShotgunOperation.index").toInt();
            QVariantMap data = branch->parse(branch->elements().at(index)->path()).toMap();
            for (QVariantMap::const_iterator i = fields.constBegin(); i != fields.constEnd(); ++i) {
                if (!data.contains(i.key()))
                    data.insert(i.key(), i.value());
            }
            result = branch->formatFields(m_pattern, data);
        } else {
            error() << "on" << branch << "specifies a type of Pattern but" << branch << ".elements has no entries";
        }

    } else if (m_type == ShotgunField::Link) {
        // this field is an entity link
        if (m_links.length() == 1 && m_linkTypes.length() == 1) {
            const ShotgunOperationAttached *linkAttached = qobject_cast<const ShotgunOperationAttached *>(qmlAttachedPropertiesObject<ShotgunOperation>(link(), false));
            Q_ASSERT(linkAttached);

            if (linkAttached->entities().contains(linkType()) &&
                    linkAttached->entities()[linkType()].toMap().contains("id")) {
                QVariantMap link;
                link["type"] = linkType();
                link["id"] = linkAttached->entities()[linkType()].toMap()["id"];
                result = link;
            } else {
                error() << "on" << branch << link() << "has no entity of type" << linkType();
            }
        }
    } else if (m_type == ShotgunField::MultiLink) {
        // this field is an entity multi-link
        if (m_links.length() == m_linkTypes.length()) {
            QVariantList links;
            for (int i = 0; i < m_links.length(); i++) {
                const ShotgunOperationAttached *linkAttached = qobject_cast<const ShotgunOperationAttached *>(qmlAttachedPropertiesObject<ShotgunOperation>(m_links.at(i), false));
                Q_ASSERT(linkAttached);

                if (linkAttached->entities().contains(m_linkTypes.at(i))) {
                    QVariantMap link;
                    link["type"] = m_linkTypes.at(i);
                    link["id"] = linkAttached->entities()[m_linkTypes.at(i)].toMap()["id"];
                    links.append(link);
                } else {
                    error() << "on" << branch<< linkAttached << "has no entity of type" << m_linkTypes.at(i);
                }
            }
            result = links;
        } else {
            error() << "on" << branch << "mismatch between numbers of links and linkTypes";
        }
    } else {
        error() << "on" << branch << "has an unsupported type " << m_type;
    }

    debug() << ">>" << result;
    return result;
}

ShotgunUrlField::ShotgunUrlField(QObject *parent) :
    ShotgunField(parent),
    m_linkType(ShotgunUrlField::Local)
{
}

ShotgunUrlField::LinkType ShotgunUrlField::linkType() const
{
    return m_linkType;
}

void ShotgunUrlField::setLinkType(ShotgunUrlField::LinkType lt)
{
    if (m_linkType != lt) {
        m_linkType = lt;
        emit linkTypeChanged(lt);
    }
}

const QString ShotgunUrlField::contentType() const
{
    return m_contentType;
}

void ShotgunUrlField::setContentType(const QString ct)
{
    if (m_contentType != ct) {
        m_contentType = ct;
        emit contentTypeChanged(ct);
    }
}

const QVariant ShotgunUrlField::localStorage() const
{
    return m_localStorage;
}

void ShotgunUrlField::setLocalStorage(const QVariant s)
{
    if (m_localStorage != s) {
        m_localStorage = s;
        emit localStorageChanged(s);
    }
}

const QUrl ShotgunUrlField::url() const
{
    return m_url;
}

void ShotgunUrlField::setUrl(const QUrl url)
{
    if (m_url != url) {
        m_url = url;
        emit urlChanged(url);
    }
}

const QVariant ShotgunUrlField::buildValue(const BranchBase *branch, const QVariantMap fields) const
{
    if (m_linkType == ShotgunUrlField::Web && m_url.isEmpty()) {
        error() << "missing or null url property";
        return QVariant();
    }

    QString localPath;
    if ((m_linkType == ShotgunUrlField::Local || m_linkType == ShotgunUrlField::Upload) &&
            branch->elements().length() > 0)
    {
        int index = branch->property("ShotgunOperation.index").toInt();
        localPath = branch->elements().at(index)->path();
    } else {
        error() << "specifies a Url field but" << branch << ".elements has no entries";
        return QVariant();
    }

    QVariantMap result;
    QVariant name = ShotgunField::buildValue(branch, fields);
    if (name.isValid())
        result["name"] = name;

    if (!m_contentType.isEmpty())
        result["content_type"] = m_contentType;

    switch (m_linkType) {
    case ShotgunUrlField::Local:
        result["link_type"] = "local";
        result["local_path"] = localPath;
        if (m_localStorage.isValid())
            result["local_storage"] = m_localStorage;
        break;

    case ShotgunUrlField::Upload:
        result["link_type"] = "upload";
        result["local_path"] = localPath;
        break;

    case ShotgunUrlField::Web:
        result["link_type"] = "web";
        result["url"] = m_url;
        break;
    }

    return result;
}
