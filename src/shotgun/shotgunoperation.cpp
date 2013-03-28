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
    bool havePushedAtIndexSignal = node()->hasSignal(SIGNAL(shotgunPushedAtIndex(int, int)));

    bool havePushSignal = node()->hasSignal(SIGNAL(shotgunPush(const QVariant, Shotgun *)));
    bool havePushedSignal = node()->hasSignal(SIGNAL(shotgunPushed(int)));

    bool havePullAtIndexSignal = node()->hasSignal(SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)));
    bool havePulledAtIndexSignal = node()->hasSignal(SIGNAL(shotgunPulledAtIndex(int, int)));

    bool havePullSignal = node()->hasSignal(SIGNAL(shotgunPull(const QVariant, Shotgun *)));
    bool havePulledSignal = node()->hasSignal(SIGNAL(shotgunPulled(int)));


    if (havePushAtIndexSignal && havePushedAtIndexSignal) {
        connect(this, SIGNAL(shotgunPushAtIndex(int, const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPushAtIndex(int, const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPushedAtIndex(int, int)),
                this, SLOT(onShotgunPushedAtIndex(int, int)));
        m_mode |= ShotgunOperationAttached::PushAtIndex;
        debug() << node() << "found .pushAtIndex";
    } else if (!havePushAtIndexSignal && havePushedAtIndexSignal) {
        error() << node() << "is missing the pushAtIndex signal";
    } else if (havePushAtIndexSignal && !havePushedAtIndexSignal) {
        error() << node() << "is missing the pushedAtIndex signal";
    }

    if (havePushSignal && havePushedSignal) {
        connect(this, SIGNAL(shotgunPush(const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPush(const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPushed(int)),
                this, SLOT(onShotgunPushed(int)));
        m_mode |= ShotgunOperationAttached::Push;
        debug() << node() << "found .push";
    } else if (!havePushSignal && havePushedSignal) {
        error() << node() << "is missing the push signal";
    } else if (havePushSignal && !havePushedSignal) {
        error() << node() << "is missing the pushed signal";
    }

    if (havePullAtIndexSignal && havePulledAtIndexSignal) {
        connect(this, SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPullAtIndex(int, const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPulledAtIndex(int, int)),
                this, SLOT(onShotgunPulledAtIndex(int, int)));
        m_mode |= ShotgunOperationAttached::PullAtIndex;
        debug() << node() << "found .pullAtIndex";
    } else if (!havePullAtIndexSignal && havePulledAtIndexSignal) {
        error() << node() << "is missing the pullAtIndex signal";
    } else if (havePullAtIndexSignal && !havePulledAtIndexSignal) {
        error() << node() << "is missing the pulledAtIndex signal";
    }

    if (havePullSignal && havePulledSignal) {
        connect(this, SIGNAL(shotgunPull(const QVariant, Shotgun *)),
                node(), SIGNAL(shotgunPull(const QVariant, Shotgun *)));
        connect(node(), SIGNAL(shotgunPulled(int)),
                this, SLOT(onShotgunPulled(int)));
        m_mode |= ShotgunOperationAttached::Pull;
        debug() << node() << "found .pull";
    } else if (!havePullSignal && havePulledSignal) {
        error() << node() << "is missing the pull signal";
    } else if (havePullSignal && !havePulledSignal) {
        error() << node() << "is missing the pulled signal";
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
    m_pulledAtIndexCount = 0;
    m_pushedAtIndexCount = 0;

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
                    warning() << node() << "has nothing to pull";
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
                    warning() << node() << "has nothing to push";
                    setStatus(OperationAttached::Finished);
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

void ShotgunOperationAttached::onShotgunPulled(int s)
{
    trace() << node() << ".onShotgunPulled(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pulled" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void ShotgunOperationAttached::onShotgunPulledAtIndex(int index, int s)
{
    trace() << node() << ".onShotgunPulledAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pulled" << node() << "at index" << m_pulledAtIndexCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_pulledAtIndexCount++;

    if (m_pulledAtIndexCount == node()->count() * receivers(SIGNAL(shotgunPullAtIndex(int,QVariant,Shotgun*)))) {
        info() << "Shotgun pulled" << node() << "with status" << status();
        info() << "Result is" << node()->details().toVariant();
        setStatus(m_indexStatus.status());
        continueRunning();
    }
}

void ShotgunOperationAttached::onShotgunPushed(int s)
{
    trace() << node() << ".onShotgunPushed(" << static_cast<OperationAttached::Status>(s) << ")";
    setStatus(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pushed" << node() << "with status" << status();
    info() << "Result is" << node()->details().toVariant();

    continueRunning();
}

void ShotgunOperationAttached::onShotgunPushedAtIndex(int index, int s)
{
    trace() << node() << ".onShotgunPushedAtIndex(" << index << "," << static_cast<OperationAttached::Status>(s) << ")";
    Q_ASSERT(operation());
    m_indexStatus.append(static_cast<OperationAttached::Status>(s));

    info() << "Shotgun pushed" << node() << "at index" << m_pushedAtIndexCount << "with status" << static_cast<OperationAttached::Status>(s);
    info() << "Result is" << node()->details().property(index).toVariant();
    m_pushedAtIndexCount++;

    if (m_pushedAtIndexCount == node()->count() * receivers(SIGNAL(shotgunPushAtIndex(int,QVariant,Shotgun*)))) {
        info() << "Shotgun pushed" << node() << "with status" << status();
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
