#ifndef FILEPATTERN_H
#define FILEPATTERN_H

#include <QString>
#include <QRegularExpression>
#include <QList>

class FilePattern
{
public:
    FilePattern(const QString &path);

    const QString pattern() const;

    bool isSequence() const;
    const QString directory() const;
    const QString baseName() const;
    const QString frameSpec() const;
    const QString extension() const;

    const QString path() const;
    const QString path(int frame) const;

    QRegularExpressionMatch match(const QString &path) const;

    bool operator==(const FilePattern &other) const;

private:
    static const QString patternFromPath(const QString &path);
    static const QRegularExpression regexFromPattern(const QString &pattern);

    QString m_pattern;
    QRegularExpression m_regex;
};

uint qHash(const FilePattern &filePattern, uint seed);

#endif
