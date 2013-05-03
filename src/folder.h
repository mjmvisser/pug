#ifndef BRANCH_H
#define BRANCH_H

#include "branch.h"
#include "fileopqueue.h"

class UpdateOperationAttached;
class ReleaseOperationAttached;

class Folder : public Branch
{
    Q_OBJECT
public:
    explicit Folder(QObject *parent = 0);

protected:
    virtual void componentComplete();

private slots:
    void update_onCook();
    void release_onCook();

    void onFileOpQueueFinished();
    void onFileOpQueueError();

private:
    UpdateOperationAttached *updateOperationAttached();
    ReleaseOperationAttached *releaseOperationAttached();
    FileOpQueue* m_queue;
};
Q_DECLARE_METATYPE(Folder*) // makes available to QVariant

#endif // BRANCH_H
