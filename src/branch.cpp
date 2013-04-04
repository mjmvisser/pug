#include <QDebug>
#include <QQmlProperty>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include <QSet>


#include "branch.h"
#include "root.h"
#include "field.h"
#include "param.h"
#include "filepattern.h"

/*!
    \class Branch
    \inmodule Pug
*/
/*!
    \qmltype Branch
    \instantiates Branch
    \inqmlmodule Pug
    \brief The Branch class is the base class for files and directories ("branches").

    Branch is the base class of Folder and File, which represent directories and files,
    respectively. Branches have a root() that indicates their filesystem parent. If the root
    is not set, it is assumed to be the first parent of type Branch.

    For example, consider the following example:
    \qml
        import Pug 1.0

        Root {
            Folder {
                id: topLevel

                Folder {
                    id: first
                }

                Folder {
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
        Folder {
            pattern: "/prod/projects/{PROJECT}/shots/{SEQUENCE}_{SHOT}"
        }
    \endqml

    The path or paths matched by the pattern are stored in the elements property. Each element is an
    instance of Element. Each element also has a data property that contains the fields represented
    by the element's path.
*/

Branch::Branch(QObject *parent) :
    Node(parent),
    m_root()
{
    addInput(this, "root");

    addParam(this, "pattern");

    setDependencyOrder(Node::InputsSelfChildren);
}

