#ifndef SUDO_H
#define SUDO_H

#include <QtQml>

#include "pugitem.h"

class Sudo : public PugItem
{
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_OBJECT
public:
    Sudo(QObject *parent = 0);

    const QString user() const;
    void setUser(const QString);

    const QString group() const;
    void setGroup(const QString);

    const QStringList wrapCommand(const QStringList command) const;

signals:
    void userChanged(const QString user);
    void groupChanged(const QString group);

private:
    QString m_user;
    QString m_group;
};

#endif
