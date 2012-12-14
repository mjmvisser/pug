#ifndef FILEOPQUEUE_H
#define FILEOPQUEUE_H

#include <QObject>
#include <QQueue>
#include <QVariantMap>
#include <QStringList>
#include <QProcess>

#include "pugitem.h"

class FileOpQueue : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
public:
    explicit FileOpQueue(QObject *parent = 0);

    const QString user() const;
    void setUser(const QString);

    const QString group() const;
    void setGroup(const QString);

    Q_INVOKABLE void mkdir(const QString dir);
    Q_INVOKABLE void chmod(const QString mode, const QString target);
    Q_INVOKABLE void copy(const QString src, const QString dest);
    Q_INVOKABLE void move(const QString src, const QString dest);
    Q_INVOKABLE void symlink(const QString target, const QString dest);
    Q_INVOKABLE void hardlink(const QString target, const QString dest);

    Q_INVOKABLE void run();

signals:
    void finished();
    void error();
    void userChanged(const QString user);
    void groupChanged(const QString group);

protected:
    struct Item {
        enum Type { Mkdir, Chmod, Copy, Move, Symlink, Hardlink } mode;
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
    QString m_user;
    QString m_group;
    QProcess *m_process;
    QQueue<Item> m_workQueue;
};

#endif
