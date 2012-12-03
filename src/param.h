#ifndef PARAM_H
#define PARAM_H

#include <QObject>
#include <QVariant>

#include "pugitem.h"

class Param : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    explicit Param(QObject *parent = 0);
    
    QVariant value() const;
    void setValue(const QVariant &v);

    QString text() const;
    void setText(const QString t);

signals:
    void valueChanged(const QVariant value);
    void textChanged(const QString text);
    void inputChanged(bool input);

private:
    QVariant m_value;
    QString m_text;
    QString m_help;
};
//Q_DECLARE_METATYPE(Param*)

#endif // PARAM_H
