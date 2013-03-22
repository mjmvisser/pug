#include "shotgunfield.h"
#include "shotgunentity.h"
#include "branch.h"
#include "shotgunoperation.h"
#include "elementsview.h"

ShotgunField::ShotgunField(QObject *parent) :
    Node(parent),
    m_type(ShotgunField::String),
    m_file(),
    m_urlType(ShotgunField::Local)
{
    setActive(true);

    addInput(this, "file");
    addInput(this, "links");

    addParam(this, "field");
    addParam(this, "type");
    addParam(this, "pattern");
    addParam(this, "value");
    addParam(this, "displayName");
    addParam(this, "urlType");
    addParam(this, "contentType");
    addParam(this, "localStorage");

    connect(this, &ShotgunField::shotgunPullAtIndex, this, &ShotgunField::onShotgunPullAtIndex);
    connect(this, &ShotgunField::shotgunPushAtIndex, this, &ShotgunField::onShotgunPushAtIndex);
    connect(this, &ShotgunField::entityChanged, this, &ShotgunField::updateCount);
}

void ShotgunField::componentComplete()
{
    Node::componentComplete();
    if (entity())
        connect(entity(), &Node::countChanged, this, &Node::setCount);

    updateCount();
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

const ShotgunEntity *ShotgunField::entity() const
{
    return firstParent<ShotgunEntity>();
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

ShotgunEntity *ShotgunField::link()
{
    return m_links.length() == 1 ? m_links.at(0) : 0;
}

const ShotgunEntity *ShotgunField::link() const
{
    return m_links.length() == 1 ? m_links.at(0) : 0;
}

void ShotgunField::setLink(ShotgunEntity *e)
{
    if (m_links.length() != 1 || m_links.at(0) != e) {
        m_links.clear();
        m_links << e;
        emit linksChanged();
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

Node *ShotgunField::file()
{
    return m_file;
}

void ShotgunField::setFile(Node *file)
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
    info() << "Shotgun pull at index" << index << "with context" << context;
    trace() << ".shotgunPullAtIndex(" << index << "," << context << ")";

    setIndex(index);

    QVariant value = buildValue(index, context.toMap());

    if (value.isValid()) {
        // TODO: Qt bug? null QVariant converts to a non-null QJSValue
        setDetail(index, "value", value.isNull() ? QJSValue(QJSValue::NullValue) : toScriptValue(value));
        info() << "Shotgun pull result is" << details();
        emit shotgunPulledAtIndex(index, OperationAttached::Finished);
    } else {
        error() << "Unable to build value";
        emit shotgunPulledAtIndex(index, OperationAttached::Error);
    }
}

void ShotgunField::onShotgunPushAtIndex(int index, const QVariant context, Shotgun *shotgun)
{
    Q_UNUSED(shotgun);
    info() << "Shotgun push at index" << index << "with context" << context;
    trace() << ".shotgunPushAtIndex(" << index << "," << context << ")";

    setIndex(index);

    QVariant value = buildValue(index, context.toMap());

    if (value.isValid()) {
        info() << "Got value" << value;
        // TODO: Qt bug? null QVariant converts to a non-null QJSValue
        setDetail(index, "value", value.isNull() ? QJSValue(QJSValue::NullValue) : toScriptValue(value));
        info() << "Shotgun push result is" << details().property(index);
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

    return 0;
}

const QVariant ShotgunField::buildStringValue(const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::String);

    QVariant result;
    if (!m_fieldName.isEmpty()) {
        if (field()) {
            QVariant value = field()->get(context);
            if (value.isValid())
                result = field()->format(value);
            else
                error() << "Can't get value for field" << m_fieldName;
        } else {
            error() << "Can't find field" << m_fieldName;
        }
    } else if (m_value.canConvert<QString>()) {
        result = m_value.toString();
    } else {
        result = QVariant(QVariant::String); // null
    }
    return result;
}

const QVariant ShotgunField::buildNumberValue(const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::Number);

    QVariant result;
    if (!m_fieldName.isEmpty()) {
        if (field()) {
            QVariant value = field()->get(context);
            if (value.isValid())
                result = field()->format(value);
            else
                error() << "Can't get value for field" << m_fieldName;
        } else {
            error() << "Can't find field" << m_fieldName;
        }
    } else if (m_value.canConvert<double>()) {
        result = m_value.toDouble();
    } else if (!m_value.isValid() || m_value.isNull()) {
        result = QVariant(QVariant::String); // null value
    }

    return result;
}

const QVariant ShotgunField::buildPathValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::Path || m_type == ShotgunField::Url);

    if (!m_file) {
        error() << "ShotgunField.Path type requires a node connected to the file property";
        return QVariant();
    }

    const QScopedPointer<const ElementsView> elementsView(new ElementsView(m_file));
    const ElementView *element = elementsView->elementAt(index);

    QVariant result;
    if (element) {
        result = element->pattern();
    } else {
        error() << "Can't get detail" << index << "from" << m_file;
    }

    return result;
}

const QVariant ShotgunField::buildPatternValue(int index, const QVariantMap context) const
{
    Q_ASSERT(m_type == ShotgunField::Pattern);

    const Branch *branch = qobject_cast<const Branch *>(m_file);

    if (!branch) {
        error() << "ShotgunField.Pattern type requires a branch connected to the file property";
        return QVariant();
    }

    QVariant result;
    if (index < branch->count()) {
        QVariantMap branchContext = branch->context(index);
        QMapIterator<QString, QVariant> i(context);
        while (i.hasNext()) {
            i.next();
            branchContext.insert(i.key(), i.value());
        }

        if (branch->fieldsComplete(m_pattern, context)) {
            QString value = branch->formatFields(m_pattern, context);
            if (!value.isEmpty())
                result = value;
            else
                error() << branch << ".formatFields(" << m_pattern << "," << context << ") returned null";
        } else {
            // run it anyway, so we can log the errors
            branch->formatFields(m_pattern, context);
            error() << branch << "missing fields for" << m_pattern;
        }
    } else {
        error() << "Can't get detail" << index << "from" << branch;
    }

    return result;
}

const QVariant ShotgunField::buildLinkValue(int index) const
{
    Q_ASSERT(m_type == ShotgunField::Link);

    // this field is an entity link
    QVariant result;
    if (m_links.length() == 1) {
        const Node *linkedEntity = m_links.at(0);

        if (linkedEntity) {
            const QVariantMap sg_entity = linkedEntity->details().property(index).property("entity").toVariant().toMap();

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
        }
    } else if (m_links.length() == 0) {
        result = QVariant(QVariant::String); // null
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
            const QVariantMap sg_entity = link()->details().property(index).property("entity").toVariant().toMap();

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
        if (!m_displayName.isEmpty())
            file["name"] = m_displayName;

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


void ShotgunField::updateCount()
{
    if (entity()) {
        setCount(entity()->count());
    }
}
