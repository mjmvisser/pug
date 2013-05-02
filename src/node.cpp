#include <QQmlProperty>
#include <QQmlListReference>

#include "node.h"
#include "param.h"
#include "input.h"
#include "output.h"

Node::Node(QObject *parent) :
    PugItem(parent),
    m_dependencyOrder(Node::InputsChildrenSelf),
    m_count(0),
    m_index(-1),
    m_x(0),
    m_y(0)
{
}

Node::DependencyOrder Node::dependencyOrder() const
{
    return m_dependencyOrder;
}

void Node::setDependencyOrder(Node::DependencyOrder depOrder)
{
    if (m_dependencyOrder != depOrder) {
        m_dependencyOrder = depOrder;
        emit dependencyOrderChanged(depOrder);
    }
}

QQmlListProperty<Node> Node::nodes_()
{
    return QQmlListProperty<Node>(this, 0, Node::nodes_append,
                                        Node::nodes_count,
                                        Node::node_at,
                                        Node::nodes_clear);
}

QQmlListProperty<Field> Node::fields_()
{
    return QQmlListProperty<Field>(this, 0, Node::fields_append,
                                        Node::fields_count,
                                        Node::field_at,
                                        Node::fields_clear);
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

// fields property
void Node::fields_append(QQmlListProperty<Field> *prop, Field *f)
{
    Node *that = static_cast<Node *>(prop->object);

    f->setParent(that);
    that->m_fields.append(f);
    emit that->fieldsChanged();
}

int Node::fields_count(QQmlListProperty<Field> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    return that->m_fields.count();
}

Field *Node::field_at(QQmlListProperty<Field> *prop, int i)
{
    Node *that = static_cast<Node *>(prop->object);
    if (i < that->m_fields.count())
        return that->m_fields[i];
    else
        return 0;
}

void Node::fields_clear(QQmlListProperty<Field> *prop)
{
    Node *that = static_cast<Node *>(prop->object);
    foreach (Field *p, that->m_fields) {
        p->setParent(0);
    }
    that->m_fields.clear();
    emit that->fieldsChanged();
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

const QList<const Input*> Node::inputs() const
{
    return constList(m_inputs);
}

const QList<Input *> Node::inputs()
{
    return m_inputs;
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
    foreach (Output *out, that->m_outputs) {
        out->setParent(0);
    }
    emit that->outputsChanged();
}

const QList<const Output*> Node::outputs() const
{
    return constList(m_outputs);
}

const QList<Output*> Node::outputs()
{
    return m_outputs;
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

int Node::childIndex() const
{
    const Node *p = parent<Node>();
    if (p) {
        int index = 0;
        foreach (const QObject *o, p->children()) {
            const Node *child = qobject_cast<const Node *>(o);
            if (child) {
                if (child == this)
                    return index;
                index++;
            }
        }
    }

    return -1;
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
        return parent<Node>();
    } else if (n == "/") {
        return rootBranch();
    } else if (n[0] == '/') {
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
            return parent<Node>() ? parent<Node>()->nodeInChildren(remainder) : 0;
        } else if (remainder.isEmpty()) {
            return nodeInChildren(root);
        }
    }

    return 0;
}

Node *Node::node(const QString n)
{
    Node *result = const_cast<Node *>(static_cast<const Node &>(*this).node(n));
    return result;
}

const Node *Node::nodeInChildren(const QString n) const
{
    foreach (const QObject *o, children()) {
        const Node* node = qobject_cast<const Node *>(o);
        if (node) {
            const Node *found = node->node(n);
            if (found) {
                return found;
            }
        }
    }

    return 0;
}

//const QList<const Node *> Node::upstream() const
//{
//    //trace() << ".upstream()";
//    QList<const Node *> result;
//    // loop through inputs and find upstream nodes
//    foreach (const Input *in, m_inputs) {
//        result << upstream(in);
//    }
//    //trace() << "    ->" << result;
//    return result;
//}
//
//const QList<Node *> Node::upstream()
//{
//    return unConstList(static_cast<const Node &>(*this).upstream());
//}
//
//const QVariantList Node::upstreamNodes()
//{
//    trace() << ".upstreamNodes()";
//    QVariantList result;
//    foreach (Node *input, upstream()) {
//        result.append(QVariant::fromValue(input));
//    }
//
//    trace() << "    ->" << result;
//    return result;
//}
//
//const QList<const Node *> Node::upstream(const Input *in) const
//{
//    //trace() << ".upstream(" << in << ")";
//    QList<const Node *> result;
//    const QVariant v = QQmlProperty::read(in->QObject::parent(), in->name());
//    if (v.canConvert<Node *>()) {
//        // single input
//        const Node *input = v.value<Node *>();
//        if (input && !result.contains(input)) {
//            result.append(input);
//        }
//    } else if (v.canConvert<QQmlListReference>()) {
//        // multi-input
//        QQmlListReference l = v.value<QQmlListReference>();
//        for (int i = 0; i < l.count(); i++) {
//            const Node *input = qobject_cast<const Node *>(l.at(i));
//            if (input && !result.contains(input)) {
//                result.append(input);
//            }
//        }
//    } else if (v.isNull()) {
//        // skip
//    } else {
//        error() << "can't interpret input" << in->name() << v;
//    }
//    //trace() << "    ->" << result;
//    return result;
//}
//
//const QList<Node *> Node::upstream(const Input *in)
//{
//    return unConstList(static_cast<const Node &>(*this).upstream(in));
//}
//
//const QList<const Node *> Node::downstream() const
//{
//    trace() << ".downstream()";
//    QList<const Node *> result;
//    if (parent<Node>()) {
//        // loop through siblings params and find inputs
//        foreach (const QObject *o, QObject::parent()->children()) {
//            const Node *n = qobject_cast<const Node*>(o);
//            if (n && n->upstream().contains(this))
//                result.append(n);
//        }
//    }
//
//    trace() << "    ->" << result;
//    return result;
//}
//
//
//const QList<Node *> Node::downstream()
//{
//    // ugly, but correct
//    // http://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
//    return unConstList(static_cast<const Node &>(*this).downstream());
//}
//
//const QVariantList Node::downstreamNodes()
//{
//    trace() << ".downstreamNodes()";
//    QVariantList result;
//    foreach (Node *n, downstream()) {
//        result.append(QVariant::fromValue(n));
//    }
//
//    trace() << "    ->" << result;
//    return result;
//}
//
//bool Node::isUpstream(const Node *other)
//{
//    foreach(const Node *node, upstream()) {
//        if (node == other)
//            return true;
//    }
//
//    return false;
//}
//
//bool Node::isDownstream(const Node *other)
//{
//    foreach(const Node *node, downstream()) {
//        if (node == other)
//            return true;
//    }
//
//    return false;
//}

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
        m_index = index;
        if (m_index >= 0 && m_index < m_count)
            emit indexChanged(index);
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

void Node::setContext(int index, const QVariantMap v, bool emitChanged)
{
    if (details().property(index).property("context").toVariant().toMap() != v) {
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

const QVariantMap Node::mergeContexts(const QVariantMap first, const QVariantMap second)
{
    QVariantMap result = first;
    QMapIterator<QString, QVariant> i(second);
    while (i.hasNext()) {
        i.next();
        result.insert(i.key(), i.value());
    }
    return result;
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

int Node::receivers(const char *signal) const
{
    return QObject::receivers(signal);
}

#include "operation.h"
void Node::printStatus(const QString indent) const
{
    qDebug() << indent.toUtf8().constData() << this;
    foreach (QObject *obj, QObject::children()) {
        OperationAttached *attached = qobject_cast<OperationAttached *>(obj);
        if (attached && attached->status() > 0) {
            qDebug() << (indent + "  ").toUtf8().constData() << attached->objectName() << "->" << attached->status();
        }
    }

    foreach (QObject *obj, QObject::children()) {
        Node *node = qobject_cast<Node *>(obj);
        if (node) {
            node->printStatus(indent + "    ");
        }
    }
}

bool Node::cycleCheck(const QList<const Node *>& visited) const
{
    QList<const Node *> myVisited = visited;
    myVisited.append(this);

    if (visited.contains(this)) {
        error() << "found cycle:" << visited;
        return true;
    }

    foreach (const Input *input, inputs()) {
        foreach (const Node *n, input->nodes()) {
            if (n->cycleCheck(myVisited))
                return true;
        }
    }

    return false;
}

const Field *Node::findField(const QString name) const
{
    // check our fields first
    foreach (const Field *field, m_fields) {
        if (field->name() == name) {
            return field;
        }
    }

    // check our parent input
    const Node *p = parent<Node>();
    if (p) {
        return p->findField(name);
    }

    return 0;
}

Field *Node::findField(const QString name)
{
    return const_cast<Field *>(static_cast<const Node &>(*this).findField(name));
}

//const QList<const Field *> Node::fields(const QStringList fieldNameList) const
//{
//    QList<const Field *> results;
//
//    // add our list of field names to those passed in
//    QStringList fnames = fieldNameList;
//    fnames.append(fieldNames(pattern()));
//
//    // parent fields
//    const Node *s = parent<Node>();
//    if (s) {
//        results.append(s->fields(fnames));
//    }
//
//    // our fields
//    foreach (const Field *f, m_fields) {
//        if (fnames.contains(f->name()))
//            results.append(f);
//    }
//
//    return results;
//}
//

static const QString escapeSpecialCharacters(const QString r)
{
    QString result = r;
    result.replace('.', "\\.");

    return result;
}

const QVariant Node::match(const QString pattern, const QString path, bool exact, bool partial) const
{
    trace() << ".match(" << pattern << "," << path << "," << exact << ")";
    static const QRegularExpression replaceFieldsRegexp("\\{(\\w+)\\}");

    const QString escapedPattern = escapeSpecialCharacters(pattern);
    QRegularExpressionMatchIterator it = replaceFieldsRegexp.globalMatch(escapedPattern);

    // adapted from QString::replace code

    int lastEnd = 0;
    // assemble a regular expression string
    QString matchRegexpStr = "^";
    QSet<const QString> usedFieldNames;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        // add the part before the match
        int len = match.capturedStart() - lastEnd;
        if (len > 0) {
            matchRegexpStr += escapedPattern.mid(lastEnd, len);
        }

        // add "(" field.regexp ")"
        QString fieldName = match.captured(1);
        const Field *f = findField(fieldName);
        if (f) {
            if (!usedFieldNames.contains(fieldName)) {
                matchRegexpStr += "(?<";
                matchRegexpStr += fieldName;
                matchRegexpStr += ">";
                matchRegexpStr += f->regexp();
                matchRegexpStr += ")";
                usedFieldNames.insert(fieldName);
            } else {
                // already used, so just reference the previous match
                matchRegexpStr += "(?P=";
                matchRegexpStr += fieldName;
                matchRegexpStr += ")";
            }
        } else {
            warning() << ".match couldn't find field " << fieldName;
        }

        lastEnd = match.capturedEnd();
    }

    // trailing string after the last match
    if (escapedPattern.length() > lastEnd) {
        matchRegexpStr += escapedPattern.mid(lastEnd);
    }

    // replace %04d with \d{4}
    static const QRegularExpression replaceFrameSpecRegexp("%0(\\d+)d");
    matchRegexpStr.replace(replaceFrameSpecRegexp, "\\d{\\1}");

    // replace #### with \d{4}
    matchRegexpStr.replace("####", "\\d{4}");

    if (exact)
        matchRegexpStr += "$";

    debug() << ".match" << path << "with" << matchRegexpStr;

    QRegularExpression matchRegexp(matchRegexpStr);

    Q_ASSERT_X(matchRegexp.isValid(), QString("regular expression: " + matchRegexpStr).toUtf8(), matchRegexp.errorString().toUtf8());

    QRegularExpressionMatch match = matchRegexp.match(path, 0,
            partial ? QRegularExpression::PartialPreferCompleteMatch : QRegularExpression::NormalMatch);

    debug() << "result:" << match;

    if (match.hasMatch() || match.hasPartialMatch()) {
        QVariantMap context;

        foreach (QString fieldName, Field::fieldNames(pattern)) {
            const Field *f = findField(fieldName);
            if (f && !match.captured(fieldName).isNull()) {
                context[fieldName] = f->parse(match.captured(fieldName));
            }
        }

        QString remainder;
        if (match.capturedEnd() >= 0) {
            remainder = path.mid(match.capturedEnd());
        } else {
            // annoying inconsistency (bug?) -- if no capture groups, there is no implicit group 0
            remainder = path.mid(pattern.length());
        }

        if (remainder.length() > 0) {
            if (!exact)
                context["_"] = remainder;
            else
                return QVariant();
        }

        debug() << "result:" << context;

        return context;
    }

    return QVariant();
}

const QString Node::format(const QString pattern, const QVariant data) const
{
    //trace() << ".format(" << pattern << ", " << data << ")";
    QVariantMap fields;
    if (data.isValid() && data.type() == QVariant::Map) {
        fields = data.toMap();
    } else {
        error() << "invalid context data" << data;
        return QString();
    }

    static QRegularExpression re("\\{(\\w+)\\}");

    QString result;
    QRegularExpressionMatchIterator it = re.globalMatch(pattern);
    int lastEnd = 0;
    int index = 0;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        // add the part before the match
        int len = match.capturedStart() - lastEnd;
        if (len > 0) {
            result += pattern.mid(lastEnd, len);
        }

        QString fieldName = match.captured(1);
        const Field *f = findField(fieldName);
        if (f) {
            QVariant value = f->get(fields);
            if (value.isValid()) {
                result += f->format(value);
            } else {
                error() << ".format couldn't find value for field" << fieldName;
                return QString();
            }
        } else {
            error() << ".format couldn't find field" << fieldName;
            return QString();
        }

        lastEnd = match.capturedEnd();
        index++;
    }

    // trailing string after the last match
    if (pattern.length() > lastEnd) {
        result += pattern.mid(lastEnd);
    }

    return result;
}

bool Node::fieldsComplete(const QString pattern, const QVariantMap context) const
{
    // return true if all fields in pattern are represented and have a valid value in the context
    foreach (const QString fieldName, Field::fieldNames(pattern)) {
        const Field *field = findField(fieldName);
        if (!field) {
            debug() << "can't find field" << fieldName;
            return false;
        }

        QVariant value = field->get(context);
        if (!value.isValid()) {
            debug() << "field is not valid" << fieldName;
            return false;
        }
    }

    return true;
}
