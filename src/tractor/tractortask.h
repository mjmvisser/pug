#ifndef TRACTORTASK_H
#define TRACTORTASK_H

#include <QList>
#include <QQmlListProperty>
#include <QString>

#include "tractorblock.h"

class TractorCmd;

class TractorTask : public TractorBlock
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool serialSubtasks READ hasSerialSubtasks WRITE setSerialSubtasks NOTIFY serialSubtasksChanged)
    Q_PROPERTY(QQmlListProperty<TractorTask> subtasks READ subtasks_ NOTIFY subtasksChanged)
    Q_PROPERTY(QQmlListProperty<TractorCmd> cmds READ cmds_ NOTIFY cmdsChanged)
    Q_PROPERTY(QQmlListProperty<TractorCmd> cleanup READ cleanup_ NOTIFY cleanupChanged)
public:
    TractorTask(QObject *parent = 0);

    const QString &title() const;
    void setTitle(const QString &);

    bool hasSerialSubtasks() const;
    void setSerialSubtasks(bool);

    QQmlListProperty<TractorTask> subtasks_();
    void addSubtask(TractorTask *);

    QQmlListProperty<TractorCmd> cmds_();
    void addCmd(TractorCmd *);

    QQmlListProperty<TractorCmd> cleanup_();
    void addCleanup(TractorCmd *);

    Q_INVOKABLE virtual const QString asString(int indent = 0) const;

signals:
    void titleChanged(const QString &title);
    void serialSubtasksChanged(bool serialSubtasks);
    void subtasksChanged();
    void cmdsChanged();
    void cleanupChanged();

private:
    // subtasks property
    static void subtasks_append(QQmlListProperty<TractorTask> *, TractorTask *);
    static int subtasks_count(QQmlListProperty<TractorTask> *);
    static TractorTask *subtask_at(QQmlListProperty<TractorTask> *, int);
    static void subtasks_clear(QQmlListProperty<TractorTask> *);

    // cmds property
    static void cmds_append(QQmlListProperty<TractorCmd> *, TractorCmd *);
    static int cmds_count(QQmlListProperty<TractorCmd> *);
    static TractorCmd *cmd_at(QQmlListProperty<TractorCmd> *, int);
    static void cmds_clear(QQmlListProperty<TractorCmd> *);

    // cleanup property
    static void cleanup_append(QQmlListProperty<TractorCmd> *, TractorCmd *);
    static int cleanup_count(QQmlListProperty<TractorCmd> *);
    static TractorCmd *cleanup_at(QQmlListProperty<TractorCmd> *, int);
    static void cleanup_clear(QQmlListProperty<TractorCmd> *);

    QString m_title;
    bool m_serialSubtasksFlag;
    QList<TractorTask *> m_subtasks;
    QList<TractorCmd *> m_cmds;
    QList<TractorCmd *> m_cleanup;
};

#endif