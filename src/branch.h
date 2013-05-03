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

class Branch : public Node
{
    Q_OBJECT
    Q_PROPERTY(Branch *root READ root WRITE setRoot NOTIFY rootChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
public:
    explicit Branch(QObject *parent = 0);

    Branch *root();
    const Branch *root() const;
    void setRoot(Branch *);

    const QString pattern() const;
    void setPattern(const QString);

    Branch *parentBranch();
    const Branch *parentBranch() const;

    bool fieldsComplete(const QVariantMap context) const;

    // TODO: rename these
    Q_INVOKABLE const QVariant parse(const QString path, bool partial=false) const;
    Q_INVOKABLE const QString map(const QVariant fields) const;

    Q_INVOKABLE const QMap<QString, QSet<QFileInfo> > listMatchingPatterns(const QVariantMap context) const;
    //Q_INVOKABLE void setPaths(const QStringList paths, const QVariantMap context);

signals:
    void rootChanged(Branch *root);
    void patternChanged(const QString pattern);

protected:
    bool exactMatch() const;
    void setExactMatch(bool f);

private:
    const QMap<QString, QSet<QFileInfo> > listMatchingPatternsHelper(const QDir, const QVariantMap, const QMap<QString, QSet<QFileInfo> >) const;
    bool containsFields(const QVariantMap& needle, const QVariantMap& haystack) const;

    Branch *m_root;
    QString m_pattern;
    bool m_exactMatchFlag;
};
Q_DECLARE_METATYPE(Branch*) // makes available to QVariant

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
