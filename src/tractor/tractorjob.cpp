#include <QTextStream>
#include <QTemporaryFile>
#include <QProcess>
#include <QDebug>

#include "tractorjob.h"
#include "tractortask.h"

TractorJob::TractorJob(QObject *parent) :
    TractorBlock(parent),
    m_serialSubtasksFlag(false)
{
}


const QString &TractorJob::envKey() const
{
    return m_envKey;
}

void TractorJob::setEnvKey(const QString &key)
{
    if (m_envKey != key) {
        m_envKey = key;
        emit envKeyChanged(key);
    }
}

const QString &TractorJob::service() const
{
    return m_service;
}

void TractorJob::setService(const QString &service)
{
    if (m_service != service) {
        m_service = service;
        emit serviceChanged(service);
    }
}

const QString &TractorJob::title() const
{
    return m_title;
}

void TractorJob::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged(title);
    }
}

const QString &TractorJob::comment() const
{
    return m_comment;
}

void TractorJob::setComment(const QString &comment)
{
    if (m_comment != comment) {
        m_comment = comment;
        emit commentChanged(comment);
    }
}

//const QString &TractorJob::metadata() const
//{
//    return m_metadata;
//}
//
//void TractorJob::setMetadata(const QString &metadata)
//{
//    if (m_metadata != metadata) {
//        m_metadata = metadata;
//        emit metadataChanged(metadata);
//    }
//}

bool TractorJob::hasSerialSubtasks() const
{
    return m_serialSubtasksFlag;
}

void TractorJob::setSerialSubtasks(bool flag) {
    if (m_serialSubtasksFlag != flag) {
        m_serialSubtasksFlag = flag;
        emit serialSubtasksChanged(flag);
    }
}

QQmlListProperty<TractorTask> TractorJob::subtasks_()
{
    return QQmlListProperty<TractorTask>(this, 0, TractorJob::subtasks_append,
                                        TractorJob::subtasks_count,
                                        TractorJob::subtask_at,
                                        TractorJob::subtasks_clear);
}

void TractorJob::addSubtask(TractorTask *task)
{
    task->setParent(this);
    m_subtasks.append(task);
}

// subtasks property
void TractorJob::subtasks_append(QQmlListProperty<TractorTask> *prop, TractorTask *p)
{
    TractorJob *that = static_cast<TractorJob *>(prop->object);

    p->setParent(that);
    that->m_subtasks.append(p);
    emit that->subtasksChanged();
}

int TractorJob::subtasks_count(QQmlListProperty<TractorTask> *prop)
{
    TractorJob *that = static_cast<TractorJob *>(prop->object);
    return that->m_subtasks.count();
}

TractorTask *TractorJob::subtask_at(QQmlListProperty<TractorTask> *prop, int i)
{
    TractorJob *that = static_cast<TractorJob *>(prop->object);
    if (i < that->m_subtasks.count())
        return that->m_subtasks[i];
    else
        return 0;
}

void TractorJob::subtasks_clear(QQmlListProperty<TractorTask> *prop)
{
    TractorJob *that = static_cast<TractorJob *>(prop->object);
    foreach (TractorTask *p, that->m_subtasks) {
        p->setParent(0);
    }
    that->m_subtasks.clear();
    emit that->subtasksChanged();
}

const QString TractorJob::asString(int indent) const
{
    QString s = "";
    QTextStream stream(&s);

    QString spaces(indent, ' ');

    stream << spaces << "##AlfredToDo 3.0" << endl;
    stream << endl;
    stream << spaces << "Job";

    if (m_envKey.length() > 0)
        stream << " -envkey {" << m_envKey << "}";

    if (m_service.length() > 0)
        stream << " -service {" << m_service << "}";

    if (m_title.length() > 0)
        stream << " -title {" << m_title << "}";

    if (m_comment.length() > 0)
        stream << " -comment {" << m_comment << "}";

    //if (m_metadata.length() > 0)
    //    stream << " -metadata {" << m_metadata << "}";

    if (m_serialSubtasksFlag)
        stream << " -serialsubtasks {1}";

    if (m_subtasks.length() > 0) {
        stream << " -subtasks {" << endl;

        foreach (const TractorTask *subtask, m_subtasks) {
            stream << subtask->asString(indent + 4);
        }

        stream << spaces << "}" << endl;
    }

    return s;
}

void TractorJob::submit()
{
    QTemporaryFile jobFile("tractorXXXXXX.job");
    if (jobFile.open()) {
        QTextStream stream(&jobFile);
        stream << this->asString();
        qDebug() << this->asString();
        jobFile.close();
        QStringList args;
        args << jobFile.fileName();
        int result = QProcess::execute("tractor-spool.py", args);
        if (result != 0) {
            qWarning() << "tractor-spool.py" << args << "returned" << result;
        }
    } else {
        qWarning() << "unable to create temporary file";
    }
}
