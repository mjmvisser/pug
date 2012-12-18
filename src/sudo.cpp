#include "sudo.h"

Sudo::Sudo(QObject *parent) :
    PugItem(parent)
{
}

const QString Sudo::user() const
{
    return m_user;
}

void Sudo::setUser(const QString user)
{
    if (m_user != user) {
        m_user = user;
        emit userChanged(user);
    }
}

const QString Sudo::group() const
{
    return m_group;
}

void Sudo::setGroup(const QString group)
{
    if (m_group != group) {
        m_group = group;
        emit groupChanged(group);
    }
}

const QStringList Sudo::wrapCommand(const QStringList command) const
{
    if (m_user.isEmpty() && m_group.isEmpty())
        return command;

    QStringList wrappedCommand;
    wrappedCommand << "sudo";

    if (!m_user.isEmpty())
        wrappedCommand << "-u" << m_user;

    if (!m_group.isEmpty())
        wrappedCommand << "-g" << m_group;

    wrappedCommand << command;

    return wrappedCommand;
}
