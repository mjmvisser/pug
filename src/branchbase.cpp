#include <QDebug>
#include <QQmlProperty>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include <QSet>

#include "branchbase.h"
#include "root.h"
#include "field.h"
#include "property.h"

BranchBase::BranchBase(QObject *parent) :
    NodeBase(parent),
    m_root()
{
    Property *rootProperty = new Property(this);
    rootProperty->setName("root");
    rootProperty->setInput(true);

    Property *patternProperty = new Property(this);
    patternProperty->setName("pattern");
}

const QList<const Field *> BranchBase::fields(const QStringList fieldNameList) const
{
    QList<const Field *> results;

    // add our list of field names to those passed in
    QStringList fnames = fieldNameList;
    fnames.append(fieldNames(pattern()));

    // parent fields
    const BranchBase *s = root();
    if (s) {
        results.append(s->fields(fnames));
    }

    // our fields
    foreach (const QObject *o, children()) {
        const Field *field = qobject_cast<const Field *>(o);
        if (field && fnames.contains(field->name()))
            results.append(field);
    }

    return results;
}

const QList<Field *> BranchBase::fields(const QStringList fieldNameList)
{
    QList<Field *> results;

    // add our list of field names to those passed in
    QStringList fnames = fieldNameList;
    fnames.append(fieldNames(pattern()));

    // parent fields
    BranchBase *s = root();
    if (s) {
        results.append(s->fields(fnames));
    }

    // our fields
    foreach (QObject *o, children()) {
        Field *field = qobject_cast<Field *>(o);
        if (field && fnames.contains(field->name()))
            results.append(field);
    }

    return results;
}

const BranchBase *BranchBase::root() const
{
    if (m_root) {
        return m_root;
    } else {
        const BranchBase *p = parentBranch();
        return p && !p->isRoot() ? p : 0;
    }
}

BranchBase *BranchBase::root()
{
    if (m_root) {
        return m_root;
    } else {
        BranchBase *p = parentBranch();
        return p && !p->isRoot() ? p : 0;
    }
}

void BranchBase::setRoot(BranchBase *branch)
{
    if (m_root != branch) {
        m_root = branch;
        emit rootChanged(m_root);
    }
}

QQmlListProperty<Element> BranchBase::elements_()
{
    return QQmlListProperty<Element>(this, m_elements);
}

const QList<const Element *>& BranchBase::elements() const
{
    // hard to believe you have to do this in modern C++
    return reinterpret_cast<const QList<const Element *> &>(m_elements);
}

QList<Element *>& BranchBase::elements()
{
    return m_elements;
}

void BranchBase::clearElements()
{
    if (!m_elements.isEmpty()) {
        while (!m_elements.isEmpty())
            m_elements.takeFirst()->deleteLater();
        emit elementsChanged();
    }
}

void BranchBase::addElement(Element *e)
{
    m_elements.append(e);
    emit elementsChanged();
}

void BranchBase::setPaths(const QStringList paths)
{
    m_elements.clear();

    foreach (const QString path, paths) {
        bool found = false;
        foreach (Element *element, m_elements) {
            if (element->matches(path)) {
                element->append(path);
                found = true;
                break;
            }
        }
        if (!found) {
            Element *element = new Element(this);
            element->setPattern(path);
            if (element->hasFrames())
                element->append(path);
            m_elements.append(element);
        }
    }

    emit elementsChanged();
}

const QString BranchBase::pattern() const
{
    return m_pattern;
}

void BranchBase::setPattern(const QString p)
{
    if (m_pattern != p) {
        m_pattern = p;
        emit patternChanged(m_pattern);
    }
}

bool BranchBase::exactMatch() const
{
    return m_exactMatchFlag;
}

void BranchBase::setExactMatch(bool f)
{
    m_exactMatchFlag = f;
}

BranchBase *BranchBase::parentBranch()
{
    return firstParent<BranchBase>();
}

const BranchBase *BranchBase::parentBranch() const
{
    return firstParent<BranchBase>();
}

Field *BranchBase::findField(const QString name)
{
    // check our fields first
    foreach (QObject *o, children()) {
        Field *field = qobject_cast<Field *>(o);
        if (field && field->name() == name) {
            return field;
        }
    }

    // check our parent input
    BranchBase *p = parentBranch();
    if (p) {
        return p->findField(name);
    }

    return 0;
}

