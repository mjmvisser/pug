#include <QDebug>
#include <QQmlProperty>

#include "nodebase.h"
#include "property.h"

NodeBase::NodeBase(QObject *parent) :
    PugItem(parent),
    m_outputFlag(false)
{
}

QQmlListProperty<NodeBase> NodeBase::nodes_()
{
    return QQmlListProperty<NodeBase>(this, 0, NodeBase::nodes_append,
                                        NodeBase::nodes_count,
                                        NodeBase::nodes_at,
                                        NodeBase::nodes_clear);
}

QQmlListProperty<PropertyBase> NodeBase::properties_()
{
    return QQmlListProperty<PropertyBase>(this, 0, NodeBase::properties_append,
                                        NodeBase::properties_count,
                                        NodeBase::properties_at,
                                        NodeBase::properties_clear);
}

QQmlListProperty<NodeBase> NodeBase::inputs_()
{
    return QQmlListProperty<NodeBase>(this, 0, NodeBase::inputs_append,
                                        NodeBase::inputs_count,
                                        NodeBase::inputs_at,
                                        NodeBase::inputs_clear);
}

QQmlListProperty<Operation> NodeBase::operations_()
{
    return QQmlListProperty<Operation>(this, 0, NodeBase::operations_append,
                                        NodeBase::operations_count,
                                        NodeBase::operations_at,
                                        NodeBase::operations_clear);
}

// children property
void NodeBase::nodes_append(QQmlListProperty<NodeBase> *prop, NodeBase *node)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    node->setParent(that);
    emit that->nodesChanged();
}

int NodeBase::nodes_count(QQmlListProperty<NodeBase> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<NodeBase *>(o))
            count++;
    }
    return count;
}

NodeBase *NodeBase::nodes_at(QQmlListProperty<NodeBase> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        NodeBase *node = qobject_cast<NodeBase *>(o);
        if (node) {
            if (index == i)
                return node;
            index++;
        }
    }
    return 0;
}

void NodeBase::nodes_clear(QQmlListProperty<NodeBase> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    const QObjectList children = prop->object->children();
    foreach (QObject *o, children) {
        NodeBase *node = qobject_cast<NodeBase *>(o);
        if (node)
            node->setParent(0);
    }
    emit that->nodesChanged();
}

// properties property
void NodeBase::properties_append(QQmlListProperty<PropertyBase> *prop, PropertyBase *p)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    p->setParent(that);
    emit that->propertiesChanged();
}

int NodeBase::properties_count(QQmlListProperty<PropertyBase> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<PropertyBase *>(o))
            count++;
    }
    return count;
}

PropertyBase *NodeBase::properties_at(QQmlListProperty<PropertyBase> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        PropertyBase *p = qobject_cast<PropertyBase *>(o);
        if (p) {
            if (index == i)
                return p;
            index++;
        }
    }
    return 0;
}

void NodeBase::properties_clear(QQmlListProperty<PropertyBase> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    const QObjectList properties = prop->object->children();
    foreach (QObject *o, properties) {
        PropertyBase *p = qobject_cast<PropertyBase *>(o);
        if (p)
            p->setParent(0);
    }
    emit that->propertiesChanged();
}

// inputs property
void NodeBase::inputs_append(QQmlListProperty<NodeBase> *prop, NodeBase *p)
{
    Q_UNUSED(prop);
    Q_UNUSED(p);
}

int NodeBase::inputs_count(QQmlListProperty<NodeBase> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        Property *p = qobject_cast<Property *>(o);
        if (p && p->isInput()) {
            count++;
        }
    }
    return count;
}

NodeBase *NodeBase::inputs_at(QQmlListProperty<NodeBase> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        Property *p = qobject_cast<Property *>(o);
        if (p && p->isInput()) {
            if (index == i)
                return prop->object->property(p->name().toUtf8()).value<NodeBase *>();
            index++;
        }
    }
    return 0;
}

void NodeBase::inputs_clear(QQmlListProperty<NodeBase> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    const QObjectList inputs = prop->object->children();
    foreach (QObject *o, inputs) {
        Property *p = qobject_cast<Property *>(o);
        if (p && p->isInput())
            p->setParent(0);
    }
    emit that->inputsChanged();
}

// operations property
void NodeBase::operations_append(QQmlListProperty<Operation> *prop, Operation *p)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    p->setParent(that);
    emit that->operationsChanged();
}

int NodeBase::operations_count(QQmlListProperty<Operation> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<Operation *>(o))
            count++;
    }
    return count;
}

Operation *NodeBase::operations_at(QQmlListProperty<Operation> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        Operation *p = qobject_cast<Operation *>(o);
        if (p) {
            if (index == i)
                return p;
            index++;
        }
    }
    return 0;
}

void NodeBase::operations_clear(QQmlListProperty<Operation> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    const QObjectList operations = prop->object->children();
    foreach (QObject *o, operations) {
        Operation *node = qobject_cast<Operation *>(o);
        if (node)
            node->setParent(0);
    }
    emit that->operationsChanged();
}

bool NodeBase::isOutput() const
{
    return m_outputFlag;
}

void NodeBase::setOutput(bool o)
{
    if (m_outputFlag != o) {
        m_outputFlag = o;
        emit outputChanged(o);
    }
}


NodeBase *NodeBase::firstNamedParent()
{
    NodeBase *p = parent<NodeBase>();
    while (p && p->name().isEmpty() && !p->isRoot()) {
        p = p->parent<NodeBase>();
    }

    return p;
}

