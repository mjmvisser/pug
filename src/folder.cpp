#include <QDebug>
#include <QJsonDocument>

#include "folder.h"
#include "root.h"
#include "field.h"
#include "elementsview.h"

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

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    QVariantMap localContext;
    if (root())
        localContext = mergeContexts(context.toMap(),
            root()->details().property(0).property("context").toVariant().toMap());
    else
        localContext = context.toMap();

    QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(localContext);

    info() << "Update matched" << matches.keys() << "from" << pattern();

    setCount(matches.size());

    if (matches.size() > 0) {
        int index = 0;
        QMapIterator<QString, QSet<QFileInfo> > i(matches);
        while (i.hasNext()) {
            i.next();
            ElementView *element = elementsView->elementAt(index);
            element->setPattern(i.key());
            element->scan(i.value().toList());

            QVariantMap elementContext = parse(i.key()).toMap();

            setContext(index, mergeContexts(localContext, elementContext));

            index++;
        }

    }
//    else {
//        error() << "no match found for" << pattern() << "with" << localContext;
//
//        emit updateFinished(OperationAttached::Error);
//    }
    emit updateFinished(OperationAttached::Finished);
}
