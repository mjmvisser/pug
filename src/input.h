#ifndef INPUT_H
#define INPUT_H

#include "pugitem.h"

class Input : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Dependency dependency READ dependency WRITE setDependency NOTIFY dependencyChanged)
    Q_PROPERTY(bool lockInput READ lockInput WRITE setLockInput NOTIFY lockInputChanged)
    Q_ENUMS(Dependency)
public:
    enum Dependency { Invalid, None, Node, Element, Frame };
    explicit Input(QObject *parent = 0);
    
    Dependency dependency() const;
    void setDependency(Dependency);

    bool lockInput() const;
    void setLockInput(bool);

    bool ignoreLocked() const;
    void setIgnoreLocked(bool);

signals:
    void dependencyChanged(Dependency dependency);
    void lockInputChanged(bool lockInput);
    
private:
    Dependency m_dependency;
    bool m_lockInputFlag;
    bool m_ignoreLockedFlag;
};

#endif // INPUT_H
