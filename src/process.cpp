#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>
#include <QSignalMapper>
#include <QJsonDocument>

#include "process.h"
#include "updateoperation.h"
#include "cookoperation.h"
//#include "elementmanager.h"
//#include "elementview.h"

Process::Process(QObject *parent) :
    Node(parent),
    m_ignoreExitCode(false),
    m_updatableFlag(false),
    m_cookableFlag(false),
    m_updatingFlag(false),
    m_cookingFlag(false)
{
}

void Process::componentComplete()
{
    Node::componentComplete();

    connect(attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>(), &UpdateOperationAttached::cook,
            this, &Process::update_onCook);
    connect(attachedPropertiesObject<CookOperation, CookOperationAttached>(), &CookOperationAttached::cook,
            this, &Process::cook_onCook);

    m_env = newObject();

    QProcessEnvironment systemEnv = QProcessEnvironment::systemEnvironment();
    foreach (const QString key, systemEnv.keys()) {
        m_env.setProperty(key, systemEnv.value(key));
    }
}

const QStringList Process::argv() const
{
    return m_argv;
}

void Process::setArgv(const QStringList a)
{
    if (m_argv != a) {
        m_argv = a;
        emit argvChanged(a);
    }
}

const QString Process::stdin() const
{
    return m_stdin;
}

void Process::setStdin(const QString s)
{
    if (m_stdin != s) {
        m_stdin = s;
        emit stdinChanged(s);
    }
}

QJSValue Process::env()
{
    return m_env;
}

void Process::setEnv(const QJSValue env)
{
    if (!m_env.strictlyEquals(env)) {
        if (env.isObject()) {
            m_env = env;
            emit envChanged();
        } else {
            warning() << "Ignoring attempt to set env to" << m_env.toString();
        }
    }
}

bool Process::ignoreExitCode() const
{
    return m_ignoreExitCode;
}

void Process::setIgnoreExitCode(bool flag)
{
    if (m_ignoreExitCode != flag) {
        m_ignoreExitCode = flag;
        emit ignoreExitCodeChanged(flag);
    }
}

bool Process::isUpdatable() const
{
    return m_updatableFlag;
}

void Process::setUpdatable(bool f)
{
    if (m_updatableFlag != f) {
        m_updatableFlag = f;
        emit updatableChanged(f);
    }
}

bool Process::isCookable() const
{
    return m_cookableFlag;
}

void Process::setCookable(bool f)
{
    if (m_cookableFlag != f) {
        m_cookableFlag = f;
        emit cookableChanged(f);
    }
}

bool Process::isUpdating() const
{
    return m_updatingFlag;
}

void Process::setUpdating(bool f)
{
    if (m_updatingFlag != f) {
        m_updatingFlag = f;
        emit updatingChanged(f);
    }
}

bool Process::isCooking() const
{
    return m_cookingFlag;
}

void Process::setCooking(bool f)
{
    if (m_cookingFlag != f) {
        m_cookingFlag = f;
        emit cookingChanged(f);
    }
}

void Process::update_onCook(const QVariant context)
{
    trace() << ".UpdateOperation.onCook(" << context << ")";
    Q_ASSERT(!m_cookingFlag);
    if (m_updatableFlag) {
        // TODO: what if count is 0?
        clearDetails();
        setUpdating(true);
        m_statuses.clear();
        for (int index = 0; index < count(); index++) {
            setIndex(index);
            executeForIndex(index, context);
        }
    } else {
        debug() << "not updatable, skipping update";
        emit attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>()->cookFinished(OperationAttached::Finished);
    }
}

void Process::cook_onCook(const QVariant context)
{
    trace() << ".CookOperation.onCook(" << context << ")";
    Q_ASSERT(!m_updatingFlag);
    if (m_cookableFlag) {
        setCooking(true);
        m_statuses.clear();
        for (int index = 0; index < count(); index++) {
            setIndex(index);
            executeForIndex(index, context);
        }
    } else {
        debug() << "not cookable, skipping cook";
        emit attachedPropertiesObject<CookOperation, CookOperationAttached>()->cookFinished(OperationAttached::Finished);
    }
}

