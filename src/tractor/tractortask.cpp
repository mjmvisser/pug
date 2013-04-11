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

bool TractorTask::hasSubtasks() const
{
    return !m_subtasks.isEmpty();
}

void TractorTask::addSubtask(TractorTask *task)
{
    m_subtasks.append(task);
}

void TractorTask::clearSubtasks()
{
    qDeleteAll(m_subtasks);
    m_subtasks.clear();
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
    m_cmds.append(cmd);
}

// cmds property
void TractorTask::cmds_append(QQmlListProperty<TractorCmd> *prop, TractorCmd *p)
{
    TractorTask *that = static_cast<TractorTask *>(prop->object);

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
    that->m_cleanup.clear();
    emit that->cleanupChanged();
}

void TractorTask::addCleanup(TractorCmd *cmd)
{
    m_cleanup.append(cmd);
}

const QString TractorTask::asString(int indent, QSet<const TractorBlock *>& visited) const
{
    QString s;
    QTextStream stream(&s);

    QString spaces(indent, ' ');

    if (visited.contains(this)) {
        stream << spaces << "Instance {" << m_title + ":self" << "}" << endl;
    } else {
        visited.insert(this);

        stream << spaces << "Task";
        stream << " -title { " << m_title << " }";

        if (m_serialSubtasksFlag)
            stream << " -serialsubtasks {1}";

        if (!m_subtasks.isEmpty()) {
            stream << " -subtasks {" << endl;
            foreach (const TractorTask *subtask, m_subtasks) {
                stream << subtask->asString(indent + 4, visited) << endl;
            }
            stream << spaces << "}";
        }

        if (!m_cmds.isEmpty()) {
            stream << " -cmds {" << endl;

            foreach (const TractorCmd *cmd, m_cmds) {
                stream << cmd->asString(indent + 4, visited);
            }

            stream << spaces << "}";
        }

        if (!m_cleanup.isEmpty()) {
            stream << " -cleanup {" << endl;

            foreach (const TractorCmd *cmd, m_cleanup) {
                stream << cmd->asString(indent + 4, visited);
            }

            stream << spaces << "}";
        }

        stream << endl;
    }
    return s;
}
