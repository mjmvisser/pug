#include <QQmlProperty>
#include <QQmlListReference>

#include "node.h"
#include "param.h"
#include "input.h"
#include "output.h"

Node::Node(QObject *parent) :
    PugItem(parent),
    m_activeFlag(false),
    m_count(0),
    m_index(-1),
    m_x(0),
    m_y(0)
{
}

QQmlListProperty<Node> Node::nodes_()
{
    return QQmlListProperty<Node>(this, 0, Node::nodes_append,
                                        Node::nodes_count,
                                        Node::node_at,
                                        Node::nodes_clear);
}

QQmlListProperty<Param> Node::params_()
{
    return QQmlListProperty<Param>(this, 0, Node::params_append,
                                        Node::params_count,
                                        Node::param_at,
                                        Node::params_clear);
}

QQmlListProperty<Input> Node::inputs_()
{
    return QQmlListProperty<Input>(this, 0, Node::inputs_append,
                                        Node::inputs_count,
                                        Node::input_at,
                                        Node::inputs_clear);
}

QQmlListProperty<Output> Node::outputs_()
{
    return QQmlListProperty<Output>(this, 0, Node::outputs_append,
                                        Node::outputs_count,
                                        Node::output_at,
                                        Node::outputs_clear);
}

// nodes property
void Node::nodes_append(QQmlListProperty<Node> *prop, Node *node)
{
    Node *that = static_cast<Node *>(prop->object);

    node->setParent(that);
    emit that->nodesChanged();
}

int Node::nodes_count(QQmlListProperty<Node> *prop)
{
    int count = 0;
    foreach (QObject *o, prop->object->children()) {
        if (qobject_cast<Node *>(o))
            count++;
    }
    return count;
}

Node *Node::node_at(QQmlListProperty<Node> *prop, int i)
{
    int index = 0;
    foreach (QObject *o, prop->object->children()) {
        Node *node = qobject_cast<Node *>(o);
        if (node) {
            if (index == i)
                return node;
            index++;
        }
    }
    return 0;
}

void Node::nodes_clear(QQmlListProperty<Node> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    const QObjectList children = prop->object->children();
    foreach (QObject *o, children) {
        Node *node = qobject_cast<Node *>(o);
        if (node)
            node->setParent(0);
    }
    emit that->nodesChanged();
}

// params property
void Node::params_append(QQmlListProperty<Param> *prop, Param *p)
{
    Node *that = static_cast<Node *>(prop->object);

    p->setParent(that);
    that->m_params.append(p);
    emit that->paramsChanged();
}

int Node::params_count(QQmlListProperty<Param> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    return that->m_params.count();
}

Param *Node::param_at(QQmlListProperty<Param> *prop, int i)
{
    Node *that = static_cast<Node *>(prop->object);
    if (i < that->m_params.count())
        return that->m_params[i];
    else
        return 0;
}

void Node::params_clear(QQmlListProperty<Param> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    foreach (Param *p, that->m_params) {
        p->setParent(0);
    }
    that->m_params.clear();
    emit that->paramsChanged();
}

// inputs property
void Node::inputs_append(QQmlListProperty<Input> *prop, Input *in)
{
    Node *that = static_cast<Node *>(prop->object);

    in->setParent(that);
    that->m_inputs.append(in);
    emit that->inputsChanged();
}

int Node::inputs_count(QQmlListProperty<Input> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    return that->m_inputs.count();
}

Input *Node::input_at(QQmlListProperty<Input> *prop, int i)
{
    Node *that = static_cast<Node *>(prop->object);
    if (i < that->m_inputs.count())
        return that->m_inputs[i];
    else
        return 0;
}

void Node::inputs_clear(QQmlListProperty<Input> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    foreach (Input *in, that->m_inputs) {
        in->setParent(0);
    }
    emit that->inputsChanged();
}

