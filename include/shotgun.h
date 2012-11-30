#ifndef SHOTGUN_H
#define SHOTGUN_H

#include <QtQml>
#include <QObject>

class ShotgunAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString entity READ entity WRITE setEntity)
    Q_PROPERTY(QString field READ field WRITE setField)
public:
    ShotgunAttached(QObject *object);

    QString entity() const;
    void setEntity(const QString e);
    QString field() const;
    void setField(const QString f);

private:
    QString m_entity;
    QString m_field;
};

class Shotgun : public QObject
{
    Q_OBJECT
public:
    Shotgun(QObject *parent = 0);

    static ShotgunAttached *qmlAttachedProperties(QObject *);
private:
};

QML_DECLARE_TYPEINFO(Shotgun, QML_HAS_ATTACHED_PROPERTIES)

#endif
