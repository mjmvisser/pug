#include <QMetaType>

#include "element.h"
#include "framelist.h"
#include "root.h"
#include "param.h"
#include "input.h"
#include "output.h"
#include "node.h"
#include "field.h"
#include "branchbase.h"
#include "branch.h"
#include "file.h"
#include "root.h"
#include "process.h"
#include "operation.h"
#include "testoperation.h"
#include "cookoperation.h"
#include "listoperation.h"
#include "releaseoperation.h"
#include "updateoperation.h"
#include "shotgunoperation.h"
#include "shotgun.h"
#include "shotgunentity.h"
#include "shotgunfield.h"
#include "fileopqueue.h"
#include "util.h"
#include "logger.h"
#include "sudo.h"
#include "nodemodel.h"
#include "tractorblock.h"
#include "tractorjob.h"
#include "tractortask.h"
#include "tractorcmd.h"
#include "tractoroperation.h"

#include "pugplugin.h"
#include "serializers.h"

static QObject *Util_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Util();
}

// template specializations so we can instantiate Element and FrameList

namespace QtMetaTypePrivate {
template <>
struct QMetaTypeFunctionHelper<Element, true> {
    static void Delete(void *t)
    {
        delete static_cast<Element*>(t);
    }

    static void *Create(const void *t)
    {
        Q_UNUSED(t)
        return new Element();
    }

    static void Destruct(void *t)
    {
        Q_UNUSED(t) // Silence MSVC that warns for POD types.
        static_cast<Element*>(t)->~Element();
    }

    static void *Construct(void *where, const void *t)
    {
        Q_UNUSED(t)
        return new (where) Element;
    }
#ifndef QT_NO_DATASTREAM
    static void Save(QDataStream &stream, const void *t)
    {
        stream << *static_cast<const Element*>(t);
    }

    static void Load(QDataStream &stream, void *t)
    {
        stream >> *static_cast<Element*>(t);
    }
#endif // QT_NO_DATASTREAM
};
}

namespace QtMetaTypePrivate {
template <>
struct QMetaTypeFunctionHelper<FrameList, true> {
    static void Delete(void *t)
    {
        delete static_cast<FrameList*>(t);
    }

    static void *Create(const void *t)
    {
        Q_UNUSED(t)
        return new FrameList();
    }

    static void Destruct(void *t)
    {
        Q_UNUSED(t) // Silence MSVC that warns for POD types.
        static_cast<FrameList*>(t)->~FrameList();
    }

    static void *Construct(void *where, const void *t)
    {
        Q_UNUSED(t)
        return new (where) FrameList;
    }
#ifndef QT_NO_DATASTREAM
    static void Save(QDataStream &stream, const void *t)
    {
        stream << *static_cast<const FrameList*>(t);
    }

    static void Load(QDataStream &stream, void *t)
    {
        stream >> *static_cast<FrameList*>(t);
    }
#endif // QT_NO_DATASTREAM
};
}

void PugPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Pug"));

    qRegisterMetaType<Element>("Element");
    qRegisterMetaType<FrameList>("FrameList");
    qmlRegisterType<Root>();
    qmlRegisterType<FrameList>(uri, 1, 0, "FrameList");
    qmlRegisterType<Element>(uri, 1, 0, "Element");
    qmlRegisterType<PugItem>(uri, 1, 0, "PugItem");
    qmlRegisterType<Input>(uri, 1, 0, "Input");
    qmlRegisterType<Output>(uri, 1, 0, "Output");
    qmlRegisterType<Param>(uri, 1, 0, "Param");
    qmlRegisterType<NodeBase>(uri, 1, 0, "NodeBase");
    qmlRegisterType<Node>(uri, 1, 0, "Node");
    qmlRegisterType<Field>(uri, 1, 0, "Field");
    qmlRegisterType<BranchBase>();
    qmlRegisterType<Branch>(uri, 1, 0, "Branch");
    qmlRegisterType<Root>(uri, 1, 0, "Root");
    qmlRegisterType<File>(uri, 1, 0, "File");
    qmlRegisterType<Process>(uri, 1, 0, "Process");
    qmlRegisterType<Operation>(uri, 1, 0, "Operation");
    qmlRegisterType<TestOperation>(uri, 1, 0, "TestOperation");
    qmlRegisterType<CookOperation>(uri, 1, 0, "CookOperation");
    qmlRegisterType<ListOperation>(uri, 1, 0, "ListOperation");
    qmlRegisterType<ReleaseOperation>(uri, 1, 0, "ReleaseOperation");
    qmlRegisterType<Sudo>(uri, 1, 0, "Sudo");
    qmlRegisterType<UpdateOperation>(uri, 1, 0, "UpdateOperation");
    qmlRegisterType<ShotgunOperation>(uri, 1, 0, "ShotgunOperation");
    qmlRegisterType<Shotgun>(uri, 1, 0, "Shotgun");
    qmlRegisterType<ShotgunReply>(uri, 1, 0, "ShotgunReply");
    qmlRegisterType<ShotgunEntity>(uri, 1, 0, "ShotgunEntity");
    qmlRegisterType<ShotgunField>(uri, 1, 0, "ShotgunField");
    qmlRegisterType<FileOpQueue>(uri, 1, 0, "FileOpQueue");
    qmlRegisterType<Log>(uri, 1, 0, "Log");
    qmlRegisterSingletonType<Util>(uri, 1, 0, "Util", Util_provider);
    qmlRegisterType<NodeModel>(uri, 1, 0, "NodeModel");
    qmlRegisterType<TractorBlock>();
    qmlRegisterType<TractorJob>(uri, 1, 0, "TractorJob");
    qmlRegisterType<TractorTask>(uri, 1, 0, "TractorTask");
    qmlRegisterType<TractorCmd>(uri, 1, 0, "TractorCmd");
    qmlRegisterType<TractorOperation>(uri, 1, 0, "TractorOperation");
}
