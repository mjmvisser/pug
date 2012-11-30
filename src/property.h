#ifndef PROPERTY_H
#define PROPERTY_H

#include <QObject>
#include <QVariant>
#include <QQmlListProperty>

#include "propertybase.h"

class Property : public PropertyBase
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool input READ isInput WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(bool output READ isOutput WRITE setOutput NOTIFY outputChanged)
public:
    explicit Property(QObject *parent = 0);
    
    QVariant value() const;
    void setValue(const QVariant &v);

    QString text() const;
    void setText(const QString t);

    bool isInput() const;
    void setInput(bool i);

    bool isOutput() const;
    void setOutput(bool o);

signals:
    void valueChanged(const QVariant value);
    void textChanged(const QString text);
    void inputChanged(bool input);
    void outputChanged(bool output);

private:
    QVariant m_value;
    QString m_text;
    QString m_help;
    bool m_input;
    bool m_output;
};
//Q_DECLARE_METATYPE(Property*)

#endif // PROPERTY_H
