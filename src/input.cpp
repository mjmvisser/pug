#include "input.h"

Input::Input(QObject *parent) :
    PugItem(parent),
    m_dependency(Input::Invalid)
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
