#include <QQmlProperty>
#include <QQmlListReference>

#include "nodebase.h"
#include "param.h"
#include "input.h"
#include "output.h"

NodeBase::NodeBase(QObject *parent) :
    PugItem(parent),
    m_activeFlag(false),
    m_count(0),
    m_index(-1),
    m_x(0),
    m_y(0)
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

QQmlListProperty<Output> NodeBase::outputs_()
{
    return QQmlListProperty<Output>(this, 0, NodeBase::outputs_append,
                                        NodeBase::outputs_count,
                                        NodeBase::output_at,
                                        NodeBase::outputs_clear);
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

// outputs property
void NodeBase::outputs_append(QQmlListProperty<Output> *prop, Output *in)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);

    in->setParent(that);
    that->m_outputs.append(in);
    emit that->outputsChanged();
}

int NodeBase::outputs_count(QQmlListProperty<Output> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    return that->m_outputs.count();
}

Output *NodeBase::output_at(QQmlListProperty<Output> *prop, int i)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    if (i < that->m_outputs.count())
        return that->m_outputs[i];
    else
        return 0;
}

void NodeBase::outputs_clear(QQmlListProperty<Output> *prop)
{
    NodeBase *that = static_cast<NodeBase *>(prop->object);
    foreach (Output *in, that->m_outputs) {
        in->setParent(0);
    }
    emit that->outputsChanged();
}

bool NodeBase::isActive() const
{
    return m_activeFlag;
}

void NodeBase::setActive(bool flag)
{
    if (m_activeFlag != flag) {
        m_activeFlag = flag;
        emit activeChanged(flag);
    }
}

QJSValue NodeBase::details()
{
    return m_details;
}

const QJSValue NodeBase::details() const
{
    return m_details;
}

void NodeBase::setDetails(const QJSValue &details)
{
    if (!m_details.strictlyEquals(details)) {
        m_details = details;
        emit detailsChanged();
    }
}

int NodeBase::childIndex() const
{
    const NodeBase *p = parent<NodeBase>();
    if (p) {
        int index = 0;
        foreach (const QObject *o, p->children()) {
            const NodeBase *child = qobject_cast<const NodeBase *>(o);
            if (child) {
                if (child == this)
                    return index;
                index++;
            }
        }
    }

    return -1;
}

NodeBase *NodeBase::child(int i)
{
    int index = 0;
    foreach (QObject *o, children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child) {
            if (index == i)
                return child;
            index++;
        }
    }
    return 0;
}

