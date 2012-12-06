#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>
#include <QSignalMapper>

#include "process.h"

Process::Process(QObject *parent) :
    NodeBase(parent),
    m_ignoreExitCode(false)
{
    connect(this, &Process::cookAtIndex, this, &Process::onCookAtIndex);
}

QStringList Process::argv()
{
    return m_argv;
}

void Process::setArgv(QStringList a)
{
    if (m_argv != a) {
        m_argv = a;
        emit argvChanged(a);
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

void Process::onCookAtIndex(int i, const QVariant env)
{
    debug() << "Process::onCookAtIndex(" << i << "," << env << ")";

    setIndex(i);

    if (m_processes.size() != count())
        m_processes.resize(count());

    QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();

    // add our env
    for (QVariantMap::const_iterator it = env.toMap().constBegin(); it != env.toMap().constEnd(); ++it) {
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

    QStringList args = m_argv;

    debug() << "Process" << args;

    if (args.length() > 0) {
        QString program = args.takeFirst();

        m_processes[i]->start(program, args);

        // close stdin so we don't hang indefinitely
        m_processes[i]->closeWriteChannel();
    } else {
        error() << "Process has no args";
        emit cookedAtIndex(i, OperationAttached::Error);
    }
}

void Process::handleFinishedProcess(QProcess *process, OperationAttached::Status status)
{
    Q_ASSERT(process);

    int i = m_processes.indexOf(process);
    Q_ASSERT(i >= 0);

    QString stdout = process->readAllStandardOutput();
    setDetail(i, "process", "stdout", stdout);
    setDetail(i, "process", "exitCode", process->exitCode());

    m_processes[i] = 0;
    process->deleteLater();

    emit cookedAtIndex(i, status);
}

void Process::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    debug() << "process finished with exit code" << exitCode << "exit status" << exitStatus;

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
    debug() << "process error" << process->errorString();

    handleFinishedProcess(process, OperationAttached::Error);
}

void Process::onReadyReadStandardError()
{
    QProcess *process = qobject_cast<QProcess *>(QObject::sender());
    error() << process->readAllStandardError();
}
