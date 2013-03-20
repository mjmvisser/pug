#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branch.h"
#include "shotgunoperation.h"

ShotgunEntity::ShotgunEntity(QObject *parent) :
    Node(parent)
{
    setActive(true);

    addInput("branch");
    addInput("shotgunFields");

    connect(this, &ShotgunEntity::shotgunPull, this, &ShotgunEntity::onShotgunPull);
    connect(this, &ShotgunEntity::shotgunPush, this, &ShotgunEntity::onShotgunPush);
    connect(this, &ShotgunEntity::branchChanged, this, &ShotgunEntity::updateCount);
}

void ShotgunEntity::componentComplete()
{
    Node::componentComplete();
    if (branch())
        connect(branch(), &Node::countChanged, this, &Node::setCount);

    updateCount();
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
    // TODO: how do we emit branchChanged() when the parent changes?
    return parent<Branch>();
}

Branch *ShotgunEntity::branch()
{
    return parent<Branch>();
}

QQmlListProperty<ShotgunField> ShotgunEntity::shotgunFields_()
{
    return QQmlListProperty<ShotgunField>(this, 0, ShotgunEntity::shotgunFields_append,
                                        ShotgunEntity::shotgunFields_count,
                                        ShotgunEntity::shotgunFields_at,
                                        ShotgunEntity::shotgunFields_clear);
}

// fields property
void ShotgunEntity::shotgunFields_append(QQmlListProperty<ShotgunField> *prop, ShotgunField *p)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);

    p->setParent(that);
    emit that->shotgunFieldsChanged();
}

int ShotgunEntity::shotgunFields_count(QQmlListProperty<ShotgunField> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<ShotgunField *>(o))
            count++;
    }
    return count;
}

ShotgunField *ShotgunEntity::shotgunFields_at(QQmlListProperty<ShotgunField> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        ShotgunField *p = qobject_cast<ShotgunField *>(o);
        if (p) {
            if (index == i)
                return p;
            index++;
        }
    }
    return 0;
}

void ShotgunEntity::shotgunFields_clear(QQmlListProperty<ShotgunField> *prop)
{
    ShotgunEntity *that = static_cast<ShotgunEntity *>(prop->object);
    const QObjectList fields = prop->object->children();
    foreach (QObject *o, fields) {
        ShotgunField *p = qobject_cast<ShotgunField *>(o);
        if (p)
            p->setParent(0);
    }
    emit that->shotgunFieldsChanged();
}


void ShotgunEntity::onShotgunPull(const QVariant context, Shotgun *shotgun)
{
    trace() << ".onShotgunPull(" << context << "," << shotgun << ")";
    Q_ASSERT(shotgun);

    if (branch()->count() == 1) {
        ShotgunReply *reply = shotgun->findOne(shotgunEntityName(), filters(), fields());
        debug() << "filters are" << filters();
        connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onReadFinished);
        connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                this, &ShotgunEntity::onShotgunReadError);
    } else if (branch()->count() > 0) {
        error() << "Shotgun pull does not support branches with multiple details (" << branch() << ")";
        emit shotgunPulled(OperationAttached::Error);
    } else {
        error() << "Folder" << branch() << "has no details";
        emit shotgunPulled(OperationAttached::Error);
    }
}

void ShotgunEntity::onShotgunPush(const QVariant context, Shotgun *shotgun)
{
    trace() << ".onShotgunPush(" << context << "," << shotgun << ")";
    Q_ASSERT(shotgun);

    if (branch()->count() == 1) {
        ShotgunReply *reply = shotgun->create(shotgunEntityName(), data()[0].toMap(), fields());
        connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onCreateFinished);
        connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                this, &ShotgunEntity::onShotgunCreateError);
    } else if (branch()->count() > 0) {
        QVariantList batchRequests;
        foreach (const QVariant d, data()) {
            QVariantMap batchRequest;

            batchRequest["request_type"] = "create";
            batchRequest["entity_type"] = shotgunEntityName();
            batchRequest["data"] = d;
            QStringList returnFields = fields();
            if (returnFields.length() > 0)
                batchRequest["return_fields"] = returnFields;

            batchRequests.append(batchRequest);
        }

        debug() << "requests:" << batchRequests;

        ShotgunReply *reply = shotgun->batch(batchRequests);
        connect(reply, &ShotgunReply::finished, this, &ShotgunEntity::onBatchCreateFinished);
        connect(reply, static_cast<ShotgunReply::ErrorFunc>(&ShotgunReply::error),
                this, &ShotgunEntity::onShotgunCreateError);
    } else {
        error() << "Folder" << branch() << "has no details";
        emit shotgunPushed(OperationAttached::Error);
    }
}

