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

    QMap<QString, QFileInfoList> matches = listMatchingPatterns(context.toMap());

    info() << "Update matched" << matches.keys() << "from" << pattern();

    setCount(matches.size());

    int index = 0;
    QMapIterator<QString, QFileInfoList> i(matches);
    while (i.hasNext()) {
        i.next();
        ElementView *element = elementsView->elementAt(index);
        element->setPattern(i.key());
        element->scan(i.value());

        QVariantMap elementContext = parse(i.key()).toMap();
        // override with calling context
        QMapIterator<QString, QVariant> i(context.toMap());
        while (i.hasNext()) {
            i.next();
            elementContext.insert(i.key(), i.value());
        }
        setContext(index, elementContext);

        index++;
    }

    emit updated(OperationAttached::Finished);
}
