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
    Q_PROPERTY(bool updating READ isUpdating NOTIFY updatingChanged)
    Q_PROPERTY(bool cooking READ isCooking NOTIFY cookingChanged)
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

    bool isUpdating() const;
    bool isCooking() const;

signals:
    void argvChanged(const QStringList argv);
    void envChanged();
    void stdinChanged(const QString stdin);
    void ignoreExitCodeChanged(bool ignoreExitCodeChanged);
    void updatingChanged(bool updating);
    void cookingChanged(bool cooking);

    void updateAtIndex(int index, const QVariant context);
    void updatedAtIndex(int index, int status);
    void cookAtIndex(int index, const QVariant context);
    void cookedAtIndex(int index, int status);

protected slots:
    void onUpdateAtIndex(int index, const QVariant context);
    void onCookAtIndex(int index, const QVariant context);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

    void handleFinishedProcess(QProcess *process, OperationAttached::Status status);

    virtual void componentComplete();

private:
    void setUpdating(bool);
    void setCooking(bool);
    void executeAtIndex(int index, const QVariant context);

    QStringList m_argv;
    QString m_stdin;
    QJSValue m_env;
    bool m_ignoreExitCode;
    bool m_updatingFlag;
    bool m_cookingFlag;
    QVector<QProcess *> m_processes;
    QHash<QProcess *, QString> m_stdouts;
    QHash<QProcess *, QVariantMap> m_contexts;
};

#endif // PROCESS_H
