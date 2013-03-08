#include "shotgunfield.h"
#include "shotgunentity.h"
#include "branchbase.h"
#include "shotgunoperation.h"

ShotgunField::ShotgunField(QObject *parent) :
    NodeBase(parent),
    m_type(ShotgunField::String),
    m_file(),
    m_urlType(ShotgunField::Local)
{
    addInput("entity");
    addInput("file");
    addInput("links");

    addParam("field");
    addParam("type");
    addParam("pattern");
    addParam("value");
    addParam("displayName");
    addParam("urlType");
    addParam("contentType");
    addParam("localStorage");

    connect(this, &ShotgunField::shotgunPullAtIndex, this, &ShotgunField::onShotgunPullAtIndex);
    connect(this, &ShotgunField::shotgunPushAtIndex, this, &ShotgunField::onShotgunPushAtIndex);
}

ShotgunEntity *ShotgunField::entity()
{
    return firstParent<ShotgunEntity>();
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

QQmlListProperty<ShotgunEntity> ShotgunField::links_()
{
    return QQmlListProperty<ShotgunEntity>(this, m_links);
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

NodeBase *ShotgunField::file()
{
    return m_file;
}

void ShotgunField::setFile(NodeBase *file)
{
    if (m_file != file) {
        m_file = file;
        emit fileChanged(file);
    }
}

const QString ShotgunField::displayName() const
{
    return m_displayName;
}

void ShotgunField::setDisplayName(const QString n)
{
    if (m_displayName != n) {
        m_displayName = n;
        emit displayNameChanged(n);
    }
}

ShotgunField::UrlType ShotgunField::urlType() const
{
    return m_urlType;
}

void ShotgunField::setUrlType(ShotgunField::UrlType lt)
{
    if (m_urlType != lt) {
        m_urlType = lt;
        emit urlTypeChanged(lt);
    }
}

const QString ShotgunField::contentType() const
{
    return m_contentType;
}

void ShotgunField::setContentType(const QString ct)
{
    if (m_contentType != ct) {
        m_contentType = ct;
        emit contentTypeChanged(ct);
    }
}

const QVariant ShotgunField::localStorage() const
{
    return m_localStorage;
}

void ShotgunField::setLocalStorage(const QVariant s)
{
    if (m_localStorage != s) {
        m_localStorage = s;
        emit localStorageChanged(s);
    }
}

void ShotgunField::onShotgunPullAtIndex(int index, const QVariant context, Shotgun *shotgun)
{
    Q_UNUSED(shotgun);
    info() << "Pull at index" << index << "with context" << context;
    trace() << ".pullAtIndex(" << index << "," << context << ")";

    QVariant value = buildValue(index, context.toMap());

    if (!value.isNull() && value.isValid()) {
        info() << "Got value" << value;

        details().property(index).setProperty("value", toScriptValue(value));
        detailsChanged();
        emit shotgunPulledAtIndex(index, OperationAttached::Finished);
    } else {
        error() << "Unable to build value";
        emit shotgunPulledAtIndex(index, OperationAttached::Error);
    }
}

void ShotgunField::onShotgunPushAtIndex(int index, const QVariant context, Shotgun *shotgun)
{
    Q_UNUSED(shotgun);
    info() << "Push at index" << index << "with context" << context;
    trace() << ".pushAtIndex(" << index << "," << context << ")";

    QVariant value = buildValue(index, context.toMap());

    if (!value.isNull() && value.isValid()) {
        info() << "Got value" << value;

        details().property(index).setProperty("value", toScriptValue(value));
        detailsChanged();
        emit shotgunPushedAtIndex(index, OperationAttached::Finished);
    } else {
        error() << "Unable to build value";
        emit shotgunPushedAtIndex(index, OperationAttached::Error);
    }
}

const QVariant ShotgunField::buildValue(int index, const QVariantMap context) const
{
    trace() << ".buildValueAtIndex(" << index << "," << context << ")";

    QVariant result;

    switch(m_type) {
    case ShotgunField::String:
        result = buildStringValue(context);
        break;
    case ShotgunField::Number:
        result = buildNumberValue(context);
        break;
    case ShotgunField::Path:
        result = buildPathValue(index);
        break;
    case ShotgunField::Pattern:
        result = buildPatternValue(index, context);
        break;
    case ShotgunField::Link:
        result = buildLinkValue(index);
        break;
    case ShotgunField::MultiLink:
        result = buildMultiLinkValue(index);
        break;
    case ShotgunField::Url:
        result = buildUrlValue(index);
        break;
    default:
        error() << "Unknown type" << static_cast<int>(m_type);
        break;
    }

    return result;
}

const Field *ShotgunField::field() const
{
    if (entity()) {
        if (entity()->branch()) {
            return entity()->branch()->findField(m_fieldName);
        } else {
            error() << "Entity has no branch";
        }
    } else {
        error() << "Field has no entity";
    }
}

const QVariant ShotgunField::buildStringValue(const Field *field, const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::String);

    QVariant result;
    if (!m_fieldName.isEmpty()) {
        if (field()) {
            QVariant value = field->get(context);
            if (value.isValid())
                result = field->format(value);
            else
                error() << "Can't get value for field" << m_fieldName;
        } else {
            error() << "Can't find field" << m_fieldName;
        }
    } else if (m_value.canConvert<QString>()) {
        result = m_value.toString();
    } else {
        error() << "Can't convert" << m_value << "to a string";
    }
    return result;
}

const QVariant ShotgunField::buildNumberValue(const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::Number);

    QVariant result;
    if (!m_fieldName.isEmpty()) {
        if (field()) {
            QVariant value = field->get(context);
            if (value.isValid())
                result = field->format(value);
            else
                error() << "Can't get value for field" << m_fieldName;
        } else {
            error() << "Can't find field" << m_fieldName;
        }
    } else if (m_value.canConvert<double>()) {
        result = m_value.toDouble();
    } else {
        error() << "Can't convert" << m_value << "to a number";
    }

    return result;
}

