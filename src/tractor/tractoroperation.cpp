#include <iostream>

#include <QVariant>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "tractoroperation.h"
#include "tractortask.h"
#include "tractorcmd.h"
#include "tractorjob.h"
#include "node.h"
#include "folder.h"

TractorOperationAttached::TractorOperationAttached(QObject *parent) :
        OperationAttached(parent),
        m_serialSubtasksFlag(false),
        m_flattenFlag(false),
        m_task(new TractorTask(this))
{
    setObjectName("tractor");
}

TractorOperationAttached *TractorOperationAttached::parentAttached()
{
    Node *p = node()->parent<Node>();
    return p ? p->attachedPropertiesObject<TractorOperationAttached>(operationMetaObject()) : 0;
}

TractorTask *TractorOperationAttached::tractorTask()
{
    return m_task;
}

const QString &TractorOperationAttached::serviceKey() const
{
    return m_serviceKey;
}

void TractorOperationAttached::setServiceKey(const QString &key)
{
    if (m_serviceKey != key) {
        m_serviceKey = key;
        emit serviceKeyChanged(key);
    }
}

const QString &TractorOperationAttached::tags() const
{
    return m_tags;
}

void TractorOperationAttached::setTags(const QString &t)
{
    if (m_tags != t) {
        m_tags = t;
        emit tagsChanged(t);
    }
}

bool TractorOperationAttached::hasSerialSubtasks() const
{
    return m_serialSubtasksFlag;
}

void TractorOperationAttached::setSerialSubtasks(bool flag)
{
    if (m_serialSubtasksFlag != flag) {
        m_serialSubtasksFlag = flag;
        emit serialSubtasksChanged(flag);
    }
}

bool TractorOperationAttached::isFlatten() const
{
    return m_flattenFlag;
}

void TractorOperationAttached::setFlatten(bool flag)
{
    if (m_flattenFlag != flag) {
        m_flattenFlag = flag;
        emit flattenChanged(flag);
    }
}

void TractorOperationAttached::run()
{
    //info() << "Tractor" << operation<TractorOperation>()->mode() << "on" << node();
    trace() << node() << ".run() [mode is" << operation<TractorOperation>()->mode() << "on" << node() << "]";

    switch(operation<TractorOperation>()->mode()) {
    case TractorOperation::Submit:
    case TractorOperation::Generate:
        generateTask();
        break;
    case TractorOperation::Execute:
        executeTask();
        break;
    case TractorOperation::Cleanup:
        cleanupTask();
        break;
    }

}

void TractorOperationAttached::setupTask(TractorTask *task)
{

    // get the executable path
    QString pugExecutable = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("pug");

    TractorRemoteCmd *cmd = new TractorRemoteCmd(task);
    cmd->setCmd(QString("%1 %2 -properties mode=Execute -context %3 -- %4")
            .arg(pugExecutable).arg(operation()->objectName())
            .arg(contextString())
            .arg(node()->path()));
    cmd->setTags(m_tags);
    task->addCmd(cmd);
    task->setTitle(node()->path() + ":self");
}

