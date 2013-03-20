#include <QDebug>

#include "filepattern.h"

FilePattern::FilePattern(const QString &path) :
    m_pattern(FilePattern::patternFromPath(path)),
    m_regex(FilePattern::regexFromPattern(m_pattern))
{
}

const QString FilePattern::pattern() const
{
    return m_pattern;
}

bool FilePattern::isSequence() const
{
    QRegularExpressionMatch match = m_regex.match(pattern());
    if (match.hasMatch())
        return !match.captured("frameSpec").isNull();
    else
        return false;
}

const QString FilePattern::directory() const
{
    QRegularExpressionMatch match = m_regex.match(pattern());
    if (match.hasMatch()) {
        return match.captured("directory");
    }
    else
        return QString();
}

const QString FilePattern::baseName() const
{
    QRegularExpressionMatch match = m_regex.match(pattern());
    if (match.hasMatch())
        return match.captured("baseName");
    else
        return QString();
}

const QString FilePattern::frameSpec() const
{
    QRegularExpressionMatch match = m_regex.match(pattern());
    if (match.hasMatch())
        return match.captured("frameSpec");
    else
        return QString();
}

const QString FilePattern::extension() const
{
    QRegularExpressionMatch match = m_regex.match(pattern());
    if (match.hasMatch())
        return match.captured("extension");
    else
        return QString();
}

const QString FilePattern::path() const
{
    if (!isSequence())
        return m_pattern;
    else
        return QString();
}

const QString FilePattern::path(int frame) const
{
    if (isSequence()) {
        QString path;
        path.sprintf(m_pattern.toLatin1(), frame);
        return path;
    } else {
        return QString();
    }
}

QRegularExpressionMatch FilePattern::match(const QString &path) const
{
    return m_regex.match(path);
}

bool FilePattern::operator==(const FilePattern &other) const
{
    return m_pattern == other.m_pattern;
}

const QString FilePattern::patternFromPath(const QString &path)
{
    if (path.isEmpty())
        return QString();

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

const QRegularExpression FilePattern::regexFromPattern(const QString &pattern)
{
    if (pattern.isEmpty())
        return QRegularExpression();

    static const QRegularExpression MATCH_PATTERN_DIRECTORY("^(.*/)$");
    static const QRegularExpression MATCH_PATTERN_FILE("^(.*/)?([^.]*)?(?:[.]([^.]*))?$");
    static const QRegularExpression MATCH_PATTERN_SEQUENCE("^(.*/)?(?:([^.]*))(?:[.](?:%0(\\d+)d))(?:[.]([^.]*))?$");

    QString regExpStr = pattern;
    if (MATCH_PATTERN_DIRECTORY.match(pattern).hasMatch()) {
        regExpStr.replace(MATCH_PATTERN_FILE, "^(?<directory>\\1)$");
    } else if (MATCH_PATTERN_FILE.match(pattern).hasMatch()) {
        regExpStr.replace(MATCH_PATTERN_FILE, "^(?<directory>\\1)(?<baseName>\\2)[.](?<extension>\\3)$");
    } else if (MATCH_PATTERN_SEQUENCE.match(pattern).hasMatch()) {
        regExpStr.replace(MATCH_PATTERN_SEQUENCE, "^(?<directory>\\1)(?<baseName>\\2)[.]((?<frame>[0-9.]{\\3}?)|(?<frameSpec>%0\\d+d))[.](?<extension>\\4)$");
    } else {
        qWarning() << "unable to generate regexp for pattern" << pattern;
    }

    return QRegularExpression(regExpStr);
}

uint qHash(const FilePattern &filePattern, uint seed)
{
    return qHash(filePattern.pattern(), seed);
}
