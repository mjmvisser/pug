#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>

#include "element.h"
#include "framelist.h"

/*!
    \class Element
    \inmodule Pug
*/

/*!
    \qmltype Element
    \instantiates Element
    \inqmlmodule Pug
    \brief Elements encapsulate the path of a directory, a file, or a sequence of files, and related data.

    Elements encapsulate the path of a directory, a file, or a sequence of files, and related data.
    BranchBase-derived classes contain a list of elements, each which is an instance of Element.
    Elements have convenience read-only properties that return the directory, baseName, frameSpec and extension
    of their path. They also store a FrameList object and corresponding read-only frames convenience property.
*/

Element::Element(QObject *parent) :
    PugItem(parent),
    m_frameList()
{
}

const QString Element::pattern() const
{
    return m_pattern;
}

void Element::setPattern(const QString p)
{
    const QString newPattern = patternFromPath(p);;
    if (m_pattern != newPattern) {
        m_pattern = newPattern;
        m_patternRegExp = regExpFromPattern(m_pattern);
        emit elementChanged();
    }
}

FrameList *Element::frameList()
{
    return m_frameList;
}

const FrameList *Element::frameList() const
{
    return m_frameList;
}

void Element::setFrameList(FrameList *fl)
{
    if (m_frameList != fl) {
        if (m_frameList)
            m_frameList->deleteLater();
        m_frameList = fl;
        emit elementChanged();
    }
}

QVariant Element::frames()
{
    return m_frameList ? m_frameList->frames() : QVariant();
}

const QVariant Element::frames() const
{
    return m_frameList ? m_frameList->frames() : QVariant();
}

void Element::setFrames(const QVariant f)
{
    if (m_frameList)
        m_frameList->deleteLater();

    if (f.canConvert<QString>()) {
        m_frameList = new FrameList(this);
        m_frameList->setPattern(f.toString());
    } else if (f.type() == QVariant::List) {
        m_frameList = new FrameList(this);
        m_frameList->setFrames(f.toList());
    } else {
        m_frameList = 0;
    }

    emit elementChanged();
}

const QVariant Element::firstFrame() const
{
    return m_frameList ? m_frameList->firstFrame() : QVariant();
}

const QVariant Element::lastFrame() const
{
    return m_frameList ? m_frameList->lastFrame() : QVariant();
}

const QStringList Element::paths() const
{
    QStringList result;
    if (hasFrames()) {
        foreach (QVariant frame, m_frameList->frames()) {
            // TODO: handle floating point frames?
            Q_ASSERT(frame.canConvert<int>());
            QString path;
            path.sprintf(m_pattern.toLatin1(), frame.toInt());
            result << path;
        }
    } else {
        result << m_pattern;
    }
    return result;
}

const QString Element::path() const
{
    if (hasFrames()) {
        return QString();
    } else {
        return m_pattern;
    }
}

const QString Element::path(QVariant frame) const
{
    if (hasFrames()) {
        Q_ASSERT(frame.canConvert<int>());
        QString path;
        path.sprintf(m_pattern.toLatin1(), frame.toInt());
        return path;
    } else {
        return m_pattern;
    }
}

const QVariant Element::frame(const QString path) const
{
    if (hasFrames()) {
        QRegularExpressionMatch match = m_patternRegExp.match(path);
        if (match.hasMatch()) {
            bool ok;
            QVariant result = match.captured("frame").toFloat(&ok);
            if (ok)
                return result;
        }
    }

    return QVariant();
}

const QString Element::directory() const
{
    QRegularExpressionMatch match = m_patternRegExp.match(m_pattern);
    if (match.hasMatch()) {
        return match.captured("directory");
    }
    else
        return QString();
}

const QString Element::baseName() const
{
    QRegularExpressionMatch match = m_patternRegExp.match(m_pattern);
    if (match.hasMatch())
        return match.captured("baseName");
    else
        return QString();
}

const QString Element::frameSpec() const
{
    QRegularExpressionMatch match = m_patternRegExp.match(m_pattern);
    if (match.hasMatch())
        return match.captured("frameSpec");
    else
        return QString();
}

const QString Element::extension() const
{
    QRegularExpressionMatch match = m_patternRegExp.match(m_pattern);
    if (match.hasMatch())
        return match.captured("extension");
    else
        return QString();
}

bool Element::hasFrames() const
{
    QRegularExpressionMatch match = m_patternRegExp.match(m_pattern);
    if (match.hasMatch())
        return !match.captured("frameSpec").isNull();
    else
        return false;
}

const QVariant Element::data() const
{
    return m_data;
}

void Element::setData(const QVariant d)
{
    if (!d.isValid())
        debug() << "OK";

    if (m_data != d) {
        m_data = d;
        emit dataChanged(m_data);
    }
}