void Process::executeForIndex(int index, const QVariant context)
{
    Q_ASSERT(m_updatingFlag || m_cookingFlag);

    Q_ASSERT(details().isArray());

    // save the context
    setDetail(index, "context", toScriptValue(context));

    if (m_processes.size() != count())
        m_processes.resize(count());

    QProcessEnvironment processEnv;

    QJSValueIterator it(m_env);
    while (it.hasNext()) {
        it.next();
        processEnv.insert(it.name(), it.value().toString());
    }

    // add our env
    for (QVariantMap::const_iterator it = context.toMap().constBegin(); it != context.toMap().constEnd(); ++it) {
        if (it.value().canConvert<QString>())
            processEnv.insert(it.key(), it.value().toString());
    }

    m_processes[index] = new QProcess(this);

    connect(m_processes[index], &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(m_processes[index], &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
    connect(m_processes[index], static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Process::onProcessFinished);
    connect(m_processes[index], static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, &Process::onProcessError);

    m_processes[index]->setProcessEnvironment(processEnv);

    // stash the context before we get argv, since that may change it
    setDetail(index, "context", toScriptValue(context));

    debug() << "process argv:" << m_argv;

    setDetail(index, "process", "argv", toScriptValue(m_argv));

    QStringList args = m_argv;

    QString program;
    if (!args.isEmpty())
        program = args.takeFirst();

    m_processes[index]->start(program, args);

    // write any input
    if (!m_stdin.isEmpty()) {
        info() << m_stdin;
        m_processes[index]->write(m_stdin.toUtf8());
    }

    // close stdin so we don't hang indefinitely
    m_processes[index]->closeWriteChannel();
}

void Process::handleFinishedProcess(QProcess *process, OperationAttached::Status status)
{
    trace() << ".handleFinishedProcess(" << process << "," << status << ")";
    Q_ASSERT(process);
    Q_ASSERT(m_cookingFlag || m_updatingFlag);

    int index = m_processes.indexOf(process);
    Q_ASSERT(index >= 0);

    QString stdout = m_stdouts.take(process);
    QString stderr = m_stderrs.take(process);

    setDetail(index, "process", "stdout", toScriptValue(stdout));
    setDetail(index, "process", "stderr", toScriptValue(stderr));
    setDetail(index, "process", "exitCode", toScriptValue(process->exitCode()));

    m_processes[index] = 0;
    process->deleteLater();

    m_statuses << status;

    int finishedProcessCount = 0;
    qCount(m_processes, static_cast<QProcess *>(0), finishedProcessCount);
    if (m_updatingFlag) {
        if (finishedProcessCount == count()) {
            // all done
            setUpdating(false);
            emit attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>()->cookFinished(m_statuses.status());
        }
    } else if (m_cookingFlag) {
        if (finishedProcessCount == count()) {
            // all done
            setCooking(false);
            emit attachedPropertiesObject<CookOperation, CookOperationAttached>()->cookFinished(m_statuses.status());
        }
    }
}

void Process::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    trace() << ".onProcessFinished(" << exitCode << "," << exitStatus << ") [signal from" << process << "]";

    if (exitStatus == QProcess::CrashExit || (!m_ignoreExitCode && exitCode != 0)) {
        error() << "process failed:" << m_argv;
        handleFinishedProcess(process, OperationAttached::Error);
    } else {
        handleFinishedProcess(process, OperationAttached::Finished);
    }
}

void Process::onProcessError(QProcess::ProcessError err)
{
    Q_UNUSED(err);

    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    trace() << ".onProcessError(" << process->errorString() << ")";

    error() << "process failed:" << m_argv;
    error() << "stdout:" << m_stdouts[process];
    error() << "stderr:" << m_stderrs[process];
    handleFinishedProcess(process, OperationAttached::Error);
}

void Process::onReadyReadStandardOutput()
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    m_stdouts[process].append(process->readAllStandardOutput());
}

void Process::onReadyReadStandardError()
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    m_stdouts[process].append(process->readAllStandardError());
}
