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
#include "input.h"
#include "output.h"


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
    // TODO: is this only required so that release nodes can find their version? there must be a better way...
    //addInput(this, "root");

    addParam(this, "pattern");

    setDependencyOrder(Node::InputsSelfChildren);
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


const QVariant Branch::parse(const QString path, bool partial) const
{
    trace() << ".parse(" << path << ")";
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

                debug() << ".parse got" << fields << remainder;

                // match the remainder
                const QVariant ourData = match(m_pattern, remainder, m_exactMatchFlag, partial);

                debug() << ".parse matched" << ourData;

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

bool Branch::fieldsComplete(const QVariantMap context) const
{
    trace() << ".fieldsComplete(" << context << ")";
    return Node::fieldsComplete(m_pattern, context) && (!root() || root()->fieldsComplete(context));
}

const QString Branch::map(const QVariant context) const
{
    //debug() << "mapping" << m_pattern << "with" << context;
    //trace() << ".map(" << context << ")";

    if (!context.isValid()) {
        warning() << "Can't map" << m_pattern << "with invalid context";
        return QString();
    } else if (!context.canConvert<QVariantMap>()) {
        warning() << "Can't map" << m_pattern << "with non-map context" << context;
        return QString();
    } else if (!fieldsComplete(context.toMap())) {
        warning() << "Can't map" << m_pattern << "with incomplete context" << context.toMap();
        return QString();
    }

    if (m_pattern[0] != '/' && root()) {
        // recurse
        if (root()) {
            QString parentPath = root()->map(context);
            if (!parentPath.isNull())
                return parentPath + format(m_pattern, context.toMap());
            else
                return QString();
        } else {
            return QString();
        }
    } else {
        return format(m_pattern, context.toMap());
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

        QFileInfo pathInfo(fp.pattern());

        if (pathInfo.exists()) {
            // optimization so we don't scan unnecessarily
            QSet<QFileInfo> infoSet;
            infoSet << pathInfo;
            result.insert(fp.pattern(), infoSet);

        } else {
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
        while (parentDir.cdUp() && !Node::fieldsComplete(parentDir.dirName(), context));

        result = listMatchingPatternsHelper(parentDir, context, result);
    }
    return result;
}

bool Branch::containsFields(const QVariantMap& needle, const QVariantMap& haystack) const
{
    // return true if all fields in needle are contained in haystack
    foreach (const QString fieldName, Field::fieldNames(m_pattern)) {
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
                if (m.isValid()) { // && containsFields(m.toMap(), context)) {
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
