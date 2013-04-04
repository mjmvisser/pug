#ifndef BRANCH_H
#define BRANCH_H

#include "branch.h"
#include "fileopqueue.h"

class Folder : public Branch
{
    Q_OBJECT
public:
    explicit Folder(QObject *parent = 0);

signals:
    void update(const QVariant context);
    void updateFinished(int status);

    void release(const QVariant context);
    void releaseFinished(int status);

private slots:
    void onUpdate(const QVariant context);
    void onRelease(const QVariant context);

    void onFileOpQueueFinished();
    void onFileOpQueueError();

private:
    FileOpQueue* m_queue;
};

Q_DECLARE_METATYPE(Folder*) // makes available to QVariant

#endif // BRANCH_H
