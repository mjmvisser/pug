#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>
#include <QStringList>

#include "node.h"

class DeprecatedNode : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString temporaryDir READ temporaryDir WRITE setTemporaryDir NOTIFY temporaryDirChanged)
public:
    explicit DeprecatedNode(QObject *parent = 0);

    const QString temporaryDir() const;
    void setTemporaryDir(const QString);

    Q_INVOKABLE const QString temporaryFile(const QString templateName) const;

signals:
    void temporaryDirChanged(const QString temporaryDir);

private:
    QString m_temporaryDir;
};

Q_DECLARE_METATYPE(DeprecatedNode*) // makes available to QVariant

#endif // NODE_H
