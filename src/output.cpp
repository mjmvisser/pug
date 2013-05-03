#include "output.h"
#include "node.h"

Output::Output(QObject *parent) :
    PugItem(parent)
{
}

QQmlListProperty<Node> Output::nodes_()
{
    return QQmlListProperty<Node>(this, 0,
                                  Output::nodes_count,
                                  Output::node_at);
}

// nodes property
int Output::nodes_count(QQmlListProperty<Node> *prop)
{
    Output *that = static_cast<Output *>(prop->object);
    return that->nodes().count();
}

Node *Output::node_at(QQmlListProperty<Node> *prop, int i)
{
    Output *that = static_cast<Output *>(prop->object);
    if (i < that->nodes().count())
        return that->nodes().at(i);
    else
        return 0;
}


const QList<const Node *> Output::nodes() const
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
        error() << "can't interpret output" << name() << v;
    }
    //trace() << "    ->" << result;
    return result;
}

const QList<Node *> Output::nodes()
{
    return unConstList(static_cast<const Output &>(*this).nodes());
}