void TractorOperationAttached::generateTask()
{
    trace() << node() << ".generateTask()";

    QList<TractorTask *> inputSubtasks;

    if (!m_flattenFlag) {
        foreach (Node *input, node()->upstream()) {
            TractorOperationAttached *inputAttached = input->attachedPropertiesObject<TractorOperationAttached>(operationMetaObject());
            inputSubtasks << inputAttached->tractorTask();
        }
    }

    QList<TractorTask *> childSubtasks;

    foreach (QObject* obj, node()->children()) {
        Node *child = qobject_cast<Node *>(obj);
        if (child && child->isOutput()) {
            TractorOperationAttached *childAttached = child->attachedPropertiesObject<TractorOperationAttached>(operationMetaObject());
            childSubtasks << childAttached->tractorTask();
        }
    }

    if (inputSubtasks.isEmpty() && childSubtasks.isEmpty()) {
        // simplest case: no children or inputs
        setupTask(m_task);
    } else {
        // we need an empty node to organize the tree
        m_task->setTitle(node()->path());
        m_task->setSerialSubtasks(true);

        TractorTask *task = new TractorTask(this);
        setupTask(task);

        TractorTask *inputsSubtask = 0;

        if (!inputSubtasks.isEmpty()) {
            if (inputSubtasks.length() == 1) {
                inputsSubtask = inputSubtasks.first();
            } else {
                inputsSubtask = new TractorTask(m_task);
                inputsSubtask->setTitle(node()->path() + ":inputs");
                foreach (TractorTask *subtask, inputSubtasks) {
                    inputsSubtask->addSubtask(subtask);
                }
            }
        }

        TractorTask *childrenSubtask = 0;

        if (!childSubtasks.isEmpty()) {
            if (childSubtasks.length() == 1) {
                childrenSubtask = childSubtasks.first();
            } else {
                childrenSubtask = new TractorTask(m_task);
                childrenSubtask->setTitle(node()->path() + ":children");
                foreach (TractorTask *subtask, childSubtasks) {
                    childrenSubtask->addSubtask(subtask);
                }
            }
        }

        switch (node()->dependencyOrder()) {
        case Node::InputsChildrenSelf:
            if (inputsSubtask)
                m_task->addSubtask(inputsSubtask);
            if (childrenSubtask)
                m_task->addSubtask(childrenSubtask);
            m_task->addSubtask(task);
            break;
        case Node::InputsSelfChildren:
            if (inputsSubtask)
                m_task->addSubtask(inputsSubtask);
            m_task->addSubtask(task);
            if (childrenSubtask)
                m_task->addSubtask(childrenSubtask);
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }

    setStatus(OperationAttached::Finished);
    continueRunning();
}

const QString TractorOperationAttached::contextString() const
{
    QString s;
    QTextStream stream(&s);

    QMapIterator<QString, QVariant> i(context());
    while (i.hasNext()) {
        i.next();
        stream << i.key() << '=' << i.value().toString();
        if (i.hasNext())
            stream << " ";
    }
    return s;
}

void TractorOperationAttached::executeTask()
{
    trace() << node() << ".executeTask()";

    if (node()->isRoot()) {
        setStatus(OperationAttached::Finished);
        continueRunning();
        return;
    }

    Operation *targetOperation = operation<TractorOperation>()->target();

    if (node() == operation()->node()) {
        Q_ASSERT(targetOperation);

        info() << "node path is" << node()->path();

        QObject *p = node();
        while (p) {
            info() << p;
            p = p->parent();
        }

        connect(targetOperation, SIGNAL(finished(OperationAttached::Status)),
                this, SLOT(onTargetFinished(OperationAttached::Status)));

        info() << "Running target:" << targetOperation << "on" << node();

        OperationAttached::Targets targets = OperationAttached::Self|OperationAttached::Inputs;
        if (node()->dependencyOrder() == Node::InputsChildrenSelf)
            targets |= OperationAttached::Children;

        targetOperation->run(node(), context(), false, targets);
    } else {
        // we're an input or child of the run node
        readTractorData(tractorDataPath());
        continueRunning();
    }
}

void TractorOperationAttached::cleanupTask()
{
    trace() << node() << ".cleanupTask()";
    debug() << node() << "removing" << tractorDataPath();
    bool success = QFile::remove(tractorDataPath());
    if (!success)
        warning() << node() << "Unable to remove file:" << tractorDataPath();
    setStatus(OperationAttached::Finished);
    continueRunning();
}

void TractorOperationAttached::onTargetFinished(OperationAttached::Status status)
{
    Operation *targetOperation = operation<TractorOperation>()->target();
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());

    trace() << node() << ".onTargetFinished(" << status << ")";

    disconnect(targetOperation, SIGNAL(finished(OperationAttached::Status)),
               this, SLOT(onTargetFinished(OperationAttached::Status)));

    if (status == OperationAttached::Finished) {
        // write this node's data
        writeTractorData(tractorDataPath());

        setStatus(targetAttached->status());

        // continue cooking
        continueRunning();
    } else {
        setStatus(OperationAttached::Error);
        continueRunning();
    }
}

void TractorOperationAttached::writeTractorData(const QString &dataPath) const
{
    trace() << node() << ".writeTractorData(" << dataPath << ")";

    const Operation *targetOperation = operation<TractorOperation>()->target();

    QFile file(dataPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error() << "can't open file" << tractorDataPath() << "for writing";
        return;
    }

    QJsonObject obj;
    obj.insert("details", QJsonArray::fromVariantList(node()->details().toVariant().toList()));
    obj.insert("count", QJsonValue(node()->count()));
    obj.insert("statuses", QJsonObject::fromVariantMap(tractorAttachedStatuses(targetOperation)));

    // write details and statuses to data dir
    file.write(QJsonDocument(obj).toJson());

    file.close();
    info() << "Wrote" << dataPath;
}

