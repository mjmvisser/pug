#include "input.h"
#include "node.h"

Input::Input(QObject *parent) :
    PugItem(parent),
    m_dependency(Input::Invalid)
{
}

Input::Dependency Input::dependency() const
{
    return m_dependency;
}

void Input::setDependency(Input::Dependency d)
{
    if (m_dependency != d) {
        m_dependency = d;
        emit dependencyChanged(d);
    }
}

QQmlListProperty<Node> Input::nodes_()
{
    return QQmlListProperty<Node>(this, 0,
                                  Input::nodes_count,
                                  Input::node_at);
}

// nodes property
int Input::nodes_count(QQmlListProperty<Node> *prop)
{
    Input *that = static_cast<Input *>(prop->object);
    return that->nodes().count();
}

Node *Input::node_at(QQmlListProperty<Node> *prop, int i)
{
    Input *that = static_cast<Input *>(prop->object);
    if (i < that->nodes().count())
        return that->nodes().at(i);
    else
        return 0;
}

const QList<const Node *> Input::nodes() const
{
    //trace() << ".nodes()";
    QList<const Node *> result;
    const QVariant v = QQmlProperty::read(QObject::parent(), name());
    if (v.canConvert<Node *>()) {
        // single input
        const Node *input = v.value<Node *>();
        if (input && !result.contains(input)) {
            result.append(input);
        }
    } else if (v.canConvert<QQmlListReference>()) {
        // multi-input
        QQmlListReference l = v.value<QQmlListReference>();
        for (int i = 0; i < l.count(); i++) {
            const Node *input = qobject_cast<const Node *>(l.at(i));
            if (input && !result.contains(input)) {
                result.append(input);
            }
        }
    } else if (v.isNull()) {
        // skip
    } else {
        error() << "can't interpret input" << name() << v;
    }

    return result;
}

const QList<Node *> Input::nodes()
{
    return unConstList(static_cast<const Input &>(*this).nodes());
}
