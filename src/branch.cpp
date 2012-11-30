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

    copious() << this << "onUpdate got" << paths;

    setPaths(paths);
    emit updated(OperationAttached::Finished);
}
