#include <QDebug>
#include <QJsonDocument>
#include <QFileInfo>

#include "folder.h"
#include "root.h"
#include "field.h"
#include "elementsview.h"
#include "updateoperation.h"
#include "releaseoperation.h"

Folder::Folder(QObject *parent) :
    Branch(parent),
    m_queue(new FileOpQueue(this))
{
    setExactMatch(false);

    connect(m_queue, &FileOpQueue::finished, this, &Folder::onFileOpQueueFinished);
    connect(m_queue, &FileOpQueue::error, this, &Folder::onFileOpQueueError);
}

void Folder::componentComplete()
{
    Branch::componentComplete();

    connect(attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>(), &UpdateOperationAttached::cook,
            this, &Folder::update_onCook);
    connect(attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>(), &ReleaseOperationAttached::cook,
            this, &Folder::release_onCook);
}

void Folder::update_onCook(const QVariant context)
{
    trace() << "UpdateOperation.onCook(" << context << ")";

    clearDetails();

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    QVariantMap localContext;
    if (root())
        // TODO: this is hella sketchy
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

            if (i.value().toList().length() > 0) {
                ElementView *element = elementsView->elementAt(index);
                element->setPattern(i.key());
                element->scan(i.value().toList());

                QVariantMap elementContext = parse(i.key()).toMap();

                setContext(index, mergeContexts(localContext, elementContext));
            }

            index++;
        }

    }
//    else {
//        error() << "no match found for" << pattern() << "with" << localContext;
//
//        emit attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>()->cookFinished();
//    }
    emit attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>()->cookFinished();
}

void Folder::release_onCook(const QVariant context)
{
    trace() << "ReleaseOperation.onCook(" << context << ")";

    ReleaseOperationAttached *attached = attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>();

    m_queue->setSudo(attached->sudo());

    QScopedPointer<ElementsView> elementsView(new ElementsView(this));

    QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(context.toMap());

    info() << "Release matched" << matches.keys() << "from" << pattern();

    setCount(matches.size());

    if (matches.size() > 0) {
        int index = 0;
        QMapIterator<QString, QSet<QFileInfo> > i(matches);
        while (i.hasNext()) {
            i.next();
            ElementView *element = elementsView->elementAt(index);
            element->setPattern(i.key());
            element->scan(i.value().toList());

            if (element->timestamp().isNull())
                m_queue->mkdir(i.key());

            QVariantMap elementContext = parse(i.key()).toMap();

            setContext(index, mergeContexts(context.toMap(), elementContext));

            index++;
        }
    }

    m_queue->run();
}

void Folder::onFileOpQueueFinished()
{
    trace() << ".onFileOpQueueFinished()";
    emit attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>()->cookFinished();
}

void Folder::onFileOpQueueError()
{
    trace() << ".onFileOpQueueError()";
    emit attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>()->cookFinished();
}

