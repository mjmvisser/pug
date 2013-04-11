#ifndef TRACTORCMD_H
#define TRACTORCMD_H

#include <QString>

#include "tractorblock.h"

class TractorCmd : public TractorBlock
{
    Q_OBJECT
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QString tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(bool expand READ isExpand WRITE setExpand NOTIFY expandChanged)
    Q_PROPERTY(QString cmd READ cmd WRITE setCmd NOTIFY cmdChanged)
public:
    TractorCmd(QObject *parent = 0);

    const QString &service() const;
    void setService(const QString &);

    const QString &tags() const;
    void setTags(const QString &);

    bool isExpand() const;
    void setExpand(bool);

    const QString &cmd() const;
    void setCmd(const QString &);

    Q_INVOKABLE virtual const QString asString(int indent, QSet<const TractorBlock *>& visited) const;

signals:
    void serviceChanged(const QString &service);
    void tagsChanged(const QString &tags);
    void expandChanged(bool expand);
    void cmdChanged(const QString &cmd);

protected:
    void setType(const QString &);

private:
    QString m_type;
    QString m_service;
    QString m_tags;
    bool m_expandFlag;
    QString m_cmd;
};

class TractorRemoteCmd : public TractorCmd
{
    Q_OBJECT
public:
    TractorRemoteCmd(QObject *parent = 0);
};

#endif
