#include "operation.h"
#include "nodebase.h"

OperationAttached::OperationAttached(QObject *parent) :
    PugItem(parent),
    m_status()
{
}

void OperationAttached::reset()
{
    setProperty("status", OperationAttached::Idle);
}

void OperationAttached::execute(const QVariant env)
{
    NodeBase* node = qobject_cast<NodeBase *>(parentItem());

    if (!node) {
        qWarning() << "Can't run execute on " << this;
        return;
    }

    // stash the environment, we'll it again soon
    m_env = env;

    // TODO: check for cycles

    executeInputs(node, env);
    executeChildren(node, env);

    if (status() == OperationAttached::Idle) {
        OperationAttached::Status ds = dependantsStatus();
        if (ds == OperationAttached::Finished) {
            setStatus(OperationAttached::Running);
            run(env);
        } else if (ds == OperationAttached::Error) {
            // dependencies failed
            setProperty("status", OperationAttached::Error);
            emit finished();
        }
    }
}

void OperationAttached::executeInputs(NodeBase *node, QVariant env)
{
    int idx = -1;

    // execute inputs with attachment object and connect their finished signal to ourself
    foreach (NodeBase* input, node->inputs()) {
        OperationAttached *inputAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(&idx, input, metaObject(), false));
        if (inputAttached && inputAttached->status() == OperationAttached::Idle) {
            connect(inputAttached, &OperationAttached::finished, this, &OperationAttached::onDependantFinished);
            inputAttached->execute(env);
        } else if (!inputAttached) {
            // recurse on objects that do NOT have the attachment
            executeInputs(node, env);
        }
    }
}

void OperationAttached::executeChildren(NodeBase *node, QVariant env)
{
    int idx = -1;

    // cook children with "cook" flag and connect their cooked signal to ourself
    foreach (QObject* o, node->children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child) {
            OperationAttached *childAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(&idx, child, metaObject(), false));
            if (childAttached && childAttached->status() == OperationAttached::Idle) {
                connect(childAttached, &OperationAttached::finished, this, &OperationAttached::onDependantFinished);
                childAttached->execute(env);
            } else if (!childAttached) {
                // recurse on children that do NOT have the attachment
                executeChildren(child, env);
            }
        }
    }
}

void OperationAttached::onDependantFinished()
{
    // disconnect
    OperationAttached* dependantAttached = qobject_cast<OperationAttached*>(QObject::sender());
    if (dependantAttached) {
        QObject::disconnect(dependantAttached, &OperationAttached::finished, this, &OperationAttached::onDependantFinished);
        // continue cooking
        execute(m_env);
    } else {
        qDebug() << this << " HUGE WTF";
        // TODO: wtf error!
    }
}

bool OperationAttached::dependantsFinished() const
{
    int idx = -1;

    foreach (NodeBase* input, parent()->inputs()) {
        OperationAttached *inputAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(&idx, input, metaObject(), false));
        if (inputAttached && (inputAttached->status() == OperationAttached::Idle || inputAttached->status() == OperationAttached::Running)) {
            return false;
        } else {
            // TODO: follow non-attached inputs
        }
    }

    foreach (QObject* o, children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child) {
            OperationAttached *childAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(&idx, child, metaObject(), false));
            if (childAttached && (child->status() == OperationAttached::Raw || child->status() == OperationAttached::Cooking)) {
                return false;
            }
        } else {
            // TODO: recurse on non-attached children
        }
    }

    return true;
}

OperationAttached::Status OperationAttached::dependantsStatus() const
{
    OperationAttached::Status status = OperationAttached::Cooked;

    foreach (NodeBase* input, inputs()) {
        OperationAttached *inputAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(idx, input, metaObject(), false));
        if (inputAttached && inputAttached->status() > status)
            status = inputAttached->status();
    }

    foreach (QObject* o, children()) {
        NodeBase *child = qobject_cast<Node *>(o);
        if (child) {
            OperationAttached *childAttached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(idx, child, metaObject(), false));
            if (childAttached && childAttached->status() > status)
                status = childAttached->status();
        }
    }

    return status;
}

//QQmlListProperty<PugItem> OperationAttached::data()
//{
//    return QQmlListProperty<PugItem>(this, 0, OperationAttached::data_append,
//                                        OperationAttached::data_count,
//                                        OperationAttached::data_at,
//                                        OperationAttached::data_clear);
//}
//
//QQmlListProperty<PugItem> OperationAttached::items()
//{
//    return QQmlListProperty<PugItem>(this, 0, OperationAttached::items_append,
//                                        OperationAttached::items_count,
//                                        OperationAttached::items_at,
//                                        OperationAttached::items_clear);
//}
//
//OperationAttached::Status OperationAttached::status() const
//{
//    return m_status;
//}
//
//void OperationAttached::setStatus(OperationAttached::Status s)
//{
//    m_status = s;
//}
//
//void OperationAttached::reset()
//{
//    setProperty("status", OperationAttached::Idle);
//}
//
//// data property
//void OperationAttached::data_append(QQmlListProperty<PugItem> *prop, PugItem *item)
//{
//    if (!item)
//        return;
//
//    OperationAttached *that = static_cast<OperationAttached *>(prop->object);
//
//    item->setParentItem(that);
//}
//
//int OperationAttached::data_count(QQmlListProperty<PugItem> *prop)
//{
//    Q_UNUSED(prop);
//    return 0;
//}
//
//PugItem *OperationAttached::data_at(QQmlListProperty<PugItem> *prop, int i)
//{
//    Q_UNUSED(prop);
//    Q_UNUSED(i);
//    return 0;
//}
//
//void OperationAttached::data_clear(QQmlListProperty<PugItem> *prop)
//{
//    Q_UNUSED(prop)
//}
//
//// children property
//void OperationAttached::items_append(QQmlListProperty<PugItem> *prop, PugItem *node)
//{
//    if (!node)
//        return;
//
//    OperationAttached *that = static_cast<OperationAttached *>(prop->object);
//
//    node->setParentItem(that);
//}
//
//int OperationAttached::items_count(QQmlListProperty<PugItem> *prop)
//{
//    int count = 0;
//    foreach (QObject *o, prop->object->children()) {
//        if (qobject_cast<PugItem *>(o))
//            count++;
//    }
//    return count;
//}
//
//PugItem *OperationAttached::items_at(QQmlListProperty<PugItem> *prop, int i)
//{
//    int index = 0;
//    foreach (QObject *o, prop->object->children()) {
//        PugItem *node = qobject_cast<PugItem *>(o);
//        if (node) {
//            if (index == i)
//                return node;
//            index++;
//        }
//    }
//    return 0;
//}
//
//void OperationAttached::items_clear(QQmlListProperty<PugItem> *prop)
//{
//    const QObjectList children = prop->object->children();
//    foreach (QObject *o, children) {
//        PugItem *node = qobject_cast<PugItem *>(o);
//        if (node)
//            node->setParent(0);
//    }
//}

Operation::Operation(QObject *parent) :
    PugItem(parent)
{
}

QStringList Operation::runAfter() const
{
    return m_runAfter;
}

void Operation::setRunAfter(const QStringList l)
{
    m_runAfter = l;
}

QStringList Operation::dependsOn() const
{
    return m_dependsOn;
}

void Operation::setDependsOn(const QStringList l)
{
    m_dependsOn = l;
}

void Operation::execute(NodeBase* node, const QVariant env)
{
    int idx = -1; // unused
    OperationAttached *attached = qobject_cast<OperationAttached *>(qmlAttachedPropertiesObject(&idx, node, attachedPropertiesMetaObject(), false));
    attached->execute(env);
}
