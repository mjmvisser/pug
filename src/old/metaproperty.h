#ifndef METAPROPERTY_H
#define METAPROPERTY_H

#include "pugitem.h"

class MetaProperty : public PugItem
{
    Q_OBJECT
public:
    explicit MetaProperty(QObject *parent = 0);
};

#endif // METAPROPERTY_H
