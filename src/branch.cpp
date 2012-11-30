#include <QDebug>
#include <QJsonDocument>

#include "branch.h"
#include "root.h"
#include "field.h"
#include "property.h"

Branch::Branch(QObject *parent) :
    BranchBase(parent)
{
    setExactMatch(false);

    // update -> onUpdate
    connect(this, &Branch::update, this, &Branch::onUpdate);
}

void Branch::onUpdate(const QVariant env)
{
    QStringList paths = listMatchingPaths(env);

    setPaths(paths);

    if (paths.isEmpty()) {
        warning() << this << ".onUpdate found no paths matching" << pattern() << "with" << QJsonDocument::fromVariant(env);
        // no matching paths found, error
        debug() << "emitting updated (error)";
        emit updated(OperationAttached::Error);
    } else {
        debug() << "emitting updated (finished)";
        emit updated(OperationAttached::Finished);
    }
}
