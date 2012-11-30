#include <QDebug>
#include <QUrl>

#include "pug.h"
#include "operation.h"

/*!
    \qmlmodule Pug
*/

/*!
    \class Pug
    \inmodule Pug
*/

/*!
    \qmltype Pug
    \instantiates Pug
    \brief Main entry point for access to the Pug graph.

    Main entry point for access to the Pug graph.
*/

Pug::Pug(QQmlEngine* engine, QObject *parent) :
        PugItem(parent),
        m_engine(engine),
        m_rootComponent()
{
    // QML_IMPORT_PATH doesn't seem to work in qt5 anymore
    QString importPaths = qgetenv("QML_IMPORT_PATH");
    QStringList importPathList = importPaths.split(':', QString::SkipEmptyParts);
    foreach (QString importPath, importPathList) {
        m_engine->addImportPath(importPath);
    }
}

const QVariantMap Pug::parse(const QString node, const QString path) const {
    // find root object
    QVariant result;
    QMetaObject::invokeMethod(m_root, "parse",
            Q_RETURN_ARG(QVariant, result),
            Q_ARG(QVariant, node),
            Q_ARG(QVariant, path));
    return result.toMap();
}

const QString Pug::map(const QString node, const QVariantMap fields) const {
    QVariant result;
    QMetaObject::invokeMethod(m_root, "map",
            Q_RETURN_ARG(QVariant, result),
            Q_ARG(QVariant, node),
            Q_ARG(QVariant, fields));
    return result.toString();
}

OperationAttached::Status Pug::runOperation(const QString operationName, const QVariantMap properties, const QString path, const QVariantMap extraData)
{
    // find the operation
    Operation *op = m_root->findOperation(operationName);

    if (!op) {
        PugItem::error() << "Can't find operation" << operationName;
        return OperationAttached::Invalid;
    }

    BranchBase *branch = m_root->findBranch(path);

    if (!branch) {
        PugItem::error() << "Can't find branch for" << path;
        return OperationAttached::Invalid;
    }

    QVariantMap data = branch->parse(path).toMap();

    for (QVariantMap::const_iterator it = extraData.constBegin(); it != extraData.constEnd(); ++it) {
        if (!data.contains(it.key()))
            data.insert(it.key(), it.value());
    }

    for (QVariantMap::const_iterator it = properties.constBegin(); it != properties.constEnd(); ++it) {
        op->setProperty(it.key().toUtf8().constData(), it.value());
    }

    // run a local event loop, terminated when the operation finishes
    QEventLoop eventLoop;
    connect(op, SIGNAL(finished(OperationAttached::Status)), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(op, "run", Qt::QueuedConnection, Q_ARG(NodeBase*, branch), Q_ARG(QVariant, data));
    eventLoop.exec();

    return op->status();
}

Root *Pug::root() {
    return m_root;
}

void Pug::load(const QString path) {
    if (path.isEmpty()) {
        emit error();
    } else {
        m_rootComponent = new QQmlComponent(m_engine, path, this);
        if (m_rootComponent->isLoading()) {
            connect(m_rootComponent, SIGNAL(statusChanged(QQmlComponent::Status)),
                    this, SLOT(continueLoading()));
        } else {
            continueLoading();
        }
    }
}

void Pug::continueLoading() {
    if (m_rootComponent->isError()) {
        qWarning() << m_rootComponent->errors();
        emit error();
    } else {
        m_root = qobject_cast<Root*>(m_rootComponent->create());
        emit loaded();
    }
}
