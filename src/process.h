#ifndef PROCESS_H
#define PROCESS_H

#include <QVariant>
#include <QString>
#include <QProcess>

#include "node.h"
#include "operation.h"

class Process : public NodeBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList argv READ argv WRITE setArgv)
    Q_PROPERTY(QString stdout READ stdout NOTIFY stdoutChanged)
public:
    explicit Process(QObject *parent = 0);
    
    QStringList argv();
    void setArgv(QStringList);

    const QString stdout() const;

signals:
    void argvChanged(const QStringList argv);
    void cook(const QVariant env);
    void cooked(int status);
    void stdoutChanged(const QString stdout);

protected slots:
    void onCook(const QVariant env);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError);
    void onReadyReadStandardError();

private:
    QStringList m_argv;
    QProcess* m_process;
    QString m_stdout;
};
//Q_DECLARE_METATYPE(Node*) // makes available to QVariant

#endif // PROCESS_H