const QVariantList ShotgunEntity::data() const
{
    trace() << ".data()";
    Q_ASSERT(branch());

    QVariantList result;
    for (int i = 0; i < branch()->count(); i++) {
        QVariantMap d;

        // build data for this entity from the entity's fields
        foreach (const QObject *o, children()) {
            const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
            if (sgf) {
                // name: value pairs
                QJSValue jsvalue = sgf->details().property(0).property("value");
                QVariant value;
                if (jsvalue.isNull())
                    value = QVariant(QVariant::String); // null QVariant
                else if (jsvalue.isUndefined())
                    value = QVariant(); // invalid QVariant
                else
                    value = qjsvalue_cast<QVariant>(jsvalue);
                trace() << "value of" << sgf << "is" << value;
                Q_ASSERT(value.isValid());
                // skip null values
                if (!value.isNull()) {
                    d.insert(sgf->shotgunFieldName(), value);
                }
            }
        }

        result.append(d);
    }

    trace() << "    ->" << result;
    return result;
}

const QVariantList ShotgunEntity::filters() const
{
    trace() << ".filters()";
    Q_ASSERT(branch());
    Q_ASSERT(branch()->count() == 1);

    QVariantList result;

    // build filters for this entity from the entity's shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf && sgf->type() != ShotgunField::Url) {
            // we skip Url fields, since they can't be filtered against
            // ["<shotgun field name>", "is", <value for field name>]
            QJSValue jsvalue = sgf->details().property(0).property("value");
            QVariant value;
            if (jsvalue.isNull())
                value = QVariant(QVariant::String); // null QVariant
            else if (jsvalue.isUndefined())
                value = QVariant(); // invalid QVariant
            else
                value = qjsvalue_cast<QVariant>(jsvalue);
            trace() << "value of" << sgf << "is" << value;
            Q_ASSERT(value.isValid());
            // we also skip null fields
            if (!value.isNull()) {
                QVariantList filter;
                filter << sgf->shotgunFieldName() << "is" << value;
                result << QVariant(filter);
            }
        }
    }

    trace() << "    ->" << result;
    return result;
}


const QStringList ShotgunEntity::fields() const
{
    trace() << ".fields()";
    Q_ASSERT(branch());
    Q_ASSERT(branch()->count() == 1);

    QStringList result;

    // build a list of Shotgun fields
    foreach (const QObject *o, children()) {
        const ShotgunField *sgf = qobject_cast<const ShotgunField *>(o);
        if (sgf) {
            result << sgf->shotgunFieldName();
        }
    }

    trace() << "    ->" << result;
    return result;
}

void ShotgunEntity::onReadFinished(const QVariant result)
{
    trace() << ".onReadFinished(" << result << ")";
    debug() << "Shotgun.find returned" << result;

    setDetail(0, "entity", toScriptValue(result.toMap()));

    QObject::sender()->deleteLater();

    emit shotgunPulled(OperationAttached::Finished);
}

void ShotgunEntity::onCreateFinished(const QVariant result)
{
    trace() << ".onCreateFinished(" << result << ")";
    debug() << "Shotgun.create returned" << result;

    setDetail(0, "entity", toScriptValue(result.toMap()));

    QObject::sender()->deleteLater();

    emit shotgunPushed(OperationAttached::Finished);
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

    emit shotgunPushed(OperationAttached::Finished);
}

void ShotgunEntity::onShotgunReadError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    trace() << ".onShotgunReadError() [error is" << reply->errorString() << "]";
    debug() << "Shotgun.find failed  [error is" << reply->errorString() << "]";
    m_status = OperationAttached::Error;

    reply->deleteLater();

    emit shotgunPulled(OperationAttached::Error);
}

void ShotgunEntity::onShotgunCreateError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());
    Q_ASSERT(reply);

    trace() << ".onShotgunCreateError() [error is" << reply->errorString() << "]";
    debug() << "Shotgun.create or .batch failed  [error is" << reply->errorString() << "]";
    m_status = OperationAttached::Error;

    reply->deleteLater();

    emit shotgunPushed(OperationAttached::Error);
}

void ShotgunEntity::updateCount()
{
    if (branch()) {
        setCount(branch()->count());
    }
}
