#ifndef FILE_H
#define FILE_H

#include "branch.h"
#include "sudo.h"
#include "fileopqueue.h"

class ElementView;
class ElementsView;
class UpdateOperationAttached;
class CookOperationAttached;
class ReleaseOperationAttached;

class File : public Branch
{
    Q_OBJECT
    Q_PROPERTY(Node *input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(LinkType linkType READ linkType WRITE setLinkType NOTIFY linkTypeChanged)
    Q_PROPERTY(QQmlListProperty<ElementView> elements READ elements_ NOTIFY elementsChanged())
    Q_ENUMS(LinkType)
public:
    enum LinkType { Hard, Symbolic };

    explicit File(QObject *parent = 0);

    Node *input();
    const Node *input() const;
    void setInput(Node *);

    LinkType linkType() const;
    void setLinkType(LinkType);

    QQmlListProperty<ElementView> elements_();

    static ElementsView *qmlAttachedProperties(QObject *);

signals:
    void inputChanged(Node *input);
    void linkTypeChanged(LinkType linkType);
    void elementsChanged();

protected:
    virtual void componentComplete();

private slots:
    void update_onCook();
    void cook_onCook();
    void release_onCook();

    void onFileOpQueueFinished();
    void onFileOpQueueError();

private:
    ElementsView *elementsView();
    UpdateOperationAttached *updateOperationAttached();
    CookOperationAttached *cookOperationAttached();
    ReleaseOperationAttached *releaseOperationAttached();

    bool makeLink(const QString &src, const QString &dest) const;
    void releaseFile(const QString srcPath, const QString destPath, int mode) const;

    Node *m_input;
    LinkType m_linkType;
    FileOpQueue* m_queue;
};
Q_DECLARE_METATYPE(File*) // makes available to QVariant
QML_DECLARE_TYPEINFO(File, QML_HAS_ATTACHED_PROPERTIES)

#endif // FILE_H
