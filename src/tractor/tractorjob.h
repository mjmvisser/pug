#ifndef TRACTORJOB_H
#define TRACTORJOB_H

#include <QList>
#include <QString>
#include <QQmlListProperty>

#include "tractorblock.h"

class TractorTask;

class TractorJob : public TractorBlock
{
    Q_OBJECT
    Q_PROPERTY(QString envKey READ envKey WRITE setEnvKey NOTIFY envKeyChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
    Q_PROPERTY(QString metadata READ metadata WRITE setMetadata NOTIFY metadataChanged)
    Q_PROPERTY(bool serialSubtasks READ hasSerialSubtasks WRITE setSerialSubtasks NOTIFY serialSubtasksChanged)
    Q_PROPERTY(QQmlListProperty<TractorTask> subtasks READ subtasks_ NOTIFY subtasksChanged)
public:
    TractorJob(QObject *parent = 0);

    const QString &envKey() const;
    void setEnvKey(const QString &);

    const QString &service() const;
    void setService(const QString &);

    const QString &title() const;
    void setTitle(const QString &);

    const QString &comment() const;
    void setComment(const QString &);

    const QString &metadata() const;
    void setMetadata(const QString &);

    bool hasSerialSubtasks() const;
    void setSerialSubtasks(bool);

    QQmlListProperty<TractorTask> subtasks_();
    void addSubtask(TractorTask *);

    Q_INVOKABLE virtual const QString asString(int indent, QSet<const TractorBlock *>&) const;

    Q_INVOKABLE void submit();

signals:
    void envKeyChanged(const QString &envKey);
    void serviceChanged(const QString &service);
    void titleChanged(const QString &title);
    void commentChanged(const QString &comment);
    void metadataChanged(const QString &metadata);
    void serialSubtasksChanged(bool serialSubtasks);
    void subtasksChanged();

private:
    // subtasks property
    static void subtasks_append(QQmlListProperty<TractorTask> *, TractorTask *);
    static int subtasks_count(QQmlListProperty<TractorTask> *);
    static TractorTask *subtask_at(QQmlListProperty<TractorTask> *, int);
    static void subtasks_clear(QQmlListProperty<TractorTask> *);

    QString m_envKey;
    QString m_service;
    QString m_title;
    QString m_comment;
    QString m_metadata;
    bool m_serialSubtasksFlag;
    QList<TractorTask *> m_subtasks;
};

#endif
