#include <QCoreApplication>
#include <QTimer>
#include <QQmlEngine>
#include <QVariant>
#include <QDebug>

#include <Pug>

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/dict.hpp>

#include <iostream>

#include "pugapi.h"

static const char *g_argv[] = {"python"}; // TODO: get actual argv[0] from python
static int g_argc = sizeof(g_argv);
static QCoreApplication g_app(g_argc, const_cast<char**>(g_argv));

BOOST_PYTHON_MODULE(pug)
{
    using namespace boost::python;
    class_<PugAPI>("PugAPI", init<std::string>())
        .def("parse", &PugAPI::parse)
        .def("map", &PugAPI::map)
        ;
}

PugAPI::PugAPI(const std::string config)
{
    m_root = new Root(new QQmlEngine);
    // Qt::QueuedConnection is required, otherwise the slot will be called immediately, and
    // since we're not in the event loop yet, quit() will have no effect
    QObject::connect(m_root, SIGNAL(configLoaded()), &g_app, SLOT(quit()), Qt::QueuedConnection);
    m_root->loadConfig(QString::fromStdString(config));
    g_app.exec();
}

PugAPI::~PugAPI()
{
    m_root->deleteLater();
}

boost::python::dict PugAPI::parse(const std::string node, const std::string path) const
{
    boost::python::dict result;

    QVariantMap qresult = m_root->parse(QString::fromStdString(node), QString::fromStdString(path));

    for (QVariantMap::const_iterator it = qresult.constBegin(); it != qresult.constEnd(); ++it) {
        switch(it.value().type()) {
        case QVariant::Int:
            result[it.key().toStdString()] = it.value().toInt();
            break;
        case QVariant::String:
            result[it.key().toStdString()] = it.value().toString().toStdString();
            break;
        default:
            // ignore things we can't convert
            break;
        }
    }

    return result;
}

const std::string PugAPI::map(const std::string node, const boost::python::dict& d) const
{
    QVariantMap qmap;
    boost::python::list keys = (boost::python::list)d.iterkeys();
    for (int i = 0; i < boost::python::len(keys); i++) {
        boost::python::object obj = d[keys[i]];
        if (PyString_Check(obj.ptr())) {
            qmap[QString::fromStdString(boost::python::extract<std::string>(keys[i]))] = QString::fromStdString(boost::python::extract<std::string>(obj));
        } else if (PyInt_Check(obj.ptr())) {
            qmap[QString::fromStdString(boost::python::extract<std::string>(keys[i]))] = QVariant(boost::python::extract<int>(obj));
        }
        // ignore things we can't convert
    }

    QString qresult = m_root->map(QString::fromStdString(node), qmap);

    return qresult.toStdString();
}