// outputs property
void Node::outputs_append(QQmlListProperty<Output> *prop, Output *in)
{
    Node *that = static_cast<Node *>(prop->object);

    in->setParent(that);
    that->m_outputs.append(in);
    emit that->outputsChanged();
}

int Node::outputs_count(QQmlListProperty<Output> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    return that->m_outputs.count();
}

Output *Node::output_at(QQmlListProperty<Output> *prop, int i)
{
    Node *that = static_cast<Node *>(prop->object);
    if (i < that->m_outputs.count())
        return that->m_outputs[i];
    else
        return 0;
}

void Node::outputs_clear(QQmlListProperty<Output> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    foreach (Output *in, that->m_outputs) {
        in->setParent(0);
    }
    emit that->outputsChanged();
}

const QList<const Input*> Node::inputs() const
{
    return constList(m_inputs);
}

bool Node::isActive() const
{
    return m_activeFlag;
}

void Node::setActive(bool flag)
{
    if (m_activeFlag != flag) {
        m_activeFlag = flag;
        emit activeChanged(flag);
    }
}

QJSValue Node::details()
{
    return m_details;
}

const QJSValue Node::details() const
{
    return m_details;
}

void Node::setDetails(const QJSValue &details)
{
    if (!m_details.strictlyEquals(details)) {
        m_details = details;
        emit detailsChanged();
    }
}

Node *Node::child(int i)
{
    int index = 0;
    foreach (QObject *o, children()) {
        Node *child = qobject_cast<Node *>(o);
        if (child) {
            if (index == i)
                return child;
            index++;
        }
    }
    return 0;
}

int Node::childCount() const
{
    int count = 0;
    foreach (QObject *o, children()) {
        Node *child = qobject_cast<Node *>(o);
        if (child) {
            count++;
        }
    }
    return count;
}

Node *Node::firstNamedParent()
{
    Node *p = parent<Node>();
    while (p && p->name().isEmpty() && !p->isRoot()) {
        p = p->parent<Node>();
    }

    return p;
}

const Node *Node::firstNamedParent() const
{
    const Node *p = parent<Node>();
    while (p && p->name().isEmpty() && !p->isRoot()) {
        p = p->parent<Node>();
    }

    return p;
}

Node *Node::node(const QString n)
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
        Node *r = rootBranch();
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

const Node *Node::node(const QString n) const
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
        const Node *r = rootBranch();
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

const QString Node::path() const
{
    QString path = objectName();

    const Node *p = firstNamedParent();
    if (p) {
        path.prepend("/");
        path.prepend(p->path());
    }

    return path;
}

Node *Node::nodeInFirstNamedParent(const QString n)
{
    Node *p = firstNamedParent();
    if (p)
        return p->node(n);
    else
        return 0;
}

const Node *Node::nodeInFirstNamedParent(const QString n) const
{
    const Node *p = firstNamedParent();
    if (p)
        return p->node(n);
    else
        return 0;
}

Node *Node::nodeInChildren(const QString n)
{
    foreach (QObject *o, children()) {
        Node* node = qobject_cast<Node *>(o);
        if (node) {
            Node *found = node->node(n);
            if (found)
                return found;
        }
    }

    return 0;
}

const Node *Node::nodeInChildren(const QString n) const
{
    foreach (const QObject *o, children()) {
        const Node* node = qobject_cast<const Node *>(o);
        if (node) {
            const Node *found = node->node(n);
            if (found)
                return found;
        }
    }

    return 0;
}

const QList<const Node *> Node::upstream() const
{
    trace() << ".upstream()";
    QList<const Node *> result;
    // loop through inputs and find upstream nodes
    foreach (const Input *in, m_inputs) {
        const QVariant v = QQmlProperty::read(in->QObject::parent(), in->name());
        debug() << "input property" << in->name() << "is" << v;
        if (v.canConvert<Node *>()) {
            // single input
            const Node *input = v.value<Node*>();
            if (input) {
                result.append(input);
            }
        } else if (v.canConvert<QQmlListReference>()) {
            // multi-input
            QQmlListReference l = v.value<QQmlListReference>();
            for (int i = 0; i < l.count(); i++) {
                const Node *input = qobject_cast<const Node *>(l.at(i));
                if (input) {
                    result.append(input);
                }
            }
        } else if (v.isNull()) {
            // skip
        } else {
            error() << "can't interpret input" << in->name() << v;
        }
    }
    trace() << "    ->" << result;
    return result;
}