const QList<const Field *> Branch::fields(const QStringList fieldNameList) const
{
    QList<const Field *> results;

    // add our list of field names to those passed in
    QStringList fnames = fieldNameList;
    fnames.append(fieldNames(pattern()));

    // parent fields
    const Branch *s = root();
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

const Branch *Branch::root() const
{
    if (m_root) {
        return m_root;
    } else {
        const Branch *p = parentBranch();
        return p && !p->isRoot() ? p : 0;
    }
}

Branch *Branch::root()
{
    if (m_root) {
        return m_root;
    } else {
        Branch *p = parentBranch();
        return p && !p->isRoot() ? p : 0;
    }
}

void Branch::setRoot(Branch *branch)
{
    if (m_root != branch) {
        m_root = branch;
        emit rootChanged(m_root);
    }
}

QQmlListProperty<Field> Branch::fields_()
{
    return QQmlListProperty<Field>(this, m_fields);
}

//void Branch::setPaths(const QStringList paths, const QVariantMap context)
//{
//
//    trace() << ".setPaths(" << paths << ")";
//
//    clearDetails();
//
//    int index = 0;
//    foreach (const QString path, paths) {
//        bool found = false;
//
//        for (int i = 0; i < details().property("length").toInt(); i++) {
//            Element *element = element(i);
//            if (element->hasFrames() && element->matches(path)) {
//                element->append(path);
//                setElement(index, element, false);
//                found = true;
//                break;
//            }
//        }
//
//        if (!found) {
//            Element *element = new Element;
//            element->setPattern(path);
//            if (element->hasFrames())
//                element->append(path);
//            QVariantMap elementContext = parse(element->pattern()).toMap();
//
//            // merge input field values
//            QMapIterator<QString, QVariant> i(context);
//            while (i.hasNext()) {
//                i.next();
//                // don't overwrite
//                if (!elementContext.contains(i.key()))
//                    elementContext.insert(i.key(), i.value());
//            }
//
//            setContext(index, elementContext, false);
//            setElement(index, element, false);
//            index++;
//        }
//    }
//
//    setCount(index);
//    debug() << details().property("length").toInt();
//    debug() << details().toVariant();
//    emit detailsChanged();
//}

const QString Branch::pattern() const
{
    return m_pattern;
}

void Branch::setPattern(const QString p)
{
    if (m_pattern != p) {
        m_pattern = p;
        emit patternChanged(m_pattern);
    }
}


bool Branch::exactMatch() const
{
    return m_exactMatchFlag;
}

void Branch::setExactMatch(bool f)
{
    m_exactMatchFlag = f;
}

Branch *Branch::parentBranch()
{
    return firstParent<Branch>();
}

const Branch *Branch::parentBranch() const
{
    return firstParent<Branch>();
}

Field *Branch::findField(const QString name)
{
    // check our fields first
    foreach (Field *field, m_fields) {
        if (field->name() == name) {
            return field;
        }
    }

    // check our parent input
    Branch *p = parentBranch();
    if (p) {
        return p->findField(name);
    }

    return 0;
}

const Field *Branch::findField(const QString name) const
{
    // check our fields first
    foreach (const Field *field, m_fields) {
        if (field->name() == name) {
            return field;
        }
    }

    // check our parent input
    const Branch *p = parentBranch();
    if (p) {
        return p->findField(name);
    }

    return 0;
}

//bool Branch::hasField(const QString name) const
//{
//    foreach (const Field *field, m_fields) {
//        if (field->name() == name) {
//            return true;
//        }
//    }
//    return false;
//}

const QStringList Branch::fieldNames(const QString pattern) const
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

const QVariant Branch::match(const QString pattern, const QString path, bool exact, bool partial) const
{
    //trace() << ".match(" << pattern << "," << path << "," << exact << ")";
    static const QRegularExpression replaceFieldsRegexp("\\{(\\w+)\\}");

    const QString escapedPattern = escapeSpecialCharacters(pattern);
    QRegularExpressionMatchIterator it = replaceFieldsRegexp.globalMatch(escapedPattern);

    // adapted from QString::replace code

    int lastEnd = 0;
    // assemble a regular expression string
    QString matchRegexpStr = "^";
    QSet<const QString> usedFieldNames;
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
            if (!usedFieldNames.contains(fieldName)) {
                matchRegexpStr += "(?<";
                matchRegexpStr += fieldName;
                matchRegexpStr += ">";
                matchRegexpStr += f->regexp();
                matchRegexpStr += ")";
                usedFieldNames.insert(fieldName);
            } else {
                // already used, so just reference the previous match
                matchRegexpStr += "(?P=";
                matchRegexpStr += fieldName;
                matchRegexpStr += ")";
            }
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

    //info() << ".match" << path << "with" << matchRegexpStr;

    QRegularExpression matchRegexp(matchRegexpStr);

    Q_ASSERT_X(matchRegexp.isValid(), QString("regular expression: " + matchRegexpStr).toUtf8(), matchRegexp.errorString().toUtf8());

    QRegularExpressionMatch match = matchRegexp.match(path, 0,
            partial ? QRegularExpression::PartialPreferCompleteMatch : QRegularExpression::NormalMatch);

    //info() << "result:" << match;

    if (match.hasMatch() || match.hasPartialMatch()) {
        QVariantMap context;

        foreach (QString fieldName, fieldNames(pattern)) {
            const Field *f = findField(fieldName);
            if (f && !match.captured(fieldName).isNull()) {
                context[fieldName] = f->parse(match.captured(fieldName));
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
                context["_"] = remainder;
            else
                return QVariant();
        }

        //debug() << "result:" << context;

        return context;
    }

    return QVariant();
}

const QString Branch::formatFields(const QString pattern, const QVariant data) const
{
    //trace() << ".formatFields(" << pattern << ", " << data << ")";
    QVariantMap fields;
    if (data.isValid() && data.type() == QVariant::Map) {
        fields = data.toMap();
    } else {
        error() << "invalid context data" << data;
        return QString();
    }

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

const QVariant Branch::parse(const QString path, bool partial) const
{
    //trace() << ".parse(" << path << ")";
    const Branch *rootBranch = 0;
    if (m_pattern.length() == 0 || m_pattern[0] != '/') {
        // not absolute;
        rootBranch = qobject_cast<const Branch *>(root());
    }

    if (rootBranch) {
        // recurse
        QVariant data = rootBranch->parse(path, false);

        if (data.isValid()) {
            QVariantMap fields = data.toMap();
            if (fields.contains("_")) {
                QString remainder = fields.take("_").toString();

                //debug() << ".parse got" << fields << remainder;

                // match the remainder
                const QVariant ourData = match(m_pattern, remainder, m_exactMatchFlag, partial);

                //debug() << ".parse matched" << ourData;

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
        return match(m_pattern, path, m_exactMatchFlag, partial);
    }
    return QVariant();
}

bool Branch::fieldsComplete(const QString pattern, const QVariantMap fields) const
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

bool Branch::fieldsComplete(const QVariantMap context) const
{
    return fieldsComplete(m_pattern, context) && (!root() || root()->fieldsComplete(context));
}

const QString Branch::map(const QVariant context) const
{
    //debug() << "mapping" << m_pattern << "with" << context;
    //trace() << ".map(" << context << ")";

    if (!context.isValid()) {
        error() << "Can't map" << m_pattern << "with invalid context";
        return QString();
    } else if (!context.canConvert<QVariantMap>()) {
        error() << "Can't map" << m_pattern << "with non-map context" << context;
        return QString();
    } else if (!fieldsComplete(context.toMap())) {
        error() << "Can't map" << m_pattern << "with incomplete context" << context.toMap();
        return QString();
    }

    if (m_pattern[0] != '/' && root()) {
        // recurse
        if (root()) {
            QString parentPath = root()->map(context);
            if (!parentPath.isNull())
                return parentPath + formatFields(m_pattern, context.toMap());
            else
                return QString();
        } else {
            return QString();
        }
    } else {
        return formatFields(m_pattern, context.toMap());
    }
}

const QMap<QString, QSet<QFileInfo> > Branch::listMatchingPatterns(const QVariantMap context) const
{
    trace() << ".listMatchingPatterns(" << context << ")";
    QMap<QString, QSet<QFileInfo> > result;

    if (fieldsComplete(context)) {
        debug() << "fields complete, mapping";
        // case 1 or 2
        QString path = map(context);
        debug() << "map got" << path << "from" << m_pattern << context << root();
        Q_ASSERT(!path.isEmpty());

        FilePattern fp(path);
        //debug() << "========================================adding empty match" << fp.pattern();
        result.insert(fp.pattern(), QSet<QFileInfo>());

        QDir parentDir = QDir(fp.directory());
        if (!m_exactMatchFlag)
            parentDir.cdUp();

        result = listMatchingPatternsHelper(parentDir, context, result);

        QMapIterator<QString, QSet<QFileInfo> > j(result);
        while (j.hasNext()) {
            j.next();
            debug() << "got:" << j.key() << j.value().count();
        }

    } else if (root()) {
        debug() << "---------------------------------------listing parent";
        // recurse on root until we run out of root
        QMap<QString, QSet<QFileInfo> > parentResults = root()->listMatchingPatterns(context);

        debug() << "Listing from:" << parentResults.keys();

        QMapIterator<QString, QSet<QFileInfo> > i(parentResults);
        while (i.hasNext()) {
            i.next();
            FilePattern fp(i.key());

            QDir parentDir = QDir(fp.directory());

            QMap<QString, QSet<QFileInfo> > parentElementResults = listMatchingPatternsHelper(parentDir, context, result);

            QMapIterator<QString, QSet<QFileInfo> > j(parentElementResults);
            while (j.hasNext()) {
                j.next();
                foreach (const QFileInfo f, j.value()) {
                    result[j.key()].insert(f);
                }
            }
        }

        QMapIterator<QString, QSet<QFileInfo> > j(result);
        while (j.hasNext()) {
            j.next();
            debug() << "got:" << j.key() << j.value().count();
        }
    } else if (m_pattern[0] == '/') {
        // special case for absolute path
        // strip off path components until fields are complete
        QDir parentDir(m_pattern);
        while (parentDir.cdUp() && !fieldsComplete(parentDir.dirName(), context));

        result = listMatchingPatternsHelper(parentDir, context, result);
    }
    return result;
}

bool Branch::containsFields(const QVariantMap& needle, const QVariantMap& haystack) const
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

const QMap<QString, QSet<QFileInfo> > Branch::listMatchingPatternsHelper(const QDir parentDir, const QVariantMap context, const QMap<QString, QSet<QFileInfo> > matches) const
{
    trace() << ".listMatchingPatternsHelper(" << parentDir.absolutePath() << "," << context << ")";
    Q_ASSERT(!parentDir.isRoot());

    debug() << "matching patterns in" << parentDir << "from" << context << "with" << m_pattern;

    QMap<QString, QSet<QFileInfo> > result = matches;

    QFileInfoList entries = parentDir.entryInfoList();
    foreach (QFileInfo entry, entries) {
        if (!entry.isHidden()) {
            debug() << "checking" << entry.filePath();
            if (entry.isDir()) {
                QVariant entryFields = parse(entry.filePath() + "/");
                // if we've got an exact match, the "_" entry will be invalid
                if (!m_exactMatchFlag) {
                    QVariant entryFields = parse(entry.filePath() + "/");

                    debug() << "parse(" << entry.filePath() << "/) returned" << entryFields;

                    if (entryFields.isValid() && containsFields(context, entryFields.toMap()) && !entryFields.toMap()["_"].isValid()) {
                        const QString path = entry.filePath() + "/";
                        FilePattern fp(path);
                        result[fp.pattern()].insert(entry);
                        debug() << "adding match" << fp.pattern() << "[file" << entry.filePath() << "]";
                        continue;
                    }
                }

                // check for a partial match
                QVariant m = parse(entry.filePath() + "/", true);
                debug() << "containsFields(" << m.toMap() << "," << context << ") returns" << containsFields(m.toMap(), context);
                if (m.isValid() && containsFields(m.toMap(), context)) {
                    debug() << "got partial match of" << entry.filePath() << "against" << pattern() << "with" << m.toMap();
                    // recurse
                    result = listMatchingPatternsHelper(entry.filePath() + "/", context, result);
                }
            } else if (m_exactMatchFlag) {
                // found a file and we're looking for an exact match
                QVariant entryFields = parse(entry.filePath());
                debug() << "entryFields" << entryFields;
                debug() << "context" << context;
                if (entryFields.isValid() && containsFields(context, entryFields.toMap())) {
                    const QString path = entry.filePath();
                    FilePattern fp(path);
                    result[fp.pattern()].insert(entry);
                    debug() << "adding match" << fp.pattern() << "[file" << entry.filePath() << "]";
                }
            }
        }
    }

    return result;
}

