#ifndef RELEASEOPERATION_H
#define RELEASEOPERATION_H

#include <QProcess>
#include <QQueue>

#include "operation.h"
#include "branchbase.h"
#include "field.h"
#include "fileopqueue.h"

class ReleaseOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_ENUMS(Mode)
    Q_PROPERTY(bool releasable READ isReleasable WRITE setReleasable NOTIFY releasableChanged)
    Q_PROPERTY(BranchBase *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString versionField READ versionFieldName WRITE setVersionFieldName NOTIFY versionFieldChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    enum Mode { Copy, Move };

    explicit ReleaseOperationAttached(QObject *parent = 0);

    bool isReleasable() const;
    void setReleasable(bool);

    BranchBase *target();
    const BranchBase *target() const;
    void setTarget(BranchBase *);

    const QString versionFieldName() const;
    void setVersionFieldName(const QString);

    Mode mode() const;
    void setMode(Mode);
    QString modeString() const;

    Q_INVOKABLE virtual void reset();
    Q_INVOKABLE virtual void run();

    Q_INVOKABLE const QString findVersionFieldName() const;
    Q_INVOKABLE int findLastVersion(const QVariant data) const;

signals:
    void releasableChanged(bool releasable);
    void targetChanged(BranchBase *target);
    void versionFieldChanged(const QString versionField);
    void modeChanged(Mode mode);
    void pathsChanged(QString paths);

protected:
    const QString release(const QString srcPath, const QString versionFieldName, int nextVersion);

    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onFileOpQueueFinished();
    void onFileOpQueueError();

private:
    bool m_releasableFlag;
    BranchBase *m_target;
    QString m_versionFieldName;
    Mode m_mode;
    QStringList m_paths;
    FileOpQueue* m_queue;
};

class ReleaseOperation : public Operation
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
public:
    explicit ReleaseOperation(QObject *parent = 0);

    const QString user() const;
    void setUser(const QString);

    const QString group() const;
    void setGroup(const QString);

    static ReleaseOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void userChanged(const QString user);
    void groupChanged(const QString group);

private:
    QString m_user;
    QString m_group;
};

QML_DECLARE_TYPEINFO(ReleaseOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
