#include "shotgunfield.h"
#include "shotgunentity.h"
#include "branch.h"
#include "elementsview.h"
#include "shotgun.h"

ShotgunField::ShotgunField(QObject *parent) :
    Node(parent),
    m_type(ShotgunField::String),
    m_requiredFlag(false),
    m_source(),
    m_urlType(ShotgunField::Local)
{
    addInput(this, "source");
    addInput(this, "links");

    addParam(this, "field");
    addParam(this, "type");
    addParam(this, "required");
    addParam(this, "pattern");
    addParam(this, "value");
    addParam(this, "displayName");
    addParam(this, "urlType");
    addParam(this, "contentType");
    addParam(this, "localStorage");

    connect(this, &ShotgunField::update, this, &ShotgunField::onUpdate);
    connect(this, &ShotgunField::release, this, &ShotgunField::onRelease);
}

const QString ShotgunField::shotgunFieldName() const
{
    return m_shotgunFieldName;
}

void ShotgunField::setShotgunFieldName(const QString sfn)
{
    if (m_shotgunFieldName != sfn) {
        m_shotgunFieldName = sfn;
        emit shotgunFieldNameChanged(sfn);
    }
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

bool ShotgunField::isRequired() const
{
    return m_requiredFlag;
}

void ShotgunField::setRequired(bool f)
{
    if (m_requiredFlag != f) {
        m_requiredFlag = f;
        emit requiredChanged(f);
    }
}

Node *ShotgunField::link()
{
    return m_links.length() == 1 ? m_links[0] : 0;
}

const Node *ShotgunField::link() const
{
    return m_links.length() == 1 ? m_links[0] : 0;
}

void ShotgunField::setLink(Node *l)
{
    if (m_links.length() != 1 || m_links[0] != l) {
        m_links.clear();
        m_links << l;
        emit linksChanged();
    }
}

QQmlListProperty<Node> ShotgunField::links_()
{
    return QQmlListProperty<Node>(this, 0,
            ShotgunField::links_append,
            ShotgunField::links_count,
            ShotgunField::link_at,
            ShotgunField::links_clear);
}


// links property
void ShotgunField::links_append(QQmlListProperty<Node> *prop, Node *e)
{
    ShotgunField *that = static_cast<ShotgunField *>(prop->object);

    that->m_links.append(e);
    emit that->linksChanged();
}

int ShotgunField::links_count(QQmlListProperty<Node> *prop)
{
    ShotgunField *that = static_cast<ShotgunField *>(prop->object);
    return that->m_links.count();
}

Node *ShotgunField::link_at(QQmlListProperty<Node> *prop, int i)
{
    ShotgunField *that = static_cast<ShotgunField *>(prop->object);
    if (i < that->m_links.count()) {
        return that->m_links[i];
    } else {
        return 0;
    }
}

void ShotgunField::links_clear(QQmlListProperty<Node> *prop)
{
    ShotgunField *that = static_cast<ShotgunField *>(prop->object);
    that->m_links.clear();
    emit that->linksChanged();
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

const Node *ShotgunField::source() const
{
    return m_source;
}

Node *ShotgunField::source()
{
    return const_cast<Node *>(static_cast<const ShotgunField &>(*this).source());
}

void ShotgunField::setSource(Node *source)
{
    if (m_source != source) {
        m_source = source;
        emit sourceChanged(m_source);
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

void ShotgunField::onUpdate(const QVariant context)
{
    info() << "Update with context" << context;
    trace() << ".update(" << context << ")";

    // TODO: ugly...
    if (source())
        setCount(source()->count());
    else
        setCount(1);

    if (count() > 0) {
        OperationAttached::Status status = OperationAttached::Finished;

        for (int i = 0; i < count(); i++) {
            setIndex(i);

            QVariant value = buildValue(i, context.toMap());

            if (value.isValid()) {
                // TODO: Qt bug? null QVariant converts to a non-null QJSValue
                setDetail(i, "value", value.isNull() ? QJSValue(QJSValue::NullValue) : toScriptValue(value));
            } else {
                error() << "Unable to build value";
                status = OperationAttached::Error;
            }
        }

        emit updateFinished(status);
    } else {
        debug() << "source" << source() << "has count of" << source()->count();
        emit updateFinished(OperationAttached::Finished);
    }
}

void ShotgunField::onRelease(const QVariant context)
{
    info() << "Update with context" << context;
    trace() << ".update(" << context << ")";

    if (source())
        setCount(source()->count());
    else
        setCount(1);

    if (count() > 0) {
        OperationAttached::Status status = OperationAttached::Finished;

        for (int i = 0; i < count(); i++) {
            setIndex(i);

            QVariant value = buildValue(i, context.toMap());

            if (value.isValid()) {
                // TODO: Qt bug? null QVariant converts to a non-null QJSValue
                setDetail(i, "value", value.isNull() ? QJSValue(QJSValue::NullValue) : toScriptValue(value));
            } else {
                error() << "Unable to build value";
                status = OperationAttached::Error;
            }
        }

        emit releaseFinished(status);
    } else {
        debug() << "source" << source() << "has count of" << source()->count();
        emit releaseFinished(OperationAttached::Finished);
    }
}

const QVariant ShotgunField::buildValue(int index, const QVariantMap context)
{
    trace() << ".buildValue(" << index << "," << context << ")";

    QVariant result;

    switch(m_type) {
    case ShotgunField::String:
        result = buildStringValue(index, context);
        break;
    case ShotgunField::Number:
        result = buildNumberValue(index, context);
        break;
    case ShotgunField::Path:
        result = buildPathValue(index, context);
        break;
    case ShotgunField::Link:
        result = buildLinkValue(index, context);
        break;
    case ShotgunField::MultiLink:
        result = buildMultiLinkValue(index, context);
        break;
    case ShotgunField::Url:
        result = buildUrlValue(index, context);
        break;
    default:
        error() << "Unknown type" << static_cast<int>(m_type);
        break;
    }

    return result;
}

const Field *ShotgunField::field() const
{
    if (source()) {
        const Branch *branch = qobject_cast<const Branch *>(source());
        if (branch) {
            return branch->findField(m_fieldName);
        }
    } else {
        error() << "Source" << source() << "is not a branch";
    }
    return 0;
}

const QVariantMap ShotgunField::valueContext(int index, const QVariantMap context) const
{
    Q_ASSERT(source());

    QVariantMap result = context;
    if (index < source()->count()) {
        result = mergeContexts(context,
                source()->details().property(index).property("context").toVariant().toMap());
        debug() << "merge with" << source() << "index" << index;
        debug() << "merging" << context << "with" << source()->details().property(index).property("context").toVariant().toMap();
    } else {
        error() << source() << "index" << index << "doesn't exist";
    }

    return result;
}

const QString ShotgunField::buildPatternValue(int index, const QVariantMap context)
{
    trace() << ".buildPatternValue(" << index << "," << context << ")";
    Q_ASSERT(!m_pattern.isEmpty());

    const Branch *branch = qobject_cast<const Branch *>(source());

    if (!branch) {
        error() << "building a pattern requires a branch connected to the source property";
        return QString();
    }

    QString result;

    const QVariantMap fieldContext = valueContext(index, context);

    debug() << "formatting" << m_pattern << "with" << fieldContext;

    if (branch->fieldsComplete(fieldContext)) {
        QString value = branch->formatFields(m_pattern, fieldContext);
        if (!value.isEmpty())
            result = value;
        else
            error() << branch << ".formatFields(" << m_pattern << "," << fieldContext << ") returned null";
    }

    return result;
}

const QVariant ShotgunField::buildStringValue(int index, const QVariantMap context)
{
    Q_UNUSED(index);
    trace() << ".buildStringValue(" << index << "," << context << ")";
    Q_ASSERT(m_type == ShotgunField::String);

    QVariant result;
    if (m_value.canConvert<QString>()) {
        result = m_value.toString();
    } else if (!m_pattern.isEmpty() && source()) {
        result = buildPatternValue(index, context);
    } else if (!m_fieldName.isEmpty()) {
        if (field()) {
            QVariant value = field()->get(valueContext(index, context));
            if (value.isValid())
                result = field()->format(value);
            else
                result = QVariant(QVariant::String); // null
        } else {
            error() << "Can't find field" << m_fieldName;
        }
    } else {
        result = QVariant(QVariant::String); // null
        Q_ASSERT(result.isNull());
    }
    return result;
}

const QVariant ShotgunField::buildNumberValue(int index, const QVariantMap context)
{
    trace() << ".buildNumberValue(" << index << "," << context << ")";
    Q_ASSERT(m_type == ShotgunField::Number);

    QVariant result;
    if (!m_fieldName.isEmpty()) {
        if (field() && field()->type() == Field::Integer) {
            result = field()->get(valueContext(index, context));
            if (!result.isValid())
                result = QVariant(QVariant::String); // null value
        }
    } else if (m_value.canConvert<double>()) {
        result = m_value.toDouble();
    } else if (!m_value.isValid() || m_value.isNull()) {
        result = QVariant(QVariant::String); // null value
        Q_ASSERT(result.isNull());
    }

    return result;
}

const QVariant ShotgunField::buildPathValue(int index, const QVariantMap context)
{
    Q_UNUSED(context);

    if (!source()) {
        debug() << "no source for path specified, returning null";
        return QVariant(QVariant::String);
    }

    trace() << ".buildPathValue(" << index << "," << context << ")";
    Q_ASSERT(m_type == ShotgunField::Path || m_type == ShotgunField::Url);

    const QScopedPointer<const ElementsView> elementsView(new ElementsView(source()));
    const ElementView *element = elementsView->elementAt(index);

    QVariant result;
    if (element) {
        result = element->pattern();
    } else {
        error() << "Can't get detail" << index << "from" << source();
    }

    return result;
}

const QVariant ShotgunField::buildLinkValue(int index, const QVariantMap context)
{
    Q_UNUSED(context);

    trace() << ".buildLinkValue(" << index << "," << context << ")";
    Q_ASSERT(m_type == ShotgunField::Link);

    // this field is an entity link
    QVariant result;
    if (m_links.length() == 1) {
        const Node *linkedEntity = m_links.at(0);

        if (linkedEntity) {
            const QVariantMap sg_entity = linkedEntity->details().property(index).property("entity").toVariant().toMap();

            debug() << "building link value at index" << index << "with entity" << sg_entity;

            if (sg_entity.contains("id") && sg_entity.contains("type")) {
                QVariantMap link;
                link["type"] = sg_entity["type"];
                link["id"] = sg_entity["id"];
                result = link;
            } else {
                error() << "linked entity" << linkedEntity << "has no id or type";
            }
        } else {
            result = QVariant(QVariant::String); // null
            Q_ASSERT(result.isNull());
        }
    } else if (m_links.length() == 0) {
        result = QVariant(QVariant::String); // null
        Q_ASSERT(result.isNull());
    } else {
        error() << m_links.length() << "linked entities but linkType is not MultiLink";
    }

    return result;
}

const QVariant ShotgunField::buildMultiLinkValue(int index, const QVariantMap context)
{
    Q_UNUSED(context);

    trace() << ".buildMultiLinkValue(" << index << "," << context << ")";
    Q_ASSERT(m_type == ShotgunField::MultiLink);

    // this field is an entity multi-link
    QVariantList result;
    for (int i = 0; i < m_links.length(); i++) {
        if (m_links[i]) {
            const QVariantMap sg_entity = link()->details().property(index).property("entity").toVariant().toMap();

            debug() << "building multilink value at index" << index << "with entity" << sg_entity;

            if (sg_entity.contains("id") && sg_entity.contains("type")) {
                QVariantMap link;
                link["type"] = sg_entity["type"];
                link["id"] = sg_entity["id"];
                result << link;
            } else {
                error() << "linked entity" << i << m_links[i] << "has no id or type";
            }
        } else {
            result << QVariant(QVariant::String); // null QVariant
            Q_ASSERT(result[0].isNull());
        }
    }

    return result;
}

const QVariant ShotgunField::buildUrlValue(int index, const QVariantMap context)
{
    Q_UNUSED(context);

    trace() << ".buildUrlValue(" << index << "," << context << ")";
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
        } else if (!m_pattern.isEmpty()) {
            result = QUrl(buildPatternValue(index, context));
        } else {
            result = QVariant(QVariant::String); // null
        }
    } else if (m_urlType == ShotgunField::Local || m_urlType == ShotgunField::Upload) {
        QVariantMap file;
        if (!m_displayName.isEmpty())
            file["name"] = m_displayName;

        if (!m_contentType.isEmpty())
            file["content_type"] = m_contentType;

        QVariant localPath = buildPathValue(index, context);

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
            error() << "Can't get path for" << source();
        }
    }

    return result;
}
