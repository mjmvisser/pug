#include <QDebug>
#include <QJsonDocument>

#include "shotgunoperation.h"
#include "shotgunentity.h"
#include "shotgunfield.h"
#include "branch.h"

#include "releaseoperation.h"

ShotgunOperationAttached::ShotgunOperationAttached(QObject *object) :
    OperationAttached(object),
    m_action(ShotgunOperationAttached::Find),
    m_mode(ShotgunOperationAttached::Skip)
{
    setObjectName("shotgun");
    bool havePushAtIndexSignal = node()->hasSignal(SIGNAL(shotgunPushAtIndex(int, const QVariant, Shotgun *)));
    bool havePushAtIndexFinishedSignal = node()->hasSignal(SIGNAL(shotgunPushAtIndexFinished(int, int)));

    bool havePushSignal = node()->hasSignal(SIGNAL(shotgunPush(const QVariant, Shotgun *)));
    bool havePushFinishedSignal = node()->hasSignal(SIGNAL(shotgunPushFinished(int)));

    bool havePullAtIndexSignal = node()->hasSignal(SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)));
    bool havePullAtIndexFinishedSignal = node()->hasSignal(SIGNAL(shotgunPullAtIndexFinished(int, int)));

    bool havePullSignal = node()->hasSignal(SIGNAL(shotgunPull(const QVariant, Shotgun *)));
    bool havePullFinishedSignal = node()->hasSignal(SIGNAL(shotgunPullFinished(int)));


    if (havePushAtIndexSignal && havePushAtIndexFinishedSignal) {
        connect(this, SIGNAL(shotgunPushAtIndex(int, const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPushAtIndex(int, const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPushAtIndexFinished(int, int)),
                this, SLOT(onShotgunPushAtIndexFinished(int, int)));
        m_mode |= ShotgunOperationAttached::PushAtIndex;
        debug() << node() << "found .pushAtIndex";
    } else if (!havePushAtIndexSignal && havePushAtIndexFinishedSignal) {
        error() << node() << "is missing the pushAtIndex signal";
    } else if (havePushAtIndexSignal && !havePushAtIndexFinishedSignal) {
        error() << node() << "is missing the pushAtIndexFinished signal";
    }

    if (havePushSignal && havePushFinishedSignal) {
        connect(this, SIGNAL(shotgunPush(const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPush(const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPushFinished(int)),
                this, SLOT(onShotgunPushFinished(int)));
        m_mode |= ShotgunOperationAttached::Push;
        debug() << node() << "found .push";
    } else if (!havePushSignal && havePushFinishedSignal) {
        error() << node() << "is missing the push signal";
    } else if (havePushSignal && !havePushFinishedSignal) {
        error() << node() << "is missing the pushFinished signal";
    }

    if (havePullAtIndexSignal && havePullAtIndexFinishedSignal) {
        connect(this, SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPullAtIndexFinished(int, int)),
                this, SLOT(onShotgunPullAtIndexFinished(int, int)));
        m_mode |= ShotgunOperationAttached::PullAtIndex;
        debug() << node() << "found .pullAtIndex";
    } else if (!havePullAtIndexSignal && havePullAtIndexFinishedSignal) {
        error() << node() << "is missing the pullAtIndex signal";
    } else if (havePullAtIndexSignal && !havePullAtIndexFinishedSignal) {
        error() << node() << "is missing the pullAtIndexFinished signal";
    }

    if (havePullSignal && havePullFinishedSignal) {
        connect(this, SIGNAL(shotgunPull(const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPull(const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPullFinished(int)),
                this, SLOT(onShotgunPullFinished(int)));
        m_mode |= ShotgunOperationAttached::Pull;
        debug() << node() << "found .pull";
    } else if (!havePullSignal && havePullFinishedSignal) {
        error() << node() << "is missing the pull signal";
    } else if (havePullSignal && !havePullFinishedSignal) {
        error() << node() << "is missing the pullFinished signal";
    }
}

ShotgunOperationAttached::Action ShotgunOperationAttached::action() const
{
    return m_action;
}

void ShotgunOperationAttached::setAction(ShotgunOperationAttached::Action a)
{
    if (m_action != a) {
        m_action = a;
        emit actionChanged(m_action);
    }
}

void ShotgunOperationAttached::run()
{
    ShotgunOperation *sgop = operation<ShotgunOperation>();
    Q_ASSERT(sgop);

    trace() << node() << operation() << ".run [mode is" << sgop->node() << "]";

    if (!sgop->shotgun()) {
        error() << sgop << "has no shotgun object";
        setStatus(OperationAttached::Error);
        continueRunning();
        return;
    }

    m_indexStatus.clear();
    m_pullAtIndexFinishedCount = 0;
    m_pushAtIndexFinishedCount = 0;

    if (sgop->mode() == ShotgunOperation::Pull) {
        if (m_action == ShotgunOperationAttached::Find) {
            info() << "Shotgun pull on" << node() << "with" << context();
            trace() << node() << ".run() [cook mode is" << m_mode << "]";

            m_indexStatus.clear();
            if (m_mode & ShotgunOperationAttached::PullAtIndex) {
                if (node()->count() > 0) {
                    debug() << "Shotgun pulling" << node()->count() << "details";
                    for (int i = 0; i < node()->count(); i++) {
                        info() << "Shotgun pulling" << node() << "at index" << i << "with" << context();
                        emit shotgunPullAtIndex(i, context(), sgop->shotgun());
                    }
                } else {
                    debug() << node() << "(parent" << node()->QObject::parent() << ") has nothing to pull";
                    setStatus(OperationAttached::Finished);
                    continueRunning();
                }
            }
            else if (m_mode & ShotgunOperationAttached::Pull) {
                emit shotgunPull(context(), sgop->shotgun());
            } else {
                // skip
                setStatus(OperationAttached::Finished);
                continueRunning();
            }
        } else {
            // skip
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
    } else if (sgop->mode() == ShotgunOperation::Push) {
        if (m_action == ShotgunOperationAttached::Create) {
            info() << "Shotgun push on" << node() << "with" << context();
            trace() << node() << ".run() [cook mode is" << m_mode << "]";

            m_indexStatus.clear();
            if (m_mode & ShotgunOperationAttached::PushAtIndex) {
                if (node()->count() > 0) {
                    debug() << "Shotgun pushing" << node()->count() << "details";
                    for (int i = 0; i < node()->count(); i++) {
                        info() << "Shotgun pushing" << node() << "at index" << i << "with" << context();
                        emit shotgunPushAtIndex(i, context(), sgop->shotgun());
                    }
                } else {
                    error() << node() << "(parent" << node()->QObject::parent() << ") has nothing to push";
                    setStatus(OperationAttached::Error);
                    continueRunning();
                }
            } else if (m_mode & ShotgunOperationAttached::Push) {
                emit shotgunPush(context(), sgop->shotgun());
            } else {
                // skip
                setStatus(OperationAttached::Finished);
                continueRunning();
            }
        } else {
            // skip
            setStatus(OperationAttached::Finished);
            continueRunning();
        }
    } else {
        error() << "Unknown Shotgun mode" << sgop->mode();
        setStatus(OperationAttached::Error);
        continueRunning();
    }
}

void ShotgunOperationAttached::onShotgunPullFinished(int s)
{
    trace() << node() << ".onShotgunPullFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pullFinished" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void ShotgunOperationAttached::onShotgunPullAtIndexFinished(int index, int s)
{
    trace() << node() << ".onShotgunPullAtIndexFinished(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pullFinished" << node() << "at index" << m_pullAtIndexFinishedCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_pullAtIndexFinishedCount++;

    if (m_pullAtIndexFinishedCount == node()->count() * receivers(SIGNAL(shotgunPullAtIndex(int,QVariant,Shotgun*)))) {
        info() << "Shotgun pullFinished" << node() << "with status" << status();
        info() << "Result is" << node()->details().toVariant();
        setStatus(m_indexStatus.status());
        continueRunning();
    }
}

void ShotgunOperationAttached::onShotgunPushFinished(int s)
{
    trace() << node() << ".onShotgunPushFinished(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pushFinished" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void ShotgunOperationAttached::onShotgunPushAtIndexFinished(int index, int s)
{
    trace() << node() << ".onShotgunPushAtIndexFinished(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pushFinished" << node() << "at index" << m_pushAtIndexFinishedCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_pushAtIndexFinishedCount++;

    if (m_pushAtIndexFinishedCount == node()->count() * receivers(SIGNAL(shotgunPushAtIndex(int,QVariant,Shotgun*)))) {
        info() << "Shotgun pushFinished" << node() << "with status" << status();
        info() << "Result is" << node()->details().toVariant();
        setStatus(m_indexStatus.status());
        continueRunning();
    }
}

const QMetaObject *ShotgunOperationAttached::operationMetaObject() const
{
    return &ShotgunOperation::staticMetaObject;
}

ShotgunOperation::ShotgunOperation(QObject *parent) :
    Operation(parent),
    m_shotgun()
{
}

ShotgunOperation::Mode ShotgunOperation::mode() const
{
    return m_mode;
}

void ShotgunOperation::setMode(ShotgunOperation::Mode m)
{
    m_mode = m;
}

Shotgun *ShotgunOperation::shotgun()
{
    return m_shotgun;
}

void ShotgunOperation::setShotgun(Shotgun *sg)
{
    m_shotgun = sg;
}

ShotgunOperationAttached *ShotgunOperation::qmlAttachedProperties(QObject *object)
{
    return new ShotgunOperationAttached(object);
}

QDebug operator<<(QDebug dbg, ShotgunOperation::Mode mode)
{
    switch(mode) {
    case ShotgunOperation::Pull:
        dbg << "Pull";
        break;
    case ShotgunOperation::Push:
        dbg << "Push";
        break;
    default:
        dbg << "Unknown!";
        break;
    }

    return dbg;
}
