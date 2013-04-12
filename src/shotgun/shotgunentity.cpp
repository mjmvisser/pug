#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branch.h"
#include "shotgun.h"
#include "elementsview.h"

ShotgunEntity::ShotgunEntity(QObject *parent) :
    Node(parent),
    m_action(ShotgunEntity::Find),
    m_limit(0)
{
    addInput(this, "branch");
    addInput(this, "shotgunFields");

    connect(this, &ShotgunEntity::update, this, &ShotgunEntity::onUpdate);
    connect(this, &ShotgunEntity::release, this, &ShotgunEntity::onRelease);
}

const QString ShotgunEntity::shotgunEntityName() const
{
    return m_shotgunEntityName;
}

void ShotgunEntity::setShotgunEntityName(const QString sen)
{
    if (m_shotgunEntityName != sen) {
        m_shotgunEntityName = sen;
        emit shotgunEntityNameChanged(sen);
    }
}

const Branch *ShotgunEntity::branch() const
{
    // TODO: how do we emit branchChanged() when this changes?
    return firstParent<Branch>();
}

Branch *ShotgunEntity::branch()
{
    return firstParent<Branch>();
}

QQmlListProperty<ShotgunField> ShotgunEntity::shotgunFields_()
{
    return QQmlListProperty<ShotgunField>(this, 0, ShotgunEntity::shotgunFields_append,
                                        ShotgunEntity::shotgunFields_count,
                                        ShotgunEntity::shotgunField_at,
                                        ShotgunEntity::shotgunFields_clear);
}

// shotgunFields property
void ShotgunEntity::shotgunFields_append(QQmlListProperty<ShotgunField> *prop, ShotgunField *f)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);

    that->m_shotgunFields.append(f);
    emit that->shotgunFieldsChanged();
}

int ShotgunEntity::shotgunFields_count(QQmlListProperty<ShotgunField> *prop)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);
    return that->m_shotgunFields.count();
}

ShotgunField *ShotgunEntity::shotgunField_at(QQmlListProperty<ShotgunField> *prop, int i)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);
    if (i < that->m_shotgunFields.count())
        return that->m_shotgunFields[i];
    else
        return 0;
}

void ShotgunEntity::shotgunFields_clear(QQmlListProperty<ShotgunField> *prop)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);
    that->m_shotgunFields.clear();
    emit that->shotgunFieldsChanged();
}

const QList<const ShotgunField *> ShotgunEntity::shotgunFields() const
{
    return constList(m_shotgunFields);
}

ShotgunEntity::Action ShotgunEntity::action() const
{
    return m_action;
}

void ShotgunEntity::setAction(ShotgunEntity::Action a)
{
    if (m_action != a) {
        m_action = a;
        emit actionChanged(m_action);
    }
}

const QVariantList ShotgunEntity::filters() const
{
    return m_filters;
}

void ShotgunEntity::setFilters(const QVariantList f)
{
    if (m_filters != f) {
        m_filters = f;
        emit filtersChanged(f);
    }
}

const QVariantList ShotgunEntity::order() const
{
    return m_order;
}

void ShotgunEntity::setOrder(const QVariantList o)
{
    if (m_order != o) {
        m_order = o;
        emit orderChanged(o);
    }
}

int ShotgunEntity::limit() const
{
    return m_limit;
}

void ShotgunEntity::setLimit(int l)
{
    if (m_limit != l) {
        m_limit = l;
        emit limitChanged(l);
    }
}

void ShotgunEntity::onUpdate(const QVariant context)
{
    clearDetails();
    if (m_action == ShotgunEntity::Find) {
        trace() << ".onUpdate(" << context << "," << ")";

        if (count() > 0) {
            debug() << "Updating" << count() << "details with context" << context;

            QVariant f = allFilters();
            if (f.isValid()) {
                m_context = context.toMap();

                debug() << "Shotgun filters are" << QJsonDocument::fromVariant(f).toJson();
                ShotgunReply *reply = Shotgun::staticInstance()->find(shotgunEntityName(), f.toList(), fields(), order(), "all", limit());

                connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onFindFinished);
                connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                        this, &ShotgunEntity::onShotgunReadError);
            } else {
                debug() << "Skipping, incomplete filters";
                emit updateFinished(OperationAttached::Finished);
            }
        } else {
            debug() << "no details";
            emit updateFinished(OperationAttached::Finished);
        }
    } else {
        emit updateFinished(OperationAttached::Finished);
    }
}

