#include <QDebug>
#include <QQmlProperty>

#include "nodebase.h"
#include "param.h"
#include "input.h"

NodeBase::NodeBase(QObject *parent) :
    PugItem(parent),
    m_outputFlag(false)
{
}

QQmlListProperty<NodeBase> NodeBase::nodes_()
{
    return QQmlListProperty<NodeBase>(this, 0, NodeBase::nodes_append,
                                        NodeBase::nodes_count,
                                        NodeBase::node_at,
                                        NodeBase::nodes_clear);
}

QQmlListProperty<Param> NodeBase::params_()
{
    return QQmlListProperty<Param>(this, 0, NodeBase::params_append,
                                        NodeBase::params_count,
                                        NodeBase::param_at,
                                        NodeBase::params_clear);
}

QQmlListProperty<Input> NodeBase::inputs_()
{
    return QQmlListProperty<Input>(this, 0, NodeBase::inputs_append,
                                        NodeBase::inputs_count,
                                        NodeBase::input_at,
                                        NodeBase::inputs_clear);
}

// nodes property
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

NodeBase *NodeBase::node_at(QQmlListProperty<NodeBase> *prop, int i)
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

// params property
void NodeBase::params_append(QQmlListProperty<Param> *prop, Param *p)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    p->setParent(that);
    that->m_params.append(p);
    emit that->paramsChanged();
}

int NodeBase::params_count(QQmlListProperty<Param> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    return that->m_params.count();
}

Param *NodeBase::param_at(QQmlListProperty<Param> *prop, int i)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    if (i < that->m_params.count())
        return that->m_params[i];
    else
        return 0;
}

void NodeBase::params_clear(QQmlListProperty<Param> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    foreach (Param *p, that->m_params) {
        p->setParent(0);
    }
    that->m_params.clear();
    emit that->paramsChanged();
}

// inputs property
void NodeBase::inputs_append(QQmlListProperty<Input> *prop, Input *in)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    in->setParent(that);
    that->m_inputs.append(in);
    emit that->inputsChanged();
}

int NodeBase::inputs_count(QQmlListProperty<Input> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    return that->m_inputs.count();
}

Input *NodeBase::input_at(QQmlListProperty<Input> *prop, int i)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    if (i < that->m_inputs.count())
        return that->m_inputs[i];
    else
        return 0;
}

void NodeBase::inputs_clear(QQmlListProperty<Input> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    foreach (Input *in, that->m_inputs) {
        in->setParent(0);
    }
    emit that->inputsChanged();
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

const QList<const NodeBase *> NodeBase::upstream() const
{
    copious() << ".upstream";
    QList<const NodeBase *> result;
    // loop through inputs and find upstream nodes
    foreach (const Input *in, m_inputs) {
        if (hasProperty(in->name().toUtf8())) {
            QVariant v = QObject::property(in->name().toUtf8());
            if (v.canConvert<NodeBase *>()) {
                const NodeBase *input = v.value<NodeBase*>();
                if (input) {
                    result.append(input);
                }
            }
        }
    }

    return result;
}

const QList<NodeBase *> NodeBase::upstream()
{
    copious() << ".upstream";
    QList<NodeBase *> result;
    // loop through inputs and find upstream nodes
    foreach (const Input *in, m_inputs) {
        if (hasProperty(in->name().toUtf8())) {
            QVariant v = QObject::property(in->name().toUtf8());
            if (v.canConvert<NodeBase *>()) {
                NodeBase *input = v.value<NodeBase*>();
                if (input) {
                    result.append(input);
                }
            }
        }
    }

    return result;
}

const QList<NodeBase *> NodeBase::downstream()
{
    QList<NodeBase *> result;

    if (parent<NodeBase>()) {
        // loop through siblings params and find inputs
        foreach (QObject *o, QObject::parent()->children()) {
            NodeBase *n = qobject_cast<NodeBase*>(o);
            if (n && n->upstream().contains(this))
                result.append(n);
        }
    }

    return result;
}

const QList<const NodeBase *> NodeBase::downstream() const
{
    QList<const NodeBase *> result;

    if (parent<NodeBase>()) {
        // loop through siblings params and find inputs
        foreach (const QObject *o, QObject::parent()->children()) {
            const NodeBase *n = qobject_cast<const NodeBase*>(o);
            if (n && n->upstream().contains(this))
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

void NodeBase::addInput(const QString name)
{
    Input *input = new Input(this);
    input->setName(name);
    m_inputs.append(input);
    emit inputsChanged();
}

void NodeBase::addParam(const QString name)
{
    Param *param = new Param(this);
    param->setName(name);
    m_params.append(param);
    emit paramsChanged();
}