const QVariantMap TractorOperationAttached::tractorAttachedStatuses(const Operation *operation, const QVariantMap statuses) const
{
    QVariantMap result = statuses;

    // write dependency statuses
    foreach(const Operation *dep, operation->dependencies()) {
        result = tractorAttachedStatuses(dep, result);
    }

    // write own status
    const OperationAttached *attached = node()->attachedPropertiesObject<OperationAttached>(operation->metaObject());
    Q_ASSERT(attached);
    Q_ASSERT(attached->status() != OperationAttached::Invalid);
    Q_ASSERT(attached->operation());
    Q_ASSERT(!attached->operation()->name().isEmpty());
    result.insert(attached->operation()->name(), static_cast<int>(attached->status()));

    return result;
}

void TractorOperationAttached::readTractorData(const QString &dataPath)
{
    trace() << node() << ".readTractorData(" << dataPath << ")";

    Operation *targetOperation = operation<TractorOperation>()->target();

    Q_ASSERT(targetOperation);

    QFile file(dataPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //error() << dataPath << "doesn't exist";   // not necessarily an error
        setStatus(OperationAttached::Finished);
        return;
    }

    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();

    node()->setDetails(toScriptValue(obj["details"].toArray().toVariantList()));
    node()->setCount(obj["count"].toDouble());

    // TODO: error handling

    QVariantMap leftovers = setTractorAttachedStatuses(targetOperation, obj["statuses"].toObject().toVariantMap());

    // we inherit the result of the target operation on the node
    OperationAttached *targetAttached = node()->attachedPropertiesObject<OperationAttached>(targetOperation->metaObject());
    setStatus(targetAttached->status());

    info() << "Read" << dataPath;

    Q_ASSERT(leftovers.isEmpty());
}

const QVariantMap TractorOperationAttached::setTractorAttachedStatuses(Operation *operation, const QVariantMap statuses)
{
    QVariantMap result = statuses;

    // read dependency statuses
    foreach(Operation *dep, operation->dependencies()) {
        result = setTractorAttachedStatuses(dep, result);
    }

    // read own status
    OperationAttached *attached = node()->attachedPropertiesObject<OperationAttached>(operation->metaObject());

    Q_ASSERT(!operation->name().isEmpty());
    int status = result.take(operation->name()).toInt();
    attached->setStatus(static_cast<OperationAttached::Status>(status));
    Q_ASSERT(static_cast<OperationAttached::Status>(status) != OperationAttached::Invalid);

    //info() << "set" << operation->name() << "status on" << attached->node() << "to" << attached->status();

    return result;
}

const QString TractorOperationAttached::tractorDataPath() const
{
    QString filename = node()->path() + ".json";
    filename.replace(QDir::separator(), '-');
    QString path = QDir(context()["TRACTOR_DATA_DIR"].toString()).filePath(filename);
    return path;
}

const QMetaObject *TractorOperationAttached::operationMetaObject() const
{
    return &TractorOperation::staticMetaObject;
}

TractorOperation::TractorOperation(QObject *parent) :
        Operation(parent),
        m_mode(TractorOperation::Submit),
        m_target()
{
}

TractorOperation::Mode TractorOperation::mode() const
{
    return m_mode;
}

void TractorOperation::setMode(TractorOperation::Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        emit modeChanged(mode);
    }
}

Operation *TractorOperation::target()
{
    return m_target;
}

const Operation *TractorOperation::target() const
{
    return m_target;
}

void TractorOperation::setTarget(Operation *target)
{
    if (m_target != target) {
        m_target = target;
        emit targetChanged(target);
    }
}

static QStringList SKIP_VARS =
        QStringList()
        << "ARCH"
        << "BASH"
        << "BASHOPTS"
        << "BASH_ALIASES"
        << "BASH_ARGC"
        << "BASH_ARGV"
        << "BASH_CMDS"
        << "BASH_EXECUTION_STRING"
        << "BASH_LINENO"
        << "BASH_SOURCE"
        << "BASH_VERSINFO"
        << "BASH_VERSION"
        << "DBUS_SESSION_BUS_ADDRESS""OSTYPE"
        << "DESKTOP_SESSION"
        << "DIRSTACK"
        << "EUID"
        << "GROUP"
        << "GROUPS"
        << "HISTCONTROL"
        << "HISTSIZE"
        << "HOME"
        << "HOSTNAME"
        << "HOSTTYPE"
        << "IFS"
        << "IMSETTINGS_INTEGRATE_DESKTOP"
        << "IMSETTINGS_MODULE"
        << "KDEDIRS"
        << "KDE_FULL_SESSION"
        << "KDE_IS_PRELINKED"
        << "KDE_MULTIHEAD"
        << "KDE_SESSION_UID"
        << "KDE_SESSION_VERSION"
        << "KMP_DUPLICATE_LIB_OK"
        << "KONSOLE_DBUS_SERVICE"
        << "KONSOLE_DBUS_SESSION"
        << "LANG"
        << "LANGUAGE"
        << "LOGNAME"
        << "LS_COLORS"
        << "MACHTYPE_"
        << "MAIL"
        << "MAYA_APP_DIR"
        << "OPTERR"
        << "OPTIND"
        << "OS"
        << "OSTYPE"
        << "POSIXLY_CORRECT"
        << "PPID"
        << "PYTHONHOME" // inserted by Maya
        << "PROFILEHOME"
        << "SHELL"
        << "SHELLOPTS"
        << "SHLVL"
        << "SSH_AGENT_PID"
        << "SSH_ASKPASS"
        << "SSH_AUTH_SOCK"
        << "WINDOWID"
        << "WINDOWPATH"
        << "XCURSOR_THEME"
        << "UID";