void ShotgunEntity::onRelease(const QVariant context)
{
    if (m_action == ShotgunEntity::Create) {
        trace() << ".onRelease(" << context << "," << ")";

        QVariant d = data();
        if (d.isValid()) {
            setCount(d.toList().length());

            if (d.toList().length() == 1) {
                debug() << "request:" << d.toList()[0].toMap();
                m_context = context.toMap();
                QVariant di = d.toList()[0];
                ShotgunReply *reply = Shotgun::staticInstance()->create(shotgunEntityName(), di.toMap(), fields());
                connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onCreateFinished);
                connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                        this, &ShotgunEntity::onShotgunCreateError);
            } else if (d.toList().length() > 1) {
                QVariantList batchRequests;
                QStringList returnFields = fields();

                foreach (const QVariant di, d.toList()) {
                    QVariantMap batchRequest;

                    batchRequest["request_type"] = "create";
                    batchRequest["entity_type"] = shotgunEntityName();
                    batchRequest["data"] = di;
                    if (returnFields.length() > 0)
                        batchRequest["return_fields"] = returnFields;

                    batchRequests.append(batchRequest);
                }

                debug() << "requests:" << batchRequests;

                ShotgunReply *reply = Shotgun::staticInstance()->batch(batchRequests);
                connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onBatchCreateFinished);
                connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                        this, &ShotgunEntity::onShotgunCreateError);
            } else {
                error() << "has no data";
                emit releaseFinished(OperationAttached::Error);
            }
        } else {
            error() << "Incomplete data";
            emit releaseFinished(OperationAttached::Error);
        }
    } else {
        emit releaseFinished(OperationAttached::Finished);
    }
}

const QVariant ShotgunEntity::data() const
{
    trace() << ".data()";
    Q_ASSERT(branch());

    // data is cartesian product of all fields
    // (most will be of length 1)

    QList<int> sgfIndex;
    QList<int> sgfCount;
    foreach (const ShotgunField *sgf, m_shotgunFields) {
        if (sgf) {
            sgfCount << sgf->count();
            sgfIndex << 0;
        }
    }

    QVariantList result;

    while (true) {
        // add the data at our current indices
        QVariantMap d;

        // build data for this entity from the entity's fields
        int i = 0;
        foreach (const ShotgunField *sgf, m_shotgunFields) {
            if (sgf) {
                // name: value pairs
                QJSValue jsValue = sgf->details().property(sgfIndex[i]).property("value");
                QVariant value;
                if (jsValue.isNull())
                    value = QVariant(); // invalid QVariant
                else
                    value = qjsvalue_cast<QVariant>(jsValue);

                if (value.isNull() && sgf->isRequired()) {
                    error() << sgf << "is marked required but is null";
                    return QVariant();
                }

                d.insert(sgf->shotgunFieldName(), value);

                i++;
            }
        }
        result << d;

        // increment our iterators, like an odometer, but from left-to-right
        // inspired by http://stackoverflow.com/questions/5279051/how-can-i-create-cartesian-product-of-vector-of-vectors
        for (i = 0; i < sgfCount.length(); i++) {
            sgfIndex[i]++;

            if (sgfIndex[i] == sgfCount[i]) {
                if (i + 1 == sgfCount.length()) {
                    // last counter is about to roll over, we're done
                    trace() << "    ->" << result;
                    return result;
                } else {
                    // this counter is at the end, reset and roll over
                    sgfIndex[i] = 0;
                }
            } else {
                break;
            }
        }
    }

    return QVariant();
}

const QVariant ShotgunEntity::jsValueToVariant(const QJSValue &jsValue)
{
    QVariant value;
    if (jsValue.isNull())
        value = QVariant(QVariant::String); // null QVariant
    else if (jsValue.isUndefined())
        value = QVariant(); // invalid QVariant
    else
        value = qjsvalue_cast<QVariant>(jsValue);

    return value;
}

