#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>
#include <QSignalMapper>

#include "process.h"

Process::Process(QObject *parent) :
    Node(parent),
    m_ignoreExitCode(false)
{
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

const QString Process::stdout(int i) const
{
    if (i < m_processes.count() && m_processes.at(i) && m_processes[i]->state() == QProcess::NotRunning) {
        return m_processes[i]->readAllStandardOutput();
    }

    return QString();
}

void Process::onCookAtIndex(int i, const QVariant context)
{
    trace() << ".onCookAtIndex(" << i << "," << context << ")";

    setIndex(i);

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

    connect(m_processes[i], &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
    connect(m_processes[i], static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Process::onProcessFinished);
    connect(m_processes[i], static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, &Process::onProcessError);

    m_processes[i]->setProcessEnvironment(processEnv);

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
        emit cookedAtIndex(i, OperationAttached::Error);
    }
}

void Process::handleFinishedProcess(QProcess *process, OperationAttached::Status status)
{
    trace() << ".handleFinishedProcess(" << process << "," << status << ")";
    Q_ASSERT(process);

    int i = m_processes.indexOf(process);
    Q_ASSERT(i >= 0);

    Q_ASSERT(details().isArray());

    QString stdout = process->readAllStandardOutput();
    if (details().property(i).isUndefined())
        details().setProperty(i, newObject());
    if (details().property(i).property("process").isUndefined())
        details().property(i).setProperty("process", newObject());
    details().property(i).property("process").setProperty("stdout", stdout);
    details().property(i).property("process").setProperty("exitCode", process->exitCode());

    m_processes[i] = 0;
    process->deleteLater();

    emit detailsChanged();
    emit cookedAtIndex(i, status);
}

void Process::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    trace() << ".onProcessFinished(" << exitCode << "," << exitStatus << ") [signal from" << process << "]";

    if (exitStatus == QProcess::CrashExit || (!m_ignoreExitCode && exitCode != 0)) {
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

    handleFinishedProcess(process, OperationAttached::Error);
}

void Process::onReadyReadStandardError()
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    warning() << process->readAllStandardError();
}

void Process::componentComplete()
{
    NodeBase::componentComplete();

    m_env = newObject();

    QProcessEnvironment systemEnv = QProcessEnvironment::systemEnvironment();
    foreach (const QString key, systemEnv.keys()) {
        m_env.setProperty(key, systemEnv.value(key));
    }
}
