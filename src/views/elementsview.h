#ifndef ELEMENTSVIEW_H
#define ELEMENTSVIEW_H

#include <QQmlListProperty>

#include "detailsview.h"
#include "elementview.h"
#include "node.h"

class ElementsView : public DetailsView
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ElementView> elements READ elements_ NOTIFY elementsChanged())
public:
    explicit ElementsView(Node *node, QObject *parent = 0);

    QQmlListProperty<ElementView> elements_();

    const ElementView *elementAt(int index) const;
    ElementView *elementAt(int index);

    int elementCount() const;

    const QDateTime timestamp() const;

    Node::Status status() const;

public slots:
    void sync();

signals:
    void elementsChanged();

private:
    // elements property
    static int elements_count(QQmlListProperty<ElementView> *);
    static ElementView *element_at(QQmlListProperty<ElementView> *, int);

    QList<ElementView *> m_elements;
};

#endif