int NodeBase::childCount() const
{
    int count = 0;
    foreach (QObject *o, children()) {
        NodeBase *child = qobject_cast<NodeBase *>(o);
        if (child) {
            count++;
        }
    }
    return count;
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

const QString NodeBase::path() const
{
    QString path = objectName();

    const NodeBase *p = firstNamedParent();
    if (p) {
        path.prepend("/");
        path.prepend(p->path());
    }

    return path;
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
    trace() << ".upstream()";
    QList<const NodeBase *> result;
    // loop through inputs and find upstream nodes
    foreach (const Input *in, m_inputs) {
        if (hasProperty(in->name().toUtf8())) {
            const QVariant v = QQmlProperty::read(const_cast<NodeBase *>(this), in->name());
            if (v.canConvert<NodeBase *>()) {
                // single input
                const NodeBase *input = v.value<NodeBase*>();
                if (input) {
                    result.append(input);
                }
            } else if (v.canConvert<QQmlListReference>()) {
                // multi-input
                QQmlListReference l = v.value<QQmlListReference>();
                for (int i = 0; i < l.count(); i++) {
                    const NodeBase *input = qobject_cast<const NodeBase *>(l.at(i));
                    if (input) {
                        result.append(input);
                    }
                }
            } else if (v.isNull()) {
                // skip
            } else {
                error() << "can't interpret input" << in->name() << v;
            }
        } else {
            error() << "can't find input" << in->name();
        }
    }
    trace() << "    ->" << result;
    return result;
}

const QList<NodeBase *> NodeBase::upstream()
{
    trace() << ".upstream()";
    QList<NodeBase *> result;
    // loop through inputs and find upstream nodes
    foreach (const Input *in, m_inputs) {
        if (hasProperty(in->name().toUtf8())) {
            QVariant v = QQmlProperty::read(const_cast<NodeBase *>(this), in->name());
            if (v.canConvert<NodeBase *>()) {
                // single input
                NodeBase *input = v.value<NodeBase*>();
                if (input) {
                    result.append(input);
                }
            } else if (v.canConvert<QQmlListReference>()) {
                // multi-input
                QQmlListReference l = v.value<QQmlListReference>();
                for (int i = 0; i < l.count(); i++) {
                    NodeBase *input = qobject_cast<NodeBase *>(l.at(i));
                    if (input) {
                        result.append(input);
                    }
                }
            } else if (v.isNull()) {
                // skip
            } else {
                error() << "can't interpret input" << in->name() << v;
            }
        } else {
            error() << "can't find input" << in->name();
        }
    }
    trace() << "    ->" << result;
    return result;
}

const QVariantList NodeBase::upstreamNodes()
{
    trace() << ".upstreamNodes()";
    QVariantList result;
    foreach (NodeBase *input, upstream()) {
        result.append(QVariant::fromValue(input));
    }

    trace() << "    ->" << result;
    return result;
}

const QList<NodeBase *> NodeBase::downstream()
{
    trace() << ".downstream()";
    QList<NodeBase *> result;
    if (parent<NodeBase>()) {
        // loop through siblings params and find inputs
        foreach (QObject *o, QObject::parent()->children()) {
            NodeBase *n = qobject_cast<NodeBase*>(o);
            if (n && n->upstream().contains(this))
                result.append(n);
        }
    }

    trace() << "    ->" << result;
    return result;
}

const QList<const NodeBase *> NodeBase::downstream() const
{
    trace() << ".downstream()";
    QList<const NodeBase *> result;
    if (parent<NodeBase>()) {
        // loop through siblings params and find inputs
        foreach (const QObject *o, QObject::parent()->children()) {
            const NodeBase *n = qobject_cast<const NodeBase*>(o);
            if (n && n->upstream().contains(this))
                result.append(n);
        }
    }

    trace() << "    ->" << result;
    return result;
}

const QVariantList NodeBase::downstreamNodes()
{
    trace() << ".downstreamNodes()";
    QVariantList result;
    foreach (NodeBase *output, downstream()) {
        result.append(QVariant::fromValue(output));
    }

    trace() << "    ->" << result;
    return result;
}

bool NodeBase::isUpstream(const NodeBase *other)
{
    foreach(const NodeBase *node, upstream()) {
        if (node == other)
            return true;
    }

    return false;
}

bool NodeBase::isDownstream(const NodeBase *other)
{
    foreach(const NodeBase *node, downstream()) {
        if (node == other)
            return true;
    }

    return false;
}

bool NodeBase::isRoot() const
{
    return false;
}

int NodeBase::count() const
{
    return m_count;
}

void NodeBase::setCount(int count)
{
    if (m_count != count) {
        m_count = count;
        emit countChanged(count);
    }
}

int NodeBase::index() const
{
    return m_index;
}

void NodeBase::setIndex(int index)
{
    if (m_index != index) {
        if (index >= m_count) {
            error() << "Attempting to set index to" << index << "but count is only" << m_count;
        } else {
            m_index = index;
            emit indexChanged(index);
        }
    }
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

QJSValue NodeBase::detail(int index, const QString arg1, const QString arg2,
        const QString arg3, const QString arg4, const QString arg5) const
{
    // access details[index]?.arg1?.arg2?.arg3?.arg4?.arg5? for each non-empty arg
    QJSValue result = m_details.property(index);
    if (!result.isUndefined() && !arg1.isEmpty()) {
        result = result.property(arg1);
        if (!result.isUndefined() && !arg2.isEmpty()) {
            result = result.property(arg2);
            if (!result.isUndefined() && !arg3.isEmpty()) {
                result = result.property(arg3);
                if (!result.isUndefined() && !arg4.isEmpty()) {
                    result = result.property(arg4);
                    if (!result.isUndefined() && !arg5.isEmpty()) {
                        result = result.property(arg5);
                    }
                }
            }
        }
    }

    return result;
}

void NodeBase::setDetail(int index, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    details().setProperty(index, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::setDetail(int index, const QString arg1, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    QJSValue detail = details().property(index);
    if (detail.isUndefined()) {
        detail = newObject();
        setDetail(index, detail, false);
    }

    detail.setProperty(arg1, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::setDetail(int index, const QString arg1, const QString arg2, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    QJSValue obj = details().property(index).property(arg1);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, obj, false);
    }

    obj.setProperty(arg2, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::setDetail(int index, const QString arg1, const QString arg2, const QString arg3, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    QJSValue obj = details().property(index).property(arg1).property(arg2);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, arg2, obj, false);
    }

    obj.setProperty(arg3, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::setDetail(int index, const QString arg1, const QString arg2, const QString arg3, const QString arg4, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    QJSValue obj = details().property(index).property(arg1).property(arg2).property(arg3);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, arg2, arg3, obj, false);
    }

    obj.setProperty(arg4, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::setDetail(int index, const QString arg1, const QString arg2, const QString arg3, const QString arg4, const QString arg5, QJSValue value, bool emitChanged)
{
    Q_ASSERT(details().isArray());
    QJSValue obj = details().property(index).property(arg1).property(arg2).property(arg3).property(arg4);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, arg2, arg3, arg4, obj, false);
    }

    obj.setProperty(arg5, value);
    if (emitChanged)
        emit detailsChanged();
}

void NodeBase::addParam(const QString name)
{
    Param *param = new Param(this);
    param->setName(name);
    m_params.append(param);
    emit paramsChanged();
}

void NodeBase::addInput(const QString name)
{
    Input *input = new Input(this);
    input->setName(name);
    m_inputs.append(input);
    emit inputsChanged();
}

void NodeBase::addOutput(const QString name)
{
    Output *output = new Output(this);
    output->setName(name);
    m_outputs.append(output);
    emit outputsChanged();
}

void NodeBase::componentComplete()
{
    trace() << "componentComplete()";
    m_details = newArray();
}

qreal NodeBase::x() const
{
    return m_x;
}

void NodeBase::setX(qreal x)
{
    if (m_x != x) {
        m_x = x;
        emit xChanged(x);
    }
}

qreal NodeBase::y() const
{
    return m_y;
}

void NodeBase::setY(qreal y)
{
    if (m_y != y) {
        m_y = y;
        emit yChanged(y);
    }
}
