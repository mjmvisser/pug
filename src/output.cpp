#include "output.h"
#include "node.h"

Output::Output(QObject *parent) :
    PugItem(parent)
{
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
