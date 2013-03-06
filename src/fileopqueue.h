#ifndef FILEOPQUEUE_H
#define FILEOPQUEUE_H

#include <QObject>
#include <QQueue>
#include <QVariantMap>
#include <QStringList>
#include <QProcess>

#include "pugitem.h"
#include "sudo.h"

class FileOpQueue : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Sudo *sudo READ sudo WRITE setSudo NOTIFY sudoChanged)
public:
    explicit FileOpQueue(QObject *parent = 0);

    Sudo *sudo();
    void setSudo(Sudo *);

    Q_INVOKABLE void mkdir(const QString dir);
    Q_INVOKABLE void chmod(const QString mode, const QString target);
    Q_INVOKABLE void chown(const QString user, const QString group, const QString target);
    Q_INVOKABLE void copy(const QString src, const QString dest);
    Q_INVOKABLE void move(const QString src, const QString dest);
    Q_INVOKABLE void symlink(const QString target, const QString dest);
    Q_INVOKABLE void hardlink(const QString target, const QString dest);

    Q_INVOKABLE void run();

signals:
    void sudoChanged(Sudo *sudo);

    void finished();
    void error();

protected:
    struct Item {
        enum Type { Mkdir, Chmod, Chown, Copy, Move, Symlink, Hardlink } mode;
        Item(Type t) : type(t) {};
        Item(Type t, const QString arg1) : type(t) { args << arg1; }
        Item(Type t, const QString arg1, const QString arg2) : type(t) { args << arg1 << arg2; }
        Type type;
        QStringList args;
    };

    void continueRunning();

private slots:
    void onProcessFinished(int, QProcess::ExitStatus);
    void onProcessError(QProcess::ProcessError);

private:
    QProcess *m_process;
    QQueue<Item> m_workQueue;
    Sudo *m_sudo;
};

#endif