const Field *BranchBase::findField(const QString name) const
{
    // check our fields first
    foreach (const QObject *o, children()) {
        const Field *field = qobject_cast<const Field *>(o);
        if (field && field->name() == name) {
            return field;
        }
    }

    // check our parent input
    const BranchBase *p = parentBranch();
    if (p) {
        return p->findField(name);
    }

    return 0;
}

const QStringList BranchBase::fieldNames(const QString pattern) const
{
    QStringList results;
    static const QRegularExpression re("\\{(\\w+)\\}");

    QRegularExpressionMatchIterator it = re.globalMatch(pattern);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        results.append(match.captured(1));
    }

    return results;
}

static const QString escapeSpecialCharacters(const QString r)
{
    QString result = r;
    result.replace('.', "\\.");

    return result;
}

const QVariant BranchBase::match(const QString pattern, const QString path, bool exact) const
{
    static const QRegularExpression replaceFieldsRegexp("\\{(\\w+)\\}");

    const QString escapedPattern = escapeSpecialCharacters(pattern);
    QRegularExpressionMatchIterator it = replaceFieldsRegexp.globalMatch(escapedPattern);

    // adapted from QString::replace code

    int lastEnd = 0;
    // assemble a regular expression string
    QString matchRegexpStr = "^";
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        // add the part before the match
        int len = match.capturedStart() - lastEnd;
        if (len > 0) {
            matchRegexpStr += escapedPattern.mid(lastEnd, len);
        }

        // add "(" field.regexp ")"
        QString fieldName = match.captured(1);
        const Field *f = findField(fieldName);
        if (f) {
            matchRegexpStr += "(?<";
            matchRegexpStr += fieldName;
            matchRegexpStr += ">";
            if (f->values().length() == 1) {
                // exactly one value -- only match that
                matchRegexpStr += QRegularExpression::escape(f->values().at(0));
            } else {
                // substitute the field's regexp
                matchRegexpStr += f->regexp();
            }
            matchRegexpStr += ")";
        } else {
            qWarning() << ".match couldn't find field " << fieldName;
        }

        lastEnd = match.capturedEnd();
    }

    // trailing string after the last match
    if (escapedPattern.length() > lastEnd) {
        matchRegexpStr += escapedPattern.mid(lastEnd);
    }

    if (exact)
        matchRegexpStr += "$";

    copious() << ".match with" << matchRegexpStr;

    QRegularExpression matchRegexp(matchRegexpStr);

    QRegularExpressionMatch match = matchRegexp.match(path);

    copious() << ".match got" << match;

    if (match.hasMatch()) {
        QVariantMap fields;

        foreach (QString fieldName, fieldNames(pattern)) {
            const Field *f = findField(fieldName);
            if (f) {
                fields[fieldName] = f->parse(match.captured(fieldName));
            } else {
                fields[fieldName] = QVariant();
            }
        }

        QString remainder;
        if (match.capturedEnd() >= 0) {
            remainder = path.mid(match.capturedEnd());
        } else {
            // annoying inconsistency (bug?) -- if no capture groups, there is no implicit group 0
            remainder = path.mid(pattern.length());
        }

        if (remainder.length() > 0) {
            if (!exact)
                fields["_"] = remainder;
            else
                return QVariant();
        }

        return fields;
    }

    return QVariant();
}

const QVariant BranchBase::match(const QString pattern, const QString path) const
{
    return match(pattern, path, exactMatch());
}

const QString BranchBase::formatFields(const QString pattern, const QVariant data) const
{
    QVariantMap fields;
    if (data.isValid() && data.type() == QVariant::Map)
        fields = data.toMap();
    else
        return QString();

    static QRegularExpression re("\\{(\\w+)\\}");

    QString result;
    QRegularExpressionMatchIterator it = re.globalMatch(pattern);
    int lastEnd = 0;
    int index = 0;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        // add the part before the match
        int len = match.capturedStart() - lastEnd;
        if (len > 0) {
            result += pattern.mid(lastEnd, len);
        }

        QString fieldName = match.captured(1);
        const Field *f = findField(fieldName);
        if (f) {
            QVariant value = f->get(fields);
            if (value.isValid())
                result += f->format(value);
            else
                error() << ".formatFields couldn't find value for field" << fieldName;
        } else {
            warning() << ".formatFields couldn't find field" << fieldName;
        }

        lastEnd = match.capturedEnd();
        index++;
    }

    // trailing string after the last match
    if (pattern.length() > lastEnd) {
        result += pattern.mid(lastEnd);
    }

    return result;
}

