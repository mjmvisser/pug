#include <QTextStream>

#include "tractorcmd.h"

TractorCmd::TractorCmd(QObject *parent) :
    TractorBlock(parent)
{
    setType("Cmd");
}


const QString &TractorCmd::service() const
{
    return m_service;
}

void TractorCmd::setService(const QString &service)
{
    if (m_service != service) {
        m_service = service;
        emit serviceChanged(service);
    }
}

const QString &TractorCmd::tags() const
{
    return m_tags;
}

void TractorCmd::setTags(const QString &tags)
{
    if (m_tags != tags) {
        m_tags = tags;
        emit tagsChanged(tags);
    }
}

bool TractorCmd::isExpand() const
{
   return m_expandFlag;
}

void TractorCmd::setExpand(bool flag)
{
    if (m_expandFlag != flag) {
        m_expandFlag = flag;
        emit expandChanged(flag);
    }
}

const QString &TractorCmd::cmd() const
{
    return m_cmd;
}

void TractorCmd::setCmd(const QString &cmd)
{
    if (m_cmd != cmd) {
        m_cmd = cmd;
        emit cmdChanged(cmd);
    }
}

void TractorCmd::setType(const QString &type)
{
    m_type = type;
}

const QString TractorCmd::asString(int indent) const
{
    QString s;
    QTextStream stream(&s);

    QString spaces(indent, ' ');

    stream << spaces << m_type;

    if (m_expandFlag)
        stream << " -expand {1}";

    if (m_service.length() > 0)
        stream << " -service {" << m_service << "}";
    else
        stream << " -service {Generic}";

    if (m_tags.length() > 0)
        stream << " -tags {" << m_tags << "}";

    stream << " {" << m_cmd << "}" << endl;

    return s;
}

TractorRemoteCmd::TractorRemoteCmd(QObject *parent) :
    TractorCmd(parent)
{
    setType("RemoteCmd");
}
