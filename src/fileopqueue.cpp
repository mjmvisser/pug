#include <QDebug>
#include <QProcess>

#include "fileopqueue.h"

FileOpQueue::FileOpQueue(QObject *parent) :
    PugItem(parent),
    m_process(new QProcess(this)),
    m_sudo()
{
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &FileOpQueue::onProcessFinished);
}

Sudo *FileOpQueue::sudo()
{
    return m_sudo;
}

void FileOpQueue::setSudo(Sudo *sudo)
{
    if (m_sudo != sudo) {
        m_sudo = sudo;
        emit sudoChanged(sudo);
    }
}

void FileOpQueue::mkdir(const QString dir)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Mkdir, dir));
}

void FileOpQueue::chmod(const QString mode, const QString target)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Chmod, mode, target));
}

void FileOpQueue::copy(const QString src, const QString dest)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Copy, src, dest));
}

void FileOpQueue::move(const QString src, const QString dest)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Move, src, dest));
}

void FileOpQueue::symlink(const QString target, const QString dest)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Symlink, target, dest));
}

void FileOpQueue::hardlink(const QString target, const QString dest)
{
    m_workQueue.enqueue(FileOpQueue::Item(FileOpQueue::Item::Hardlink, target, dest));
}

void FileOpQueue::run()
{
    continueRunning();
}

void FileOpQueue::continueRunning()
{
    trace() << ".continueRunning() [queue size " << m_workQueue.length() << "]";
    if (!m_workQueue.isEmpty()) {
        // get the next item
        FileOpQueue::Item item = m_workQueue.dequeue();

        // default environment
        QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();

        m_process->setProcessEnvironment(processEnv);

        QStringList argv;

        switch (item.type) {
        case FileOpQueue::Item::Mkdir:
            argv << "mkdir" << "-p" << item.args;
            break;
        case FileOpQueue::Item::Copy:
            argv << "cp" << item.args;
            break;
        case FileOpQueue::Item::Move:
            argv << "mv" << item.args;
            break;
        case FileOpQueue::Item::Symlink:
            argv << "ln" << "-s" << item.args;
            break;
        case FileOpQueue::Item::Hardlink:
            argv << "ln" << item.args;
            break;
        default:
            qWarning() << this << "unknown file mode " << item.type;
            break;
        }

        if (m_sudo) {
            argv = m_sudo->wrapCommand(argv);
        }

        debug() << "running:" << argv;

        const QString program = argv.takeFirst();
        m_process->start(program, argv);
    } else {
        emit finished();
    }
}

void FileOpQueue::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        emit error();
    } else {
        continueRunning();
    }
}

void FileOpQueue::onProcessError(QProcess::ProcessError err)
{
    Q_UNUSED(err);
    emit error();
}

