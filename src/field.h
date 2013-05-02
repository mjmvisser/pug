#ifndef FIELD_H
#define FIELD_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "pugitem.h"

class Field : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QString env READ env WRITE setEnv NOTIFY envChanged)
    Q_PROPERTY(QString regexp READ regexp WRITE setRegexp NOTIFY regexpChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QStringList values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString formatSpec READ formatSpec WRITE setFormatSpec NOTIFY formatSpecChanged)
    Q_ENUMS(Type)
public:
    enum Type { String, Integer, Date, DateTime };

    explicit Field(QObject *parent = 0);
    
    const QString env() const;
    void setEnv(const QString);

    const QString regexp() const;
    void setRegexp(const QString);

    Type type() const;
    void setType(Type);

    int width() const;
    void setWidth(int);

    const QStringList values() const;
    void setValues(const QStringList);

    const QString defaultValue() const;
    void setDefaultValue(const QString);

    const QString formatSpec() const;
    void setFormatSpec(const QString);

    Q_INVOKABLE const QVariant parse(const QString) const;
    Q_INVOKABLE const QVariant get(const QVariantMap) const;
    Q_INVOKABLE const QString format(const QVariant) const;

    static const QStringList fieldNames(const QString pattern);

signals:
    void envChanged(const QString env);
    void regexpChanged(const QString regexp);
    void typeChanged(Type type);
    void widthChanged(int width);
    void valuesChanged(const QStringList values);
    void defaultValueChanged(const QString defaultValue);
    void formatSpecChanged(const QString formatSpec);

private:
    QString m_env;
    QString m_regexp;
    int m_width;
    Type m_type;
    QStringList m_values;
    QString m_defaultValue;
    QString m_formatSpec;
};
Q_DECLARE_METATYPE(Field*)

#endif // FIELD_H
