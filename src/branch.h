#ifndef BRANCHBASE_H
#define BRANCHBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QList>
#include <QString>
#include <QVariant>
#include <QFileInfoList>
#include <QMap>

#include "node.h"
#include "field.h"

class Branch : public Node
{
    Q_OBJECT
    Q_PROPERTY(Branch *root READ root WRITE setRoot NOTIFY rootChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QQmlListProperty<Field> fields READ fields_)
public:
    explicit Branch(QObject *parent = 0);

    Branch *root();
    const Branch *root() const;
    void setRoot(Branch *);

    const QString pattern() const;
    void setPattern(const QString);

    QQmlListProperty<Field> fields_();

    Branch *parentBranch();
    const Branch *parentBranch() const;

    Field *findField(const QString name);
    const Field *findField(const QString name) const;

    //bool hasField(const QString name) const;

    const QList<const Field *> fields(const QStringList fieldNameList = QStringList()) const;

    bool fieldsComplete(const QString pattern, const QVariantMap fields) const;

    Q_INVOKABLE const QVariant match(const QString pattern, const QString path, bool exact, bool partial) const;
    Q_INVOKABLE const QStringList fieldNames(const QString pattern) const;
    Q_INVOKABLE const QString formatFields(const QString pattern, const QVariant data) const;

    // TODO: rename these
    Q_INVOKABLE const QVariant parse(const QString path, bool partial=false) const;
    Q_INVOKABLE const QString map(const QVariant fields) const;

    Q_INVOKABLE const QMap<QString, QFileInfoList> listMatchingPatterns(const QVariantMap context) const;
    //Q_INVOKABLE void setPaths(const QStringList paths, const QVariantMap context);

signals:
    void rootChanged(Branch *root);
    void patternChanged(const QString pattern);

protected:
    bool exactMatch() const;
    void setExactMatch(bool f);

private:
    const QMap<QString, QFileInfoList> listMatchingPatternsHelper(const QDir, const QVariantMap, const QMap<QString, QFileInfoList>) const;
    bool containsFields(const QVariantMap& needle, const QVariantMap& haystack) const;

    Branch *m_root;
    QList<Field *> m_fields;
    QString m_pattern;
    bool m_exactMatchFlag;
};
//
//inline QDebug operator<<(QDebug dbg, const Branch *branch)
//{
//    if (!branch)
//        return dbg << "Branch(0x0) ";
//    dbg.nospace() << "Branch(" << (void*)branch;
//    if (!branch->objectName().isEmpty())
//        dbg << ", name = " << branch->objectName();
//    if (!branch->pattern().isEmpty())
//        dbg << ", pattern = " << branch->pattern();
//    dbg << ')';
//    return dbg.space();
//}


#endif // BRANCHBASE_H