const QVariant ShotgunEntity::allFilters()
{
    trace() << ".filters()";
    Q_ASSERT(count() > 0);

    QVariantList result;

    // build filters for this entity from the entity's shotgun fields
    foreach (ShotgunField *sgf, m_shotgunFields) {
        if (sgf) {
            if (sgf->type() != ShotgunField::Url) {
                // we skip Url fields, since they can't be filtered against
                // ["<shotgun field name>", "is", <value for field name>]
                if (sgf->count() == 1) {
                    QVariant value = jsValueToVariant(sgf->details().property(0).property("value"));
                    sgf->setIndex(0);
                    Q_ASSERT(value.isValid() || value.isNull());
                    // we skip null fields... TODO: what if we want to match against a null field?
                    if (!value.isNull()) {
                        QVariantList filter;
                        filter << sgf->shotgunFieldName() << "is" << value;
                        result << QVariant(filter); // have to wrap filter in a QVariant, otherwise it concatenates the lists!
                    }
                } else if (sgf->count() > 0) {
                    QVariantList filters;
                    for (int i = 0; i < sgf->count(); i++) {
                        sgf->setIndex(i);
                        QVariant value = jsValueToVariant(sgf->details().property(i).property("value"));
                        Q_ASSERT(value.isValid());

                        QVariantList filter;
                        filter << sgf->shotgunFieldName() << "is" << value;
                        filters << QVariant(filter);
                    }

                    QVariantMap complexFilter;
                    complexFilter.insert("filter_operator", "any");
                    complexFilter.insert("filters", filters);
                    result << complexFilter;
                } else {
                    // count is 0
                    if (sgf->isRequired()) {
                        debug() << "field" << sgf << "is marked required, but has no data";
                        return QVariant();
                    }
                }
            }
        }
    }

    // add custom filters
    result << m_filters;

    trace() << "    ->" << result;
    return result;
}



const QStringList ShotgunEntity::fields() const
{
    trace() << ".fields()";
    Q_ASSERT(branch());

    QStringList result;

    // build a list of Shotgun fields
    foreach (const ShotgunField *sgf, m_shotgunFields) {
        if (sgf) {
            result << sgf->shotgunFieldName();
        }
    }

    trace() << "    ->" << result;
    return result;
}

void ShotgunEntity::onFindFinished(const QVariant results)
{
    trace() << ".onFindFinished(" << results << ")";

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    int i = 0;
    foreach (const QVariant result, results.toList()) {
        setContext(i, m_context);
        setDetail(i, "entity", toScriptValue(result.toMap()));

        // path fields become elements
        foreach (const ShotgunField *sgf, m_shotgunFields) {
            if (sgf) {
                const QString shotgunFieldName = sgf->shotgunFieldName();
                if (sgf->type() == ShotgunField::Path && !shotgunFieldName.isEmpty()) {
                    elementsView->elementAt(i)->setPattern(result.toMap()[shotgunFieldName].toString());
                    elementsView->elementAt(i)->scan();
                }
            }
        }

        i++;
    }

    QObject::sender()->deleteLater();

    emit updateFinished(OperationAttached::Finished);
}

void ShotgunEntity::onCreateFinished(const QVariant result)
{
    trace() << ".onCreateFinished(" << result << ")";
    debug() << "Shotgun.create returned" << result;

    setDetail(0, "entity", toScriptValue(result.toMap()));

    QObject::sender()->deleteLater();

    emit releaseFinished(OperationAttached::Finished);
}

void ShotgunEntity::onBatchCreateFinished(const QVariant result)
{
    trace() << ".onBatchCreateFinished(" << result << ")";
    debug() << "Shotgun.batch returned" << result;

    for (int i = 0; i < result.toList().length(); i++) {
        setDetail(i, "entity", toScriptValue(result.toList().at(i).toMap()));
    }

    QObject::sender()->deleteLater();

    debug() << "received" << QJsonDocument::fromVariant(result);

    emit releaseFinished(OperationAttached::Finished);
}

void ShotgunEntity::onShotgunReadError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    trace() << ".onShotgunReadError() [error is" << reply->errorString() << "]";
    error() << "Shotgun.find failed  [error is" << reply->errorString() << "]";

    reply->deleteLater();

    emit updateFinished(OperationAttached::Error);
}

void ShotgunEntity::onShotgunCreateError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    trace() << ".onShotgunCreateError() [error is" << reply->errorString() << "]";
    error() << "Shotgun.create or .batch failed  [error is" << reply->errorString() << "]";

    reply->deleteLater();

    emit releaseFinished(OperationAttached::Error);
}
