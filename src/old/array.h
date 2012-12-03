#ifndef ARRAY_H
#define ARRAY_H

#include <QObject>
#include <QQmlListProperty>

#include "metaproperty.h"

class Array : public Param
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Param> contents READ contents)
    Q_PROPERTY(int index READ index)
    Q_CLASSINFO("DefaultProperty", "contents")
public:
    explicit Array(QObject *parent = 0);

    QQmlListProperty<Param> contents_();

signals:

public slots:

private:
    int index() const;

    QList<Param *> m_contents;
};

#endif // ARRAY_H
