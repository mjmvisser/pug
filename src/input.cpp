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
    //trace() << "    ->" << result;
    return result;
}

const QList<Node *> Input::nodes()
{
    return unConstList(static_cast<const Input &>(*this).nodes());
}