bool Element::matches(const QString path) const
{
    return m_patternRegExp.match(path).hasMatch();
}

void Element::append(const QString path)
{
    if (hasFrames()) {
        if (!m_frameList)
            m_frameList = new FrameList(this);

        QVariantList frames = m_frameList->frames();

        QRegularExpressionMatch match = m_patternRegExp.match(path);
        if (match.hasMatch()) {
            QString frameStr = match.captured("frame");
            if (!frameStr.isNull()) {
                bool ok;
                float frame = frameStr.toFloat(&ok);
                frames << frame;
            } else {
                qWarning() << ".append got path with no frame" << path;
            }

            m_frameList->setFrames(frames);

            emit elementChanged();

        } else {
            qWarning() << this << ".append" << "path" << "does not match" << m_pattern;
        }
    } else {
        qWarning() << this << ".append, but do not have frames";
    }
}

const QString Element::toString() const
{
    if (hasFrames()) {
        return QString("%s [%s]").arg(m_pattern).arg(m_frameList->toString());
    } else {
        return m_pattern;
    }
}

const QString Element::patternFromPath(const QString path)
{
    static const QRegularExpression MATCH_FILENAME_WITH_FRAME(
            "^"
            "(?<directory>(.*/)?)"
            "(?<baseName>[^.]*)"
            "([.]"
                "("
                    "(?<frame>[0-9.]*?)|"
                    "(?<frameSpec>"
                        "(?<hash>#)|"
                        "(?<whirl>@+)|"
                        "(?<nuke>%0\\d+d)|"
                        "(?<houdini>($F?<width>(\\d?)))"
                    ")"
                ")"
            ")?"
            "([.]"
                "(?<extension>[^.]*)"
            ")?"
            "$");

    Q_ASSERT(MATCH_FILENAME_WITH_FRAME.isValid());

    QRegularExpressionMatch match = MATCH_FILENAME_WITH_FRAME.match(path);
    if (match.hasMatch()) {
        const QString directory = match.captured("directory");

        const QString baseName = match.captured("baseName");

        QString frameSpec;
        if (!match.captured("frame").isNull()) {
            int width = match.captured("frame").length();
            if (width >= 3 || (width >=1 && match.captured("frame").at(0) == '0')) {
                frameSpec = "%" + QString("0%1d").arg(width);
            } else {
                frameSpec = "%d";
            }
        } else if (!match.captured("hash").isNull()) {
            frameSpec = "%04d";

        } else if (!match.captured("whirl").isNull()) {
            int width = match.captured("whirl").length();
            frameSpec = "%" + QString("0%1d").arg(width);

        } else if (!match.captured("nuke").isNull()) {
            frameSpec = match.captured("nuke");

        } else if (!match.captured("houdini").isNull()) {
            bool ok;
            int width = match.captured("width").toFloat(&ok);
            if (ok) {
                frameSpec = "%" + QString("0%1d").arg(width);
            }
        }

        const QString extension = match.captured("extension");

        QString pattern;

        if (!directory.isNull())
            pattern += directory;
        if (!baseName.isNull())
            pattern += baseName;
        if (!frameSpec.isNull())
            pattern += "." + frameSpec;
        if (!extension.isNull())
            pattern += "." + extension;

        return pattern;

    } else {
        qWarning() << "couldn't match path" << path;
        return QString();
    }
}

const QRegularExpression Element::regExpFromPattern(const QString pattern)
{
    if (pattern.isNull())
        return QRegularExpression();

    static const QRegularExpression MATCH_PATTERN_FILE("^(.*/)?([^.]*)?(?:[.]([^.]*))?$");
    static const QRegularExpression MATCH_PATTERN_SEQUENCE("^(.*/)?(?:([^.]*))(?:[.](?:%0(\\d+)d))(?:[.]([^.]*))?$");

    QString regExpStr = pattern;
    if (MATCH_PATTERN_FILE.match(pattern).hasMatch()) {
        regExpStr.replace(MATCH_PATTERN_FILE, "^(?<directory>\\1)(?<baseName>\\2)[.](?<extension>\\3)$");
    } else if (MATCH_PATTERN_SEQUENCE.match(pattern).hasMatch()) {
        regExpStr.replace(MATCH_PATTERN_SEQUENCE, "^(?<directory>\\1)(?<baseName>\\2)[.]((?<frame>[0-9.]{\\3}?)|(?<frameSpec>%0\\d+d))[.](?<extension>\\4)$");
    } else {
        qWarning() << "unable to generate regexp for pattern" << pattern;
    }

    return QRegularExpression(regExpStr);
}
