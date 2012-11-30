#ifndef OPERATION_H
#define OPERATION_H

#include <QtQml>
#include <QObject>
#include <QString>
#include <QVariant>

#include "pugitem.h"

class NodeBase;

class OperationAttached : public PugItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
public:
    // these are listed in order of increasing comparison precedence
    enum Status { Idle, Running, Finished, Error }; // TODO: maybe switch these to isIdle, isRunning, etc.

    explicit OperationAttached(QObject *parent = 0);

    Status status() const;
    void setStatus(Status s);

    Q_INVOKABLE virtual void reset();
    virtual void run(const QVariant) = 0;

    void execute(QVariant env);

protected slots:

signals:
    void statusChanged(Status status);
    void finished();

private:
    Status m_status;
    QVariant m_env;
};

class Operation : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList runAfter READ runAfter WRITE setRunAfter)
    Q_PROPERTY(QStringList dependsOn READ dependsOn WRITE setDependsOn)
public:
    explicit Operation(QObject *parent = 0);

    QStringList runAfter() const;
    void setRunAfter(const QStringList);

    QStringList dependsOn() const;
    void setDependsOn(const QStringList);

    //static OperationAttached *qmlAttachedProperties(QObject *); // must be defined in subclasses
signals:

private:
    QStringList m_runAfter;
    QStringList m_dependsOn;
};


#endif