const QVariant ShotgunField::buildPathValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::Path || m_type == ShotgunField::Url);

    QVariant result;
    if (m_file && m_file->details().isArray() && m_file->details().property("length").toInt() > index) {
        result = qjsvalue_cast<Element *>(m_file->details().property(index).property("element"))->pattern();
    } else {
        error() << "Can't get detail" << index << "from" << m_file;
    }

    return result;
}

const QVariant ShotgunField::buildPatternValue(int index, const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::Pattern);

    QVariant result;
    if (entity()) {
        if (entity()->branch()) {
            if (branch()->details().isArray() && branch()->details().property("length").toInt() > index) {
                QVariantMap data = qjsvalue_cast<QVariantMap>(branch->details().property(index).property("context"));
                for (QVariantMap::const_iterator i = context.constBegin(); i != context.constEnd(); ++i) {
                    if (!data.contains(i.key()))
                        data.insert(i.key(), i.value());
                }

                if (branch->fieldsComplete(m_pattern, data)) {
                    result = branch->formatFields(m_pattern, data);
                } else {
                    // run it anyway, so we can log the errors
                    branch->formatFields(m_pattern, data);
                    error() << branch << "missing fields for" << m_pattern;
                }
            } else {
                error() << "Can't get detail" << index << "from" << branch;
            }
        } else {
            error() << "Entity has no branch";
        }
    } else {
        error() << "Field has no entity";
    }

    return result;
}

const QVariant ShotgunField::buildLinkValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::Link);

    // this field is an entity link
    QVariant result;
    if (m_links.length() == 1) {
        const NodeBase *linkedEntity = m_links.at(0);

        if (linkedEntity) {
            const QVariantMap sg_entity = qjsvalue_cast<QVariantMap>(linkedEntity->detail(index, "entity"));

            if (sg_entity.contains("id")) {
                // Shotgun entity type is the linked ShotgunEntity's name
                QVariantMap link;
                link["type"] = linkedEntity->name();
                link["id"] = sg_entity["id"];
                result = link;
            } else {
                error() << "linked entity" << linkedEntity << "has no id";
            }
        } else {
            error() << "linked entity is null";
        }
    } else if (m_links.length() == 0) {
        error() << "links is empty";
    } else {
        error() << m_links.length() << "linked entities but linkType is not MultiLink";
    }

    return result;
}

const QVariant ShotgunField::buildMultiLinkValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::MultiLink);

    // this field is an entity multi-link
    QVariantList result;
    for (int i = 0; i < m_links.length(); i++) {
        if (m_links[i]) {
            const QVariantMap sg_entity = qjsvalue_cast<QVariantMap>(link()->detail(index, "entity"));

            if (sg_entity.contains("id")) {
                QVariantMap link;
                // Shotgun entity type is the linked ShotgunEntity's name
                link["type"] = m_links[i]->name();
                link["id"] = sg_entity["id"];
                result.append(link);
            } else {
                error() << "linked entity" << i << m_links[i] << "has no id";
            }
        } else {
            error() << "linked entity" << i << m_links[i] << "is null";
        }
    }

    return result;
}

const QVariant ShotgunField::buildUrlValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::Url);

    QVariant result;
    if (m_urlType == ShotgunField::Web) {
        if (!m_value.isNull() && m_value.isValid()) {
            if (m_value.canConvert<QUrl>())
                result = m_value.toUrl();
            else if (m_value.canConvert<QString>())
                result = QUrl(m_value.toString());
            else
                error() << "Can't convert value" << m_value << "to url";
        } else {
            error() << "Missing or null value";
        }
    } else if (m_urlType == ShotgunField::Local || m_urlType == ShotgunField::Upload) {
        QVariantMap file;
        // use the value for the name
        // TODO: maybe we should have a separate "name" property to be explicit?
        if (!m_value.isNull() && m_value.isValid() && m_value.canConvert<QString>())
            file["name"] = m_value.toString();

        if (!m_contentType.isEmpty())
            file["content_type"] = m_contentType;

        QVariant localPath = buildPathValue(index);

        if (!localPath.isNull() && localPath.isValid()) {
            if (m_urlType == ShotgunField::Local) {
                file["link_type"] = "local";
                file["local_path"] = localPath;
                if (m_localStorage.isValid())
                    file["local_storage"] = m_localStorage;
            } else if (m_urlType == ShotgunField::Upload) {
                file["link_type"] = "upload";
                file["local_path"] = localPath;
            }
            result = file;
        } else {
            error() << "Can't get path for" << m_file;
        }
    }

    return result;
}
