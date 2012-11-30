#ifndef PARM_H
#define PARM_H

#include <QObject>
#include <QVariant>
#include <QQmlListProperty>

#include "propertybase.h"

class Property : public PropertyBase
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    explicit Property(QObject *parent = 0);
    
    QVariant value() const;
    void setValue(const QVariant &v);

    QString text() const;
    void setText(const QString t);

private:
    QVariant m_value;
    QString m_text;
    QString m_help;
};
Q_DECLARE_METATYPE(Property*)

#endif // PARM_H
