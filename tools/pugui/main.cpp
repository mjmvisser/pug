#include <QTimer>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QStringList>
#include <QQuickView>

#include "graphconnection.h"

#include <Pug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QByteArray pugRoot = qgetenv("PUGROOT");

    if (pugRoot.isEmpty()) {
        qWarning() << "PUGROOT not set.";
        return 1;
    }

    QGuiApplication app(argc, argv);

    QQmlEngine* engine = new QQmlEngine;
    Pug *pug = new Pug(engine);

    qmlRegisterType<GraphConnection>("PugUI", 1, 0, "GraphConnection");

    QEventLoop eventLoop;
    QObject::connect(pug, &Pug::loaded, &eventLoop, &QEventLoop::quit);
    QObject::connect(pug, static_cast<void (Pug::*)()>(&Pug::error), &eventLoop, &QEventLoop::quit);
    //static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished)
    QMetaObject::invokeMethod(pug, "load", Qt::QueuedConnection, Q_ARG(QString, pugRoot));
    eventLoop.exec();

    engine->rootContext()->setContextProperty("PUG", pug);

    QString currentDirPath = qgetenv("PWD");
    currentDirPath += "/";

    QQuickView view(engine, 0);
    const QUrl url("qrc:///pugui.qml");
    view.connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setSource(url);
    view.show();
    return app.exec();
}
