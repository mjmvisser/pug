#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QMap>
#include <QVariant>
#include <QMapIterator>

#include "util.h"

Util::Util(QObject *parent) :
    QObject(parent)
{
}

bool Util::mkpath(const QString path)
{
    QDir dir;
    return dir.mkpath(path);
}

bool Util::rmdir(const QString path)
{
    QDir dir;
    return dir.rmdir(path);
}

const QString Util::readFile(const QString path)
{
    QFile data(path);
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        return in.readAll().trimmed();
    } else {
        return QString();
    }
}

bool Util::exists(const QString path)
{
    QFile file(path);
    return file.exists();
}

bool Util::touch(const QString path)
{
    QFile file(path);
    return file.open(QFile::WriteOnly);
}

bool Util::remove(const QString path)
{
    QFile file(path);
    return file.remove();
}

bool Util::copy(const QString src, const QString dest)
{
    return QFile::copy(src, dest);
}

ElementsView *Util::elementsView(Node *node)
{
    if (node) {
        // no parent means V8 takes ownership
        ElementsView *obj = new ElementsView(node);
        return obj;
    } else {
        return 0;
    }
}

const QVariantMap Util::mergeContexts(const QVariantMap first, const QVariantMap second) const
{
    return Node::mergeContexts(first, second);
}