const QVariant BranchBase::parse(const QString path) const
{
    copious() << "parse" << path;
    const BranchBase *rootBranch = qobject_cast<const BranchBase *>(root());
    if (rootBranch) {
        // recurse
        QVariant data = rootBranch->parse(path);

        if (data.isValid()) {
            QVariantMap fields = data.toMap();
            if (fields.contains("_")) {
                QString remainder = fields.take("_").toString();

                copious() << ".parse got" << fields << remainder;

                // match the remainder
                const QVariant ourData = match(m_pattern, remainder);

                copious() << ".parse matched" << ourData;

                if (ourData.isValid()) {
                    const QVariantMap ourFields = ourData.toMap();
                    // merge root fields with our own
                    for (QVariantMap::const_iterator i = ourFields.constBegin(); i != ourFields.constEnd(); ++i) {
                        fields[i.key()] = i.value();
                    }
                    // return the data
                    return fields;
                }
            }

        }
    } else {
        // base case
        return match(m_pattern, path);
    }
    return QVariant();
}

const QString BranchBase::map(const QVariant fields) const
{
    copious() << ".map" << fields;
    if (fields.isValid() && fields.type() == QVariant::Map && containsFields(fieldNames(m_pattern), fields.toMap().keys())) {
        QString mappedParent;
        const BranchBase *rootBranch = qobject_cast<const BranchBase *>(root());
        // recurse
        if (rootBranch)
            mappedParent = rootBranch->map(fields);

        return mappedParent + formatFields(m_pattern, fields.toMap());
    }

    return QString();
}

const QStringList BranchBase::listMatchingPaths(const QVariant data) const
{
    copious() << ".listMatchingPaths" << QJsonDocument::fromVariant(data);
    QStringList result;

    QString path = map(data);

    if (!path.isEmpty()) {
        QFileInfo pathInfo(path);
        if (!pathInfo.isHidden()) {
            if ((pathInfo.isDir() && !m_exactMatchFlag) ||
                (pathInfo.isFile() && m_exactMatchFlag)) {
                result << path;
                return result;
            }
        }
    }

    if (root()) {
        QString parentPath = root()->map(data);

        if (!parentPath.isEmpty()) {
            QDir parentDir(parentPath);
            if (parentDir.isAbsolute()) {
                result = listMatchingPathsHelper(parentDir, data.toMap());
            } else {
                qWarning() << ".map returned a relative path" << parentPath;
            }
        } else {
            qWarning() << ".map returned an empty path!";
        }
    }
    return result;
}

bool BranchBase::containsFields(const QVariantMap& needle, const QVariantMap& haystack) const
{
    // return true if all fields in needle are contained in haystack
    foreach (const QString fieldName, fieldNames(m_pattern)) {
        if (needle.contains(fieldName)) {
            if (!haystack.contains(fieldName) ||
                    (needle[fieldName] != haystack[fieldName])) {
                return false;
            }
        }
    }

    return true;
}

bool BranchBase::containsFields(const QStringList& needle, const QStringList& haystack) const
{
    QSet<QString> needleSet = QSet<QString>::fromList(needle);
    QSet<QString> haystackSet = QSet<QString>::fromList(haystack);

    return haystackSet.contains(needleSet);
}

const QStringList BranchBase::listMatchingPathsHelper(const QDir parentDir, const QVariantMap fields) const
{
    Q_ASSERT(!parentDir.isRoot());
    copious() << ".listMatchingPathsHelper" << parentDir << QJsonDocument::fromVariant(fields);
    QStringList result;
    QFileInfoList entries = parentDir.entryInfoList();
    foreach (QFileInfo entry, entries) {
        if (!entry.isHidden()) {
            copious() << "checking" << entry.absoluteFilePath();
            if (entry.isDir()) {
                if (!m_exactMatchFlag) {
                    QVariant entryFields = parse(entry.absoluteFilePath() + "/");
                    // if we've got an exact match, the "_" entry will be invalid
                    if (entryFields.isValid() && containsFields(fields, entryFields.toMap()) && !entryFields.toMap()["_"].isValid()) {
                        result.append(entry.absoluteFilePath() + "/");
                    }
                }
                result.append(listMatchingPathsHelper(entry.absoluteFilePath(), fields));

            } else if (m_exactMatchFlag) {
                QVariant entryFields = parse(entry.absoluteFilePath());
                if (entryFields.isValid() && containsFields(fields, entryFields.toMap())) {
                    result.append(entry.absoluteFilePath());
                }
            }
        }
    }

    return result;
}
