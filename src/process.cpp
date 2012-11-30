#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>

#include "process.h"

Process::Process(QObject *parent) :
    NodeBase(parent)
{
    m_process = new QProcess(this);

    connect(m_process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Process::onProcessFinished);
    connect(m_process, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, &Process::onProcessError);
    connect(this, &Process::cook, this, &Process::onCook);
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

const QString Process::stdout() const
{
    return m_stdout;
}

void Process::onCook(const QVariant env)
{
    debug() << "Process::onCook";
    m_stdout.clear();

    // TODO: at some point, we'll need to load these from a file instead of using the system environment
    QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();

    // add our env
    for (QVariantMap::const_iterator i = env.toMap().constBegin(); i != env.toMap().constEnd(); ++i) {
        if (i.value().canConvert<QString>())
            processEnv.insert(i.key(), i.value().toString());
    }

    m_process->setProcessEnvironment(processEnv);

    QStringList args = m_argv;

    debug() << "Process" << args;

    if (args.length() > 0) {
        QString program = args.takeFirst();

        m_process->start(program, args);
    }

    // TODO: what do on fail?
}

void Process::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_stdout = m_process->readAllStandardOutput();
    emit stdoutChanged(m_stdout);
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        emit cooked(OperationAttached::Error);
    } else {
        emit cooked(OperationAttached::Finished);
    }
}

void Process::onProcessError(QProcess::ProcessError err)
{
    m_stdout = m_process->readAllStandardOutput();
    emit stdoutChanged(m_stdout);
    debug() << "process error" << m_process->errorString();
    emit cooked(OperationAttached::Error);
}

void Process::onReadyReadStandardError()
{
    error() << m_process->readAllStandardError();
}
