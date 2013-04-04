#ifndef INPUT_H
#define INPUT_H

#include "pugitem.h"

class Input : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Dependency dependency READ dependency WRITE setDependency NOTIFY dependencyChanged)
    Q_ENUMS(Dependency)
public:
    enum Dependency { Invalid, None, Node, Element, Frame };
    explicit Input(QObject *parent = 0);
    
    Dependency dependency() const;
    void setDependency(Dependency);

signals:
    void dependencyChanged(Dependency dependency);
    
private:
    Dependency m_dependency;
};

#endif // INPUT_H
