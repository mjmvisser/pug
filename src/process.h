#ifndef PROCESS_H
#define PROCESS_H

#include <QVariant>
#include <QString>
#include <QProcess>
#include <QVector>

#include "node.h"
#include "operation.h"

class Process : public Node
{
    Q_OBJECT
    Q_PROPERTY(QStringList argv READ argv WRITE setArgv NOTIFY argvChanged)
    Q_PROPERTY(QString stdin READ stdin WRITE setStdin NOTIFY stdinChanged)
    Q_PROPERTY(QJSValue env READ env WRITE setEnv NOTIFY envChanged)
    Q_PROPERTY(bool ignoreExitCode READ ignoreExitCode WRITE setIgnoreExitCode NOTIFY ignoreExitCodeChanged)
public:
    explicit Process(QObject *parent = 0);
    
    const QStringList argv() const;
    void setArgv(const QStringList);

    const QString stdin() const;
    void setStdin(const QString);

    QJSValue env();
    void setEnv(const QJSValue);

    bool ignoreExitCode() const;
    void setIgnoreExitCode(bool);

    Q_INVOKABLE const QString stdout(int index) const;

signals:
    void argvChanged(const QStringList argv);
    void envChanged();
    void stdinChanged(const QString stdin);
    void ignoreExitCodeChanged(bool ignoreExitCodeChanged);
    void cookAtIndex(int index, const QVariant context);
    void cookedAtIndex(int index, int status);

protected slots:
    void onCookAtIndex(int index, const QVariant context);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError);
    void onReadyReadStandardError();

    void handleFinishedProcess(QProcess *process, OperationAttached::Status status);

    virtual void componentComplete();

private:
    QStringList m_argv;
    QString m_stdin;
    QJSValue m_env;
    bool m_ignoreExitCode;
    QVector<QProcess *> m_processes;
};
//Q_DECLARE_METATYPE(Node*) // makes available to QVariant

#endif // PROCESS_H
