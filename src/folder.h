#ifndef BRANCH_H
#define BRANCH_H

#include "branch.h"
#include "fileopqueue.h"

class Folder : public Branch
{
    Q_OBJECT
public:
    explicit Folder(QObject *parent = 0);

protected:
    virtual void componentComplete();

private slots:
    void update_onCook(const QVariant);
    void release_onCook(const QVariant);

    void onFileOpQueueFinished();
    void onFileOpQueueError();

private:
    FileOpQueue* m_queue;
};

Q_DECLARE_METATYPE(Folder*) // makes available to QVariant

#endif // BRANCH_H
