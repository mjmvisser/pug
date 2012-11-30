#include <QDebug>
#include <QProcess>

#include "fileopqueue.h"

FileOpQueue::FileOpQueue(QObject *parent) :
    PugItem(parent),
    m_process(new QProcess(this))
{
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &FileOpQueue::onProcessFinished);
}

const QString FileOpQueue::user() const
{
    return m_user;
}

void FileOpQueue::setUser(const QString u)
{
    if (m_user != u) {
        m_user = u;
        emit userChanged(m_user);
    }
}

const QString FileOpQueue::group() const
{
    return m_group;
}

void FileOpQueue::setGroup(const QString g)
{
    if (m_group != g) {
        m_group = g;
        emit groupChanged(m_group);
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

void FileOpQueue::run(const QVariantMap env)
{
    m_env = env;
    continueRunning();
}

void FileOpQueue::continueRunning()
{
    //qDebug() << this << ".continueRunning" << "queue size" << m_workQueue.length();
    if (!m_workQueue.isEmpty()) {
        // get the next item
        FileOpQueue::Item item = m_workQueue.dequeue();

        // default environment
        QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();

        // add our env
        for (QVariantMap::const_iterator i = m_env.constBegin(); i != m_env.constEnd(); ++i) {
            if (i.value().canConvert<QString>())
                processEnv.insert(i.key(), i.value().toString());
        }

        m_process->setProcessEnvironment(processEnv);

        QStringList argv;

        if (!m_user.isEmpty()) {
            argv << "sudo";

            argv << "-u" << m_user;

            if (!m_group.isEmpty())
                argv << "-g" << m_group;
        }

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

        copious() << argv;

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

