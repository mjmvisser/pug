#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>
#include <QSignalMapper>
#include <QJsonDocument>

#include "process.h"
//#include "elementmanager.h"
//#include "elementview.h"

Process::Process(QObject *parent) :
    Node(parent),
    m_ignoreExitCode(false),
    m_updatingFlag(false),
    m_cookingFlag(false)
{
    connect(this, &Process::updateAtIndex, this, &Process::onUpdateAtIndex);
    connect(this, &Process::cookAtIndex, this, &Process::onCookAtIndex);
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

void Process::onUpdateAtIndex(int i, const QVariant context)
{
    trace() << ".onUpdateAtIndex(" << i << "," << context << ")";
    setUpdating(true);
    setIndex(i);
    if (m_argv.isEmpty()) {
        warning() << "no args, skipping update";
        setUpdating(false);
        emit updateAtIndexFinished(i, OperationAttached::Finished);
        return;
    }

    Q_ASSERT(!m_cookingFlag);
    executeAtIndex(i, context);
}

void Process::onCookAtIndex(int i, const QVariant context)
{
    trace() << ".onCookAtIndex(" << i << "," << context << ")";
    setCooking(true);
    setIndex(i);
    if (m_argv.isEmpty()) {
        warning() << "no args, skipping cook";
        setCooking(false);
        emit cookAtIndexFinished(i, OperationAttached::Finished);
        return;
    }

    Q_ASSERT(!m_updatingFlag);
    executeAtIndex(i, context);
}

void Process::executeAtIndex(int i, const QVariant context)
{
    Q_ASSERT(m_updatingFlag || m_cookingFlag);

    Q_ASSERT(details().isArray());

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

    m_processes[i] = new QProcess(this);

    connect(m_processes[i], &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(m_processes[i], &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
    connect(m_processes[i], static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Process::onProcessFinished);
    connect(m_processes[i], static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, &Process::onProcessError);

    m_processes[i]->setProcessEnvironment(processEnv);

    m_contexts[m_processes[i]] = context.toMap();

    debug() << "process argv:" << m_argv;

    QStringList args = m_argv;

    if (args.length() > 0) {
        QString program = args.takeFirst();

        m_processes[i]->start(program, args);

        // write any input
        if (!m_stdin.isEmpty()) {
            info() << m_stdin;
            m_processes[i]->write(m_stdin.toUtf8());
        }

        // close stdin so we don't hang indefinitely
        m_processes[i]->closeWriteChannel();
    } else {
        error() << "Process has no args";
        emit cookAtIndexFinished(i, OperationAttached::Error);
    }
}

void Process::handleFinishedProcess(QProcess *process, OperationAttached::Status status)
{
    trace() << ".handleFinishedProcess(" << process << "," << status << ")";
    Q_ASSERT(process);
    Q_ASSERT(m_cookingFlag || m_updatingFlag);

    int i = m_processes.indexOf(process);
    Q_ASSERT(i >= 0);

    QString stdout = m_stdouts.take(process);

    debug() << "stdout:" << stdout;

    QVariantMap context = m_contexts.take(process);
    setDetail(i, "context", toScriptValue(context));
    setDetail(i, "process", "stdout", toScriptValue(stdout));
    setDetail(i, "process", "exitCode", toScriptValue(process->exitCode()));

    m_processes[i] = 0;
    process->deleteLater();

    int finishedProcessCount = 0;
    qCount(m_processes, static_cast<QProcess *>(0), finishedProcessCount);
    if (m_updatingFlag) {
        if (finishedProcessCount == count()) {
            // all done
            setUpdating(false);
        }
        emit updateAtIndexFinished(i, status);
    } else if (m_cookingFlag) {
        if (finishedProcessCount == count()) {
            // all done
            setCooking(false);
        }
        emit cookAtIndexFinished(i, status);
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
    debug() << process->readAllStandardError();
}

void Process::componentComplete()
{
    Node::componentComplete();

    m_env = newObject();

    QProcessEnvironment systemEnv = QProcessEnvironment::systemEnvironment();
    foreach (const QString key, systemEnv.keys()) {
        m_env.setProperty(key, systemEnv.value(key));
    }
}
