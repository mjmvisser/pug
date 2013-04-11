#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

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

int Pug::s_sigIntFd[2] = {0,0};

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

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, s_sigIntFd))
        qFatal("Couldn't create INT socketpair");

    m_sigIntSocketNotifier = new QSocketNotifier(s_sigIntFd[1], QSocketNotifier::Read, this);
    connect(m_sigIntSocketNotifier, &QSocketNotifier::activated, this, &Pug::handleSigInt);

    struct sigaction sigInt;

    sigInt.sa_handler = Pug::intSignalHandler;
    sigemptyset(&sigInt.sa_mask);
    sigInt.sa_flags = 0;
    sigInt.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &sigInt, 0) == -1)
       qFatal("Couldn't hook SIGINT");
}

void Pug::intSignalHandler(int unused)
{
    Q_UNUSED(unused);
    char a = 1;
    ::write(s_sigIntFd[0], &a, sizeof(a));
}

void Pug::handleSigInt(int unused)
{
    Q_UNUSED(unused);

    m_sigIntSocketNotifier->setEnabled(false);
    char tmp;
    ::read(s_sigIntFd[1], &tmp, sizeof(tmp));

    qDebug() << "Interrupted";

    QCoreApplication::exit(1);

    m_sigIntSocketNotifier->setEnabled(true);
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

OperationAttached::Status Pug::runOperation(const QString operationName, const QVariantMap properties, const QString nodePath, const QVariantMap context)
{
    // find the operation
    Operation *op = m_root->findOperation(operationName);

    if (!op) {
        PugItem::error() << "Can't find operation" << operationName;
        return OperationAttached::Invalid;
    }

//    Branch *branch = m_root->findBranch(path);
//
//    if (!branch) {
//        PugItem::error() << "Can't find branch for" << path;
//        return OperationAttached::Invalid;
//    }
//
//    QVariantMap data = branch->parse(path).toMap();
//
//    for (QVariantMap::const_iterator it = context.constBegin(); it != context.constEnd(); ++it) {
//        if (!data.contains(it.key())) {
//            data.insert(it.key(), it.value());
//        }
//    }

    Node *node = m_root->node(nodePath);

    if (node->path() != nodePath) {
        PugItem::error() << "path of node returned is" << node->path() << "not" << nodePath;
        return OperationAttached::Error;
    }

    if (!node) {
        PugItem::error() << "Can't find node" << nodePath;
        return OperationAttached::Error;
    }

    for (QVariantMap::const_iterator it = properties.constBegin(); it != properties.constEnd(); ++it) {
        bool result = op->setProperty(it.key().toUtf8().constData(), it.value());
        if (!result) {
            PugItem::error() << "Failed to set property" << it.key() << "to" << it.value();
        }
    }

    // run a local event loop, terminated when the operation finishes
    QEventLoop eventLoop;
    connect(op, SIGNAL(finished(OperationAttached::Status)), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(op, "run", Qt::QueuedConnection, Q_ARG(Node*, node), Q_ARG(QVariant, context));
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
