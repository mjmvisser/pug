#include <QDebug>
#include <QQmlProperty>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDir>

#include "branchbase.h"
#include "root.h"
#include "field.h"
#include "param.h"

/*!
    \class BranchBase
    \inmodule Pug
*/
/*!
    \qmltype BranchBase
    \instantiates BranchBase
    \inqmlmodule Pug
    \brief The BranchBase class is the base class for files and directories ("branches").

    BranchBase is the base class of Branch and File, which represent directories and files,
    respectively. Branches have a root() that indicates their filesystem parent. If the root
    is not set, it is assumed to be the first parent of type BranchBase.

    For example, consider the following example:
    \qml
        import Pug 1.0

        Root {
            Branch {
                id: topLevel

                Branch {
                    id: first
                }

                Branch {
                    id: second
                    root: first

                    File {
                        id: third
                    }
                }
            }
        }
    \endqml
    In this object tree, the root of third is second, the root of second is first, and the root of first
    is topLevel.

    The pattern property contains a string that is used to match, parse or format paths and data from
    a map of field names to field values. The pattern may reference fields using curly braces, i.e.:
    \qml
        Branch {
            pattern: "/prod/projects/{PROJECT}/shots/{SEQUENCE}_{SHOT}"
        }
    \endqml

    The path or paths matched by the pattern are stored in the elements property. Each element is an
    instance of Element. Each element also has a data property that contains the fields represented
    by the element's path.
*/

