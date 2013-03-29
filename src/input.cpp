#include "input.h"

Input::Input(QObject *parent) :
    PugItem(parent),
    m_dependency(Input::Invalid),
    m_lockInputFlag(false),
    m_ignoreLockedFlag(false)
{
}

Input::Dependency Input::dependency() const
{
    return m_dependency;
}

void Input::setDependency(Input::Dependency d)
{
    if (m_dependency != d) {
        m_dependency = d;
        emit dependencyChanged(d);
    }
}

bool Input::lockInput() const
{
    return m_lockInputFlag;
}

void Input::setLockInput(bool f)
{
    if (m_lockInputFlag != f) {
        m_lockInputFlag = f;
        emit lockInputChanged(f);
    }
}

bool Input::ignoreLocked() const
{
    return m_ignoreLockedFlag;
}

void Input::setIgnoreLocked(bool f)
{
    m_ignoreLockedFlag = f;
}