const QList<Node *> Node::upstream()
{
    return unConstList(static_cast<const Node &>(*this).upstream());
}

const QVariantList Node::upstreamNodes()
{
    trace() << ".upstreamNodes()";
    QVariantList result;
    foreach (Node *input, upstream()) {
        result.append(QVariant::fromValue(input));
    }

    trace() << "    ->" << result;
    return result;
}

const QList<const Node *> Node::upstream(const Input *in) const
{
    trace() << ".upstream(" << in << ")";
    QList<const Node *> result;
    const QVariant v = QQmlProperty::read(in->QObject::parent(), in->name());
    if (v.canConvert<Node *>()) {
        // single input
        const Node *input = v.value<Node *>();
        if (input) {
            result.append(input);
        }
    } else if (v.canConvert<QQmlListReference>()) {
        // multi-input
        QQmlListReference l = v.value<QQmlListReference>();
        for (int i = 0; i < l.count(); i++) {
            const Node *input = qobject_cast<const Node *>(l.at(i));
            if (input) {
                result.append(input);
            }
        }
    } else if (v.isNull()) {
        // skip
    } else {
        error() << "can't interpret input" << in->name() << v;
    }
    trace() << "    ->" << result;
    return result;
}

const QList<Node *> Node::upstream(const Input *in)
{
    return unConstList(static_cast<const Node &>(*this).upstream(in));
}

const QList<const Node *> Node::downstream() const
{
    trace() << ".downstream()";
    QList<const Node *> result;
    if (parent<Node>()) {
        // loop through siblings params and find inputs
        foreach (const QObject *o, QObject::parent()->children()) {
            const Node *n = qobject_cast<const Node*>(o);
            if (n && n->upstream().contains(this))
                result.append(n);
        }
    }

    trace() << "    ->" << result;
    return result;
}


const QList<Node *> Node::downstream()
{
    // ugly, but correct
    // http://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
    return unConstList(static_cast<const Node &>(*this).downstream());
}

const QVariantList Node::downstreamNodes()
{
    trace() << ".downstreamNodes()";
    QVariantList result;
    foreach (Node *output, downstream()) {
        result.append(QVariant::fromValue(output));
    }

    trace() << "    ->" << result;
    return result;
}

bool Node::isUpstream(const Node *other)
{
    foreach(const Node *node, upstream()) {
        if (node == other)
            return true;
    }

    return false;
}

bool Node::isDownstream(const Node *other)
{
    foreach(const Node *node, downstream()) {
        if (node == other)
            return true;
    }

    return false;
}

bool Node::isRoot() const
{
    return false;
}

int Node::count() const
{
    return m_count;
}

void Node::setCount(int count)
{
    if (m_count != count) {
        m_count = count;

        for (int i = 0; i < count; i++) {
            if (!details().property(i).isObject()) {
                setDetail(i, newObject(), false);
            }
        }

        emit countChanged(count);
        emit detailsChanged();
    }
}

int Node::index() const
{
    return m_index;
}

void Node::setIndex(int index)
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

const Node *Node::rootBranch() const
{
    const Node* p = this;
    while (p && !p->isRoot())
        p = p->parent<Node>();

    return p;
}

Node *Node::rootBranch()
{
    Node* p = this;
    while (p && !p->isRoot())
        p = p->parent<Node>();

    return p;
}

void Node::setDetail(int index, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    m_details.setProperty(index, value);
    if (emitChanged)
        emit detailsChanged();
    //trace() << ".setDetail(" << index << "," << value << "," << emitChanged << ")";
}

