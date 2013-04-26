#ifndef INPUT_H
#define INPUT_H

#include <QList>

#include "pugitem.h"

class Node;

class Input : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Dependency dependency READ dependency WRITE setDependency NOTIFY dependencyChanged)
    Q_ENUMS(Dependency)
public:
    enum Dependency { Invalid };
    explicit Input(QObject *parent = 0);
    
    Dependency dependency() const;
    void setDependency(Dependency);

    const QList<const Node *> nodes() const;
    const QList<Node *> nodes();

signals:
    void dependencyChanged(Dependency dependency);
    
private:
    Dependency m_dependency;
};

#endif // INPUT_H
