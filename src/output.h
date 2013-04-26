#ifndef OUTPUT_H
#define OUTPUT_H

#include "pugitem.h"

class Node;

class Output : public PugItem
{
    Q_OBJECT
public:
    explicit Output(QObject *parent = 0);

    const QList<const Node *> nodes() const;
    const QList<Node *> nodes();

signals:
    
public slots:
    
};

#endif // OUTPUT_H
