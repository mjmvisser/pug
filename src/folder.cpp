#include <QDebug>
#include <QJsonDocument>
#include <QFileInfo>

#include "folder.h"
#include "root.h"
#include "field.h"
#include "updateoperation.h"
#include "cookoperation.h"
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

    connect(updateOperationAttached(), &UpdateOperationAttached::cook, this, &Folder::update_onCook);
    connect(releaseOperationAttached(), &ReleaseOperationAttached::cook, this, &Folder::release_onCook);
}

void Folder::update_onCook()
{
    trace() << "UpdateOperation.onCook()";

    clearDetails();

    QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(context());

    info() << "Update matched" << matches.keys() << "from" << pattern();

    setCount(matches.size());

    if (matches.size() > 0) {
        int index = 0;
        QMapIterator<QString, QSet<QFileInfo> > i(matches);

        while (i.hasNext()) {
            i.next();

            setDetail(index, "element", "pattern", toScriptValue(i.key()));
            QVariantMap elementContext = parse(i.key()).toMap();
            setContext(index, mergeContexts(context(), elementContext));

            Q_ASSERT(i.value().size() <= 1);

            if (i.value().size() > 0) {
                const QFileInfo info = i.value().toList().first();
                setDetail(index, "timestamp", toScriptValue(info.lastModified()));
            } else {
                setDetail(index, "timestamp", QJSValue(QJSValue::NullValue));
            }

            index++;
        }
    }

    emit updateOperationAttached()->cookFinished();
}

void Folder::release_onCook()
{
    trace() << "ReleaseOperation.onCook()";

    if (!releaseOperationAttached()->versionFieldName().isEmpty()) {

        if (!context().contains(releaseOperationAttached()->versionFieldName())) {
            error() << "context has no" << releaseOperationAttached()->versionFieldName();
            emit releaseOperationAttached()->cookFinished();
            return;
        }

        m_queue->setSudo(releaseOperationAttached()->sudo());

        QMap<QString, QSet<QFileInfo> > matches = listMatchingPatterns(context());

        info() << "Release matched" << matches.keys() << "from" << pattern();

        setCount(matches.size());

        if (matches.size() > 0) {
            int index = 0;
            QMapIterator<QString, QSet<QFileInfo> > i(matches);
            while (i.hasNext()) {
                i.next();

                setDetail(index, "element", "pattern", toScriptValue(i.key()));
                QVariantMap elementContext = parse(i.key()).toMap();
                setContext(index, mergeContexts(context(), elementContext));

                Q_ASSERT(i.value().size() <= 1);

                if (i.value().size() > 0) {
                    const QFileInfo info = i.value().toList().first();
                    setDetail(index, "timestamp", toScriptValue(info.lastModified()));
                } else {
                    setDetail(index, "timestamp", QJSValue(QJSValue::NullValue));
                    m_queue->mkdir(i.key());
                }

                index++;
            }
        }

        m_queue->run();
    } else {
        emit releaseOperationAttached()->cookFinished();
    }
}

void Folder::onFileOpQueueFinished()
{
    trace() << ".onFileOpQueueFinished()";
    emit releaseOperationAttached()->cookFinished();
}

void Folder::onFileOpQueueError()
{
    trace() << ".onFileOpQueueError()";
    emit releaseOperationAttached()->cookFinished();
}

UpdateOperationAttached *Folder::updateOperationAttached()
{
    return attachedPropertiesObject<UpdateOperation, UpdateOperationAttached>();
}

ReleaseOperationAttached *Folder::releaseOperationAttached()
{
    return attachedPropertiesObject<ReleaseOperation, ReleaseOperationAttached>();
}