void TractorOperation::run(Node *node, const QVariant context, bool reset)
{
    QThread::sleep(1); // wait for NFS to catch up

    trace() << ".run(" << node << "," << context << "," << reset << ")";
    //
    m_target->resetAll(node, context.toMap());
    m_target->resetAllStatus(node);

    QVariantMap context2 = context.toMap();

    if (m_mode == TractorOperation::Generate || m_mode == TractorOperation::Submit) {
        QDir dir;
        dir.mkpath(context.toMap()["TRACTOR_DATA_DIR"].toString());

        // build a unique path for this tractor job (%j is replaced by tractor with job id)
        QString tractorDataDir = QDir(context.toMap()["TRACTOR_DATA_DIR"].toString()).filePath("%j/");
        context2["TRACTOR_DATA_DIR"] = tractorDataDir;

    } else { // Execute
        QDir dir;
        dir.mkpath(context.toMap()["TRACTOR_DATA_DIR"].toString());
    }

    Operation::run(node, context2, reset);

    if (m_mode == TractorOperation::Generate || m_mode == TractorOperation::Submit) {
        TractorJob *tractorJob = buildTractorJob(node, context2);

        // env dump
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        // add our env
        for (QVariantMap::const_iterator it = context.toMap().constBegin(); it != context.toMap().constEnd(); ++it) {
            if (it.value().canConvert<QString>())
                env.insert(it.key(), it.value().toString());
        }

        QTemporaryFile envFile(QDir(context.toMap()["TRACTOR_DATA_DIR"].toString()).filePath("tractorXXXXXX.env"));
        envFile.setAutoRemove(false);
        if (envFile.open()) {
            QTextStream stream(&envFile);
            foreach (const QString key, env.keys()) {
                if (!SKIP_VARS.contains(key)) {
                    stream << key << "=" << env.value(key) << endl;
                }
            }
            envFile.close();

            envFile.setPermissions(envFile.permissions() | QFile::ReadGroup | QFile::ReadOwner | QFile::ReadUser);

            tractorJob->setEnvKey("envdump=" + envFile.fileName());

        } else {
            error() << "Can't write environment to" << envFile.fileName();
        }

        if (m_mode == TractorOperation::Submit) {
            tractorJob->submit();
        } else {
            QSet<const TractorBlock *> visited;
            info() << tractorJob->asString(0, visited);
        }
        tractorJob->deleteLater();
    }
}

TractorJob *TractorOperation::buildTractorJob(Node *node, const QVariant context)
{
    trace() << ".buildTractorJob(" << node << "," << context << ")";
    TractorJob *job = new TractorJob(this);

    TractorOperationAttached *attached = node->attachedPropertiesObject<TractorOperationAttached>(metaObject());

    job->setTitle(context.toMap()["TITLE"].toString());
    job->setMetadata("retrycount=0");
    job->addSubtask(attached->tractorTask());

    return job;
}

TractorOperationAttached *TractorOperation::qmlAttachedProperties(QObject *object)
{
    return new TractorOperationAttached(object);
}

QDebug operator<<(QDebug dbg, TractorOperation::Mode mode)
{
    switch(mode) {
    case TractorOperation::Submit:
        dbg << "Submit";
        break;
    case TractorOperation::Generate:
        dbg << "Generate";
        break;
    case TractorOperation::Execute:
        dbg << "Execute";
        break;
    case TractorOperation::Cleanup:
        dbg << "Cleanup";
        break;
    default:
        dbg << "Unknown!";
        break;
    }

    return dbg;
}