const NodeBase *NodeBase::firstNamedParent() const
{
    const NodeBase *p = parent<NodeBase>();
    while (p && p->name().isEmpty() && !p->isRoot()) {
        p = p->parent<NodeBase>();
    }

    return p;
}

NodeBase *NodeBase::node(const QString n)
{
    copious() << ".node" << n;

    if (n.isNull())
        return 0;

    if (n.isEmpty())
        return this;

    if (name() == n || n == ".") {
        // found it!
        return this;
    } else if (n == "..") {
        // found it!
        return firstNamedParent();
    } else if (n.at(0) == '/') {
        NodeBase *r = rootBranch();
        return r ? r->nodeInChildren(n.mid(1)) : 0;
    } else {
        QStringList parts = n.split('/', QString::SkipEmptyParts);
        QString root = parts.takeFirst();
        QString remainder = parts.join('/');

        if (name() == root || root == ".") {
            if (remainder.isEmpty())
                return this;
            else
                return nodeInChildren(remainder);
        } else if (root == "..") {
            return nodeInFirstNamedParent(remainder);
        } else if (remainder.isEmpty()) {
            return nodeInChildren(root);
        } else {
            return nodeInChildren(remainder);
        }
    }

    return 0;
}

const NodeBase *NodeBase::node(const QString n) const
{
    copious() << ".node" << n;

    if (n.isNull())
        return 0;

    if (n.isEmpty())
        return this;

    if (name() == n || n == ".") {
        // found it!
        return this;
    } else if (n == "..") {
        // found it!
        return firstNamedParent();
    } else if (n.at(0) == '/') {
        const NodeBase *r = rootBranch();
        return r ? r->nodeInChildren(n.mid(1)) : 0;
    } else {
        QStringList parts = n.split('/', QString::SkipEmptyParts);
        QString root = parts.takeFirst();
        QString remainder = parts.join('/');

        if (name() == root || root == ".") {
            if (remainder.isEmpty())
                return this;
            else
                return nodeInChildren(remainder);
        } else if (root == "..") {
            return nodeInFirstNamedParent(remainder);
        } else if (remainder.isEmpty()) {
            return nodeInChildren(root);
        } else {
            return nodeInChildren(remainder);
        }
    }

    return 0;
}

NodeBase *NodeBase::nodeInFirstNamedParent(const QString n)
{
    NodeBase *p = firstNamedParent();
    if (p)
        return p->node(n);
    else
        return 0;
}

const NodeBase *NodeBase::nodeInFirstNamedParent(const QString n) const
{
    const NodeBase *p = firstNamedParent();
    if (p)
        return p->node(n);
    else
        return 0;
}

NodeBase *NodeBase::nodeInChildren(const QString n)
{
    copious() << ".nodeInChildren" << n;
    foreach (QObject *o, children()) {
        NodeBase* node = qobject_cast<NodeBase *>(o);
        if (node) {
            NodeBase *found = node->node(n);
            if (found)
                return found;
        }
    }

    return 0;
}

const NodeBase *NodeBase::nodeInChildren(const QString n) const
{
    copious() << ".nodeInChildren" << n;
    foreach (const QObject *o, children()) {
        const NodeBase* node = qobject_cast<const NodeBase *>(o);
        if (node) {
            const NodeBase *found = node->node(n);
            if (found)
                return found;
        }
    }

    return 0;
}

const QList<NodeBase *> NodeBase::inputs()
{
    copious() << ".inputs";
    QList<NodeBase *> result;
    // loop through properties and find inputs
    foreach (QObject *o, children()) {
        Property *p = qobject_cast<Property*>(o);
        if (p && p->isInput()) {
            NodeBase *input = QObject::property(p->name().toUtf8()).value<NodeBase*>();
            if (input) {
                result.append(input);
            }
        }
    }

    return result;
}

const QList<const NodeBase *> NodeBase::inputs() const
{
    copious() << ".inputs";
    QList<const NodeBase *> result;
    // loop through properties and find inputs
    foreach (const QObject *o, children()) {
        const Property *p = qobject_cast<const Property*>(o);
        if (p && p->isInput()) {
            const NodeBase *input = QObject::property(p->name().toUtf8()).value<NodeBase*>();
            if (input) {
                result.append(input);
            }
        }
    }

    return result;
}

const QList<NodeBase *> NodeBase::downstream()
{
    QList<NodeBase *> result;

    if (parent<NodeBase>()) {
        // loop through siblings properties and find inputs
        foreach (QObject *o, QObject::parent()->children()) {
            NodeBase *n = qobject_cast<NodeBase*>(o);
            if (n && n->inputs().contains(this))
                result.append(n);
        }
    }

    return result;
}

const QList<const NodeBase *> NodeBase::downstream() const
{
    QList<const NodeBase *> result;

    if (parent<NodeBase>()) {
        // loop through siblings properties and find inputs
        foreach (const QObject *o, QObject::parent()->children()) {
            const NodeBase *n = qobject_cast<const NodeBase*>(o);
            if (n && n->inputs().contains(this))
                result.append(n);
        }
    }

    return result;
}

bool NodeBase::isRoot() const
{
    return false;
}

const NodeBase *NodeBase::rootBranch() const
{
    const NodeBase* p = this;
    while (p && !p->isRoot())
        p = p->parent<NodeBase>();

    return p;
}

NodeBase *NodeBase::rootBranch()
{
    NodeBase* p = this;
    while (p && !p->isRoot())
        p = p->parent<NodeBase>();

    return p;
}
