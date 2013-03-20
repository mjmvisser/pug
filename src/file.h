#ifndef FILE_H
#define FILE_H

#include "branch.h"
#include "node.h"

class File : public Branch
{
    Q_OBJECT
    Q_PROPERTY(Node *input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(LinkType linkType READ linkType WRITE setLinkType NOTIFY linkTypeChanged)
    Q_ENUMS(LinkType)
public:
    enum LinkType { Hard, Symbolic };

    explicit File(QObject *parent = 0);

    Node *input();
    const Node *input() const;
    void setInput(Node *);

    LinkType linkType() const;
    void setLinkType(LinkType);

signals:
    void inputChanged(Node *input);
    void linkTypeChanged(LinkType linkType);

    void update(const QVariant context);
    void updated(int status);

    void cookAtIndex(int index, const QVariant context);
    void cookedAtIndex(int index, int status);

protected slots:
    void onUpdate(const QVariant context);
    void onCookAtIndex(int index, const QVariant context);

private:
    bool makeLink(const QString &src, const QString &dest) const;

    Node *m_input;
    LinkType m_linkType;
};
Q_DECLARE_METATYPE(File*) // makes available to QVariant

#endif // FILE_H
