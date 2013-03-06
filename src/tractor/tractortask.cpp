#include <QTextStream>

#include "tractortask.h"
#include "tractorcmd.h"

TractorTask::TractorTask(QObject *parent) :
    TractorBlock(parent),
    m_serialSubtasksFlag(false)
{
}

const QString &TractorTask::title() const
{
    return m_title;
}

void TractorTask::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged(title);
    }
}

bool TractorTask::hasSerialSubtasks() const
{
    return m_serialSubtasksFlag;
}

void TractorTask::setSerialSubtasks(bool flag) {
    if (m_serialSubtasksFlag != flag) {
        m_serialSubtasksFlag = flag;
        emit serialSubtasksChanged(flag);
    }
}

QQmlListProperty<TractorTask> TractorTask::subtasks_()
{
    return QQmlListProperty<TractorTask>(this, 0, TractorTask::subtasks_append,
                                        TractorTask::subtasks_count,
                                        TractorTask::subtask_at,
                                        TractorTask::subtasks_clear);
}

void TractorTask::addSubtask(TractorTask *task)
{
    task->setParent(this);
    m_subtasks.append(task);
}

// subtasks property
void TractorTask::subtasks_append(QQmlListProperty<TractorTask> *prop, TractorTask *p)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);

    p->setParent(that);
    that->m_subtasks.append(p);
    emit that->subtasksChanged();
}

int TractorTask::subtasks_count(QQmlListProperty<TractorTask> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    return that->m_subtasks.count();
}

TractorTask *TractorTask::subtask_at(QQmlListProperty<TractorTask> *prop, int i)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    if (i < that->m_subtasks.count())
        return that->m_subtasks[i];
    else
        return 0;
}

void TractorTask::subtasks_clear(QQmlListProperty<TractorTask> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    foreach (TractorTask *p, that->m_subtasks) {
        p->setParent(0);
    }
    that->m_subtasks.clear();
    emit that->subtasksChanged();
}

QQmlListProperty<TractorCmd> TractorTask::cmds_()
{
    return QQmlListProperty<TractorCmd>(this, 0, TractorTask::cmds_append,
                                        TractorTask::cmds_count,
                                        TractorTask::cmd_at,
                                        TractorTask::cmds_clear);
}

void TractorTask::addCmd(TractorCmd *cmd)
{
    cmd->setParent(this);
    m_cmds.append(cmd);
}

// cmds property
void TractorTask::cmds_append(QQmlListProperty<TractorCmd> *prop, TractorCmd *p)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);

    p->setParent(that);
    that->m_cmds.append(p);
    emit that->cmdsChanged();
}

int TractorTask::cmds_count(QQmlListProperty<TractorCmd> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    return that->m_cmds.count();
}

TractorCmd *TractorTask::cmd_at(QQmlListProperty<TractorCmd> *prop, int i)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    if (i < that->m_cmds.count())
        return that->m_cmds[i];
    else
        return 0;
}

void TractorTask::cmds_clear(QQmlListProperty<TractorCmd> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    foreach (TractorCmd *p, that->m_cmds) {
        p->setParent(0);
    }
    that->m_cmds.clear();
    emit that->cmdsChanged();
}

QQmlListProperty<TractorCmd> TractorTask::cleanup_()
{
    return QQmlListProperty<TractorCmd>(this, 0, TractorTask::cleanup_append,
                                        TractorTask::cleanup_count,
                                        TractorTask::cleanup_at,
                                        TractorTask::cleanup_clear);
}

// cleanup property
void TractorTask::cleanup_append(QQmlListProperty<TractorCmd> *prop, TractorCmd *p)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);

    p->setParent(that);
    that->m_cleanup.append(p);
    emit that->cleanupChanged();
}

int TractorTask::cleanup_count(QQmlListProperty<TractorCmd> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    return that->m_cleanup.count();
}

TractorCmd *TractorTask::cleanup_at(QQmlListProperty<TractorCmd> *prop, int i)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    if (i < that->m_cleanup.count())
        return that->m_cleanup[i];
    else
        return 0;
}

void TractorTask::cleanup_clear(QQmlListProperty<TractorCmd> *prop)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);
    foreach (TractorCmd *p, that->m_cleanup) {
        p->setParent(0);
    }
    that->m_cleanup.clear();
    emit that->cleanupChanged();
}

void TractorTask::addCleanup(TractorCmd *cmd)
{
    cmd->setParent(this);
    m_cleanup.append(cmd);
}

const QString TractorTask::asString(int indent) const
{
    QString s;
    QTextStream stream(&s);

    QString spaces(indent, ' ');

    stream << spaces << "Task";
    stream << " -title { " << m_title << " }";

    if (m_serialSubtasksFlag)
        stream << " -serialsubtasks {1}";

    if (m_subtasks.length() > 0) {
        stream << " -subtasks {" << endl;

        foreach (const TractorTask *subtask, m_subtasks) {
            stream << subtask->asString(indent + 4);
        }

        stream << spaces << "}" << endl;
    }

    if (m_cmds.length() > 0) {
        stream << " -cmds {" << endl;

        foreach (const TractorCmd *cmd, m_cmds) {
            stream << cmd->asString(indent + 4);
        }

        stream << spaces << "}" << endl;
    }

    if (m_cleanup.length() > 0) {
        stream << " -cleanup {" << endl;

        foreach (const TractorCmd *cmd, m_cleanup) {
            stream << cmd->asString(indent + 4);
        }

        stream << spaces << "}" << endl;
    }

    return s;
}