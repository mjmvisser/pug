#include <QDebug>
#include <QJsonDocument>

#include "branch.h"
#include "root.h"
#include "field.h"

Branch::Branch(QObject *parent) :
    BranchBase(parent)
{
    setExactMatch(false);

    // update -> onUpdate
    connect(this, &Branch::update, this, &Branch::onUpdate);
}

void Branch::onUpdate(const QVariant context)
{
    QStringList paths = listMatchingPaths(context.toMap());

    copious() << this << "onUpdate got" << paths;

    setPaths(paths, context.toMap());
    emit updated(OperationAttached::Finished);
}
