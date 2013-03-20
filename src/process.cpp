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

void Process::onCookAtIndex(int i, const QVariant context)
{
    trace() << ".onCookAtIndex(" << i << "," << context << ")";
    debug() << "intially details are:" << details();

    setIndex(i);

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
        emit cookedAtIndex(i, OperationAttached::Error);
    }
}

void Process::handleFinishedProcess(QProcess *process, OperationAttached::Status status)
{
    trace() << ".handleFinishedProcess(" << process << "," << status << ")";
    Q_ASSERT(process);

    int i = m_processes.indexOf(process);
    Q_ASSERT(i >= 0);

    QString stdout = m_stdouts.take(process);

    debug() << "stdout:" << stdout;

    QVariantMap context = m_contexts.take(process);
//    if (!parseDetails(stdout, context)) {
        setDetail(i, "context", toScriptValue(context));
        setDetail(i, "process", "stdout", toScriptValue(stdout));
        setDetail(i, "process", "exitCode", toScriptValue(process->exitCode()));
    //}

    m_processes[i] = 0;
    process->deleteLater();

    emit detailsChanged();
    emit cookedAtIndex(i, status);
}

//bool Process::parseDetails(const QString stdout, const QVariantMap context)
//{
//    trace() << ".parseDetails(" << stdout << "," << context << ")";
//    QRegularExpression regex("^begin-json details$(.*)^====$",
//            QRegularExpression::DotMatchesEverythingOption|
//            QRegularExpression::MultilineOption);
//
//    QRegularExpressionMatch match = regex.match(stdout);
//
//    if (match.hasMatch()) {
//        QJsonDocument json = QJsonDocument::fromJson(match.captured(1).trimmed().toUtf8());
//        if (json.isArray()) {
//            QVariantList l = json.toVariant().toList();
//
//            ElementManagerAttached *elementManagerAttached = attachedPropertiesObject<ElementManager, ElementManagerAttached>();
//            Q_ASSERT(elementManagerAttached);
//
//            int index = e;
//            foreach (const QVariant v, l) {
//                QVariantMap m = v.toMap();
//
//                QMapIterator<QString, QVariant> i(m);
//                while (i.hasNext()) {
//                    i.next();
//                    if (i.key() == "element") {
//                        Element *element = new Element(this);
//                        element->setPattern(i.value().toMap()["pattern"].toString());
//
//                        // TODO: frames
//
//                        setDetail(index, i.key(), newQObject(element));
//                    } else if (i.key() == "context") {
//                        QVariantMap detailContext = m["context"].toMap();
//                        // merge contexts
//                        QMapIterator<QString, QVariant> i(context);
//                        while (i.hasNext()) {
//                            i.next();
//                            // don't overwrite
//                            if (!detailContext.contains(i.key()))
//                                detailContext.insert(i.key(), i.value());
//                        }
//                        setDetail(index, i.key(), toScriptValue(detailContext));
//                    } else {
//                        setDetail(index, i.key(), toScriptValue(i.value()));
//                    }
//                }
//                index++;
//            }
//
//            return true;
//        } else {
//            error() << "json written to stdout is not an array";
//            return false;
//        }
//    } else {
//        // no json matched
//        debug() << "no json matched from" << stdout;
//        return false;
//    }
//}

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
