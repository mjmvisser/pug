#ifndef RELEASEOPERATION_H
#define RELEASEOPERATION_H

#include <QProcess>
#include <QQueue>
#include <QJSValue>

#include "operation.h"
#include "branch.h"
#include "node.h"
#include "field.h"
#include "fileopqueue.h"

class ReleaseOperationAttached : public OperationAttached
{
    Q_OBJECT
    Q_ENUMS(Mode)
    Q_PROPERTY(Node *source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Branch *versionBranch READ versionBranch WRITE setVersionBranch NOTIFY versionBranchChanged)
    Q_PROPERTY(QString versionField READ versionFieldName WRITE setVersionFieldName NOTIFY versionFieldChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    enum Mode { Copy, Move };

    explicit ReleaseOperationAttached(QObject *parent = 0);

    Node *source();
    const Node *source() const;
    void setSource(Node *);

    Branch *versionBranch();
    const Branch *versionBranch() const;
    void setVersionBranch(Branch *);

    const QString versionFieldName() const;
    void setVersionFieldName(const QString);

    Mode mode() const;
    void setMode(Mode);

    Sudo *sudo();

    Q_INVOKABLE const QString findVersionFieldName() const;
    Q_INVOKABLE int findLastVersion(const QVariantMap) const;

signals:
    void sourceChanged(Node *source);
    void versionBranchChanged(Branch *branch);
    void versionFieldChanged(const QString versionField);
    void modeChanged(Mode mode);

protected:
    virtual const QMetaObject *operationMetaObject() const;

private slots:
    void onPrepare();

private:
    Node *m_source;
    Branch *m_versionBranch;
    QString m_versionFieldName;
    Mode m_mode;
};

class ReleaseOperation : public Operation
{
    Q_OBJECT
    Q_PROPERTY(Sudo *sudo READ sudo WRITE setSudo NOTIFY sudoChanged)
public:
    explicit ReleaseOperation(QObject *parent = 0);

    Sudo *sudo();
    void setSudo(Sudo *);    

    static ReleaseOperationAttached *qmlAttachedProperties(QObject *);

signals:
    void sudoChanged(Sudo *sudo);

private:
    Sudo *m_sudo;
};

QML_DECLARE_TYPEINFO(ReleaseOperation, QML_HAS_ATTACHED_PROPERTIES)

#endif