BranchBase::BranchBase(QObject *parent) :
    NodeBase(parent),
    m_root()
{
    addInput("root");
    addParam("pattern");
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
    foreach (const Field *f, m_fields) {
        if (fnames.contains(f->name()))
            results.append(f);
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

QQmlListProperty<Field> BranchBase::fields_()
{
    return QQmlListProperty<Field>(this, m_fields);
}

const Element *BranchBase::elementAt(int index) const
{
    if (index < details().length())
        return details().at(index).toMap().value("element").value<Element *>();
    else
        return 0;
}

Element *BranchBase::elementAt(int index)
{
    if (index < details().length())
        return details().at(index).toMap().value("element").value<Element *>();
    else
        return 0;
}

const QVariantMap BranchBase::envAt(int index) const
{
    if (index < details().length())
        return details().at(index).toMap().value("env").value<QVariantMap>();
    else
        return QVariantMap();
}

void BranchBase::setEnvAt(int index, const QVariantMap env)
{
    if (index < details().length()) {
        QVariantMap detail = details().at(index).toMap();
        detail.insert("env", env);
        setDetail(index, detail);
    }
}

void BranchBase::setPaths(const QStringList paths, const QVariantMap env)
{
    clearDetails();

    int index = 0;
    foreach (const QString path, paths) {
        bool found = false;

        for (int i = 0; i < details().count(); i++) {
            Element *element = elementAt(i);
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
            QVariantMap elementEnv = parse(element->pattern()).toMap();

            // merge input env
            QMapIterator<QString, QVariant> i(env);
            while (i.hasNext()) {
                i.next();
                // don't overwrite
                if (!elementEnv.contains(i.key()))
                    elementEnv.insert(i.key(), i.value());
            }

            setDetail(index, "element", QVariant::fromValue(element));
            setDetail(index, "env", elementEnv);
            index++;
        }
    }
    emit detailsChanged();
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
    foreach (Field *field, m_fields) {
        if (field->name() == name) {
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
    foreach (const Field *field, m_fields) {
        if (field->name() == name) {
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
    debug() << "matching" << path << "against" << pattern << "(exact is" << exact << ")";
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
            matchRegexpStr += f->regexp();
            matchRegexpStr += ")";
        } else {
            warning() << ".match couldn't find field " << fieldName;
        }

        lastEnd = match.capturedEnd();
    }

    // trailing string after the last match
    if (escapedPattern.length() > lastEnd) {
        matchRegexpStr += escapedPattern.mid(lastEnd);
    }

    if (exact)
        matchRegexpStr += "$";

    debug() << ".match" << path << "with" << matchRegexpStr;

    QRegularExpression matchRegexp(matchRegexpStr);

    QRegularExpressionMatch match = matchRegexp.match(path);

    debug() << ".match got" << match;

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
    copious() << ".formatFields(" << pattern << ", " << data << ")";
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
    const BranchBase *rootBranch = 0;
    if (m_pattern.length() == 0 || m_pattern[0] != '/') {
        // not absolute;
        rootBranch = qobject_cast<const BranchBase *>(root());
    }

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

bool BranchBase::areFieldsComplete(const QString pattern, const QVariantMap fields) const
{
    foreach (const QString fieldName, fieldNames(pattern)) {
        const Field *field = findField(fieldName);
        if (!field) {
            debug() << "can't find field" << fieldName;
            return false;
        }

        QVariant value = field->get(fields);
        if (!value.isValid()) {
            debug() << "field is not valid" << fieldName;
            return false;
        }
    }

    return true;
}

const QString BranchBase::map(const QVariant fields) const
{
    copious() << ".map" << fields;
    if (fields.isValid() && fields.canConvert<QVariantMap>() && areFieldsComplete(m_pattern, fields.toMap())) {
        QString mappedParent;
        if (m_pattern.length() == 0 || m_pattern[0] != '/') {
            // not absolute
            const BranchBase *rootBranch = qobject_cast<const BranchBase *>(root());
            copious() << "root" << rootBranch;
            // recurse
            if (rootBranch)
                mappedParent = rootBranch->map(fields);
        }
        return mappedParent + formatFields(m_pattern, fields.toMap());
    }

    return QString();
}

const QStringList BranchBase::listMatchingPaths(const QVariantMap env) const
{
    copious() << ".listMatchingPaths" << QJsonDocument::fromVariant(env);
    QStringList result;

    QString path = map(env);

    copious() << "initial map got" << path;

    if (!path.isEmpty()) {
        QFileInfo pathInfo(path);
        copious() << "hidden:" << pathInfo.isHidden();
        copious() << "dir:" << pathInfo.isDir();
        copious() << "file:" << pathInfo.isFile();
        copious() << "exists:" << pathInfo.exists();
        copious() << "m_exactMatchFlag:" << m_exactMatchFlag;
        if (!pathInfo.isHidden()) {
            if ((pathInfo.isDir() && !m_exactMatchFlag) ||
                (pathInfo.isFile() && m_exactMatchFlag)) {
                result << path;
                copious() << "returning" << result;
                return result;
            }
        }
    }

    if (m_pattern.length() > 0 && m_pattern[0] == '/') {
        // absolute
        copious() << "absolute file path detected";
        QFileInfo pathInfo(path);
        result = listMatchingPathsHelper(pathInfo.absoluteDir(), env);
    } else if (root()) {
        QString parentPath = root()->map(env);

        if (!parentPath.isEmpty()) {
            QDir parentDir(parentPath);
            if (parentDir.isAbsolute()) {
                result = listMatchingPathsHelper(parentDir, env);
            } else {
                warning() << ".map returned a relative path" << parentPath;
            }
        } else {
            warning() << ".map returned an empty path!";
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
                        copious() << "found" << entry.absoluteFilePath();
                        result.append(entry.absoluteFilePath() + "/");
                    }
                }
                result.append(listMatchingPathsHelper(entry.absoluteFilePath(), fields));

            } else if (m_exactMatchFlag) {
                QVariant entryFields = parse(entry.absoluteFilePath());
                if (entryFields.isValid() && containsFields(fields, entryFields.toMap())) {
                    copious() << "found" << entry.absoluteFilePath();
                    result.append(entry.absoluteFilePath());
                }
            }
        }
    }

    copious() << "got" << result;
    return result;
}