void Node::setDetail(int index, const QString arg1, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    QJSValue detail = m_details.property(index);
    if (detail.isUndefined()) {
        detail = newObject();
        setDetail(index, detail, false);
    }

    detail.setProperty(arg1, value);
    if (emitChanged)
        emit detailsChanged();
    //trace() << ".setDetail(" << index << "," << arg1 << "," << value << "," << emitChanged << ")";
}

void Node::setDetail(int index, const QString arg1, const QString arg2, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    QJSValue obj = m_details.property(index).property(arg1);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, obj, false);
    }

    obj.setProperty(arg2, value);
    if (emitChanged)
        emit detailsChanged();
    //trace() << ".setDetail(" << index << "," << arg1 << "," << arg2 << "," << value << "," << emitChanged << ")";
}

void Node::setDetail(int index, const QString arg1, const QString arg2, const QString arg3, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    QJSValue obj = m_details.property(index).property(arg1).property(arg2);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, arg2, obj, false);
    }

    obj.setProperty(arg3, value);
    if (emitChanged)
        emit detailsChanged();

    //trace() << ".setDetail(" << index << "," << arg1 << "," << arg2 << "," << arg3 << "," << value << "," << emitChanged << ")";
}

void Node::setDetail(int index, const QString arg1, const QString arg2, quint32 arg3, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    QJSValue arr = m_details.property(index).property(arg1).property(arg2);
    if (arr.isUndefined()) {
        arr = newArray();
        setDetail(index, arg1, arg2, arr, false);
    }

    arr.setProperty(arg3, value);

    if (emitChanged)
        emit detailsChanged();
    //trace() << ".setDetail(" << index << "," << arg1 << "," << arg2 << "," << arg3 << "," << value << "," << emitChanged << ")";
}

void Node::setDetail(int index, const QString arg1, const QString arg2, quint32 arg3, const QString arg4, QJSValue value, bool emitChanged)
{
    Q_ASSERT(m_details.isArray());
    QJSValue obj = m_details.property(index).property(arg1).property(arg2).property(arg3);
    if (obj.isUndefined()) {
        obj = newObject();
        setDetail(index, arg1, arg2, arg3, obj, false);
    }

    obj.setProperty(arg4, value);
    if (emitChanged)
        emit detailsChanged();
    //trace() << ".setDetail(" << index << "," << arg1 << "," << arg2 << "," << arg3 << "," << arg4 << "," << value << "," << emitChanged << ")";
}

void Node::clearDetails()
{
    setDetails(newArray());
}

const QVariantMap Node::context(int index) const
{
    return details()
            .property(index)
            .property("context").toVariant().toMap();
}

void Node::setContext(int index, const QVariantMap v, bool emitChanged)
{
    if (context(index) != v) {
        setDetail(index, "context", toScriptValue(v), emitChanged);
    }
}

Param *Node::addParam(QObject *parent, const QString name)
{
    Param *param = new Param(parent);
    param->setName(name);
    m_params.append(param);
    emit paramsChanged();
    return param;
}

Input *Node::addInput(QObject *parent, const QString name)
{
    Input *input = new Input(parent);
    input->setName(name);
    m_inputs.append(input);
    emit inputsChanged();
    return input;
}

Output *Node::addOutput(const QString name)
{
    Output *output = new Output(this);
    output->setName(name);
    m_outputs.append(output);
    emit outputsChanged();
    return output;
}

void Node::classBegin()
{
    trace() << "componentComplete()";
    m_details = newArray();
}

qreal Node::x() const
{
    return m_x;
}

void Node::setX(qreal x)
{
    if (m_x != x) {
        m_x = x;
        emit xChanged(x);
    }
}

qreal Node::y() const
{
    return m_y;
}

void Node::setY(qreal y)
{
    if (m_y != y) {
        m_y = y;
        emit yChanged(y);
    }
}
