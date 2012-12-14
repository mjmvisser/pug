#ifndef FILE_H
#define FILE_H

#include "branchbase.h"

class Node;
class FileOpQueue;

class File : public BranchBase
{
    Q_OBJECT
    Q_PROPERTY(NodeBase *input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(LinkType linkType READ linkType WRITE setLinkType NOTIFY linkTypeChanged)
    Q_ENUMS(LinkType)
public:
    enum LinkType { Hard, Symbolic };

    explicit File(QObject *parent = 0);

    NodeBase *input();
    const NodeBase *input() const;
    void setInput(NodeBase *);

    LinkType linkType() const;
    void setLinkType(LinkType);

signals:
    void inputChanged(NodeBase *input);
    void linkTypeChanged(LinkType linkType);

    void cook(const QVariant context);
    void cooked(int status);

protected slots:
    void onUpdate(const QVariant context);
    void onCook(const QVariant context);

private:
    NodeBase *m_input;
    LinkType m_linkType;
};
Q_DECLARE_METATYPE(File*) // makes available to QVariant

#endif // FILE_H
