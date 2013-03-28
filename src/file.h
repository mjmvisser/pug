#ifndef FILE_H
#define FILE_H

#include "branch.h"
#include "node.h"
#include "framelist.h"

class File : public Branch
{
    Q_OBJECT
    Q_PROPERTY(Node *input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(LinkType linkType READ linkType WRITE setLinkType NOTIFY linkTypeChanged)
    Q_PROPERTY(FrameList *frames READ frames WRITE setFrames NOTIFY framesChanged)
    Q_ENUMS(LinkType)
public:
    enum LinkType { Hard, Symbolic };

    explicit File(QObject *parent = 0);

    Node *input();
    const Node *input() const;
    void setInput(Node *);

    LinkType linkType() const;
    void setLinkType(LinkType);

    FrameList *frames();
    void setFrames(FrameList *frames);

signals:
    void inputChanged(Node *input);
    void linkTypeChanged(LinkType linkType);
    void framesChanged(FrameList *frames);

    void update(const QVariant context);
    void updateFinished(int status);

    void cookAtIndex(int index, const QVariant context);
    void cookAtIndexFinished(int index, int status);

protected slots:
    void onUpdate(const QVariant context);
    void onCookAtIndex(int index, const QVariant context);

private:
    bool makeLink(const QString &src, const QString &dest) const;

    Node *m_input;
    LinkType m_linkType;
    FrameList *m_frames;
};
Q_DECLARE_METATYPE(File*) // makes available to QVariant

#endif // FILE_H
