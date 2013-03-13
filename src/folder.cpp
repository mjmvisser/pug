#include <QDebug>
#include <QJsonDocument>

#include "folder.h"
#include "root.h"
#include "field.h"

Folder::Folder(QObject *parent) :
    Branch(parent)
{
    setExactMatch(false);

    // update -> onUpdate
    connect(this, &Folder::update, this, &Folder::onUpdate);
}

void Folder::onUpdate(const QVariant context)
{
    trace() << ".onUpdate(" << context << ")";

    QStringList paths = listMatchingPaths(context.toMap());

    info() << "Update matched" << paths << "from" << pattern();

    setPaths(paths, context.toMap());
    emit updated(OperationAttached::Finished);
}
