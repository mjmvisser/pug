#ifndef BRANCHBASE_H
#define BRANCHBASE_H

#include <QObject>
#include <QQmlListProperty>
#include <QList>
#include <QString>
#include <QVariant>

#include "nodebase.h"
#include "field.h"
#include "element.h"

class BranchBase : public NodeBase
{
    Q_OBJECT
    Q_PROPERTY(BranchBase *root READ root WRITE setRoot NOTIFY rootChanged)
    Q_PROPERTY(QQmlListProperty<Element> elements READ elements_ NOTIFY elementsChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QQmlListProperty<Field> fields READ fields_)
public:
    explicit BranchBase(QObject *parent = 0);

    BranchBase *root();
    const BranchBase *root() const;
    void setRoot(BranchBase *);

    QQmlListProperty<Field> fields_();

    QQmlListProperty<Element> elements_();

    QList<Element *>& elements();
    const QList<const Element *>& elements() const;

    void clearElements();
    void addElement(Element *);

    void setPaths(const QStringList);

    const QString pattern() const;
    void setPattern(const QString);

    bool exactMatch() const;
    void setExactMatch(bool f);

    BranchBase *parentBranch();
    const BranchBase *parentBranch() const;

    Field *findField(const QString name);
    const Field *findField(const QString name) const;

    const QList<const Field *> fields(const QStringList fieldNameList = QStringList()) const;

    bool areFieldsComplete(const QString pattern, const QVariantMap fields) const;

    Q_INVOKABLE const QVariant match(const QString pattern, const QString path, bool exact) const;
    Q_INVOKABLE const QVariant match(const QString pattern, const QString path) const;
    Q_INVOKABLE const QStringList fieldNames(const QString pattern) const;
    Q_INVOKABLE const QString formatFields(const QString pattern, const QVariant data) const;

    // TODO: rename these
    Q_INVOKABLE const QVariant parse(const QString path) const;
    Q_INVOKABLE const QString map(const QVariant fields) const;

    Q_INVOKABLE const QStringList listMatchingPaths(const QVariant data) const;

signals:
    void update(const QVariant env);
    void updated(int status);

    void rootChanged(BranchBase *root);
    void elementsChanged();
    void patternChanged(const QString pattern);

private:
    const QStringList listMatchingPathsHelper(const QDir parentDir, const QVariantMap fields) const;
    bool containsFields(const QVariantMap& needle, const QVariantMap& haystack) const;

    BranchBase *m_root;
    QList<Field *> m_fields;
    QList<Element *> m_elements;
    QString m_pattern;
    bool m_exactMatchFlag;
};
Q_DECLARE_METATYPE(BranchBase*) // makes available to QVariant

#endif // BRANCHBASE_H
