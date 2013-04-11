#include <QTimer>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QStringList>

#include <Pug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QByteArray pugRoot = qgetenv("PUGROOT");

    if (pugRoot.isEmpty()) {
        qWarning() << "PUGROOT not set.";
        return 1;
    }

    QCoreApplication app(argc, argv);

    QQmlEngine* engine = new QQmlEngine;
    Pug *pug = new Pug(engine);

    QEventLoop eventLoop;
    QObject::connect(pug, &Pug::loaded, &eventLoop, &QEventLoop::quit);
    QObject::connect(pug, static_cast<void (Pug::*)()>(&Pug::error), &eventLoop, &QEventLoop::quit);
    //static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished)
    QMetaObject::invokeMethod(pug, "load", Qt::QueuedConnection, Q_ARG(QString, pugRoot));
    eventLoop.exec();

//    QString currentDirPath = qgetenv("PWD");
//    currentDirPath += "/";

    bool printStatus = false;

    if (app.arguments().length() > 1) {
        QStringList args = app.arguments().mid(1);
        QString opName = args.takeFirst();

        QString nodePath;
//        QString cwd = currentDirPath;
        QVariantMap properties, context;

        while (args.length() > 0) {
            QString arg = args.takeFirst();

            if (arg == "-status") {
                printStatus = true;
            } else if (arg == "-properties") {
                while (args.length() > 0 && !args.first().startsWith('-')) {
                    QString prop = args.takeFirst();
                    QStringList propParts = prop.split('=');
                    if (propParts.length() != 2) {
                        qWarning() << "Skipping badly formatted property:" << prop;
                    } else {
                        properties.insert(propParts[0], propParts[1]);
                    }
                }
            } else if (arg == "-context") {
                while (args.length() > 0 && !args.first().startsWith('-')) {
                    QString data = args.takeFirst();
                    QStringList dataParts = data.split('=');
                    if (dataParts.length() != 2) {
                        qWarning() << "Skipping badly formatted context data:" << data;
                    } else {
                        context.insert(dataParts[0], dataParts[1]);
                    }
                }
            } else if (!arg.startsWith('-')) {
                nodePath = arg;
//                QString path = arg;
//                QFileInfo pathInfo(path);
//                if (pathInfo.isDir())
//                    path += "/";
//                cwd = path;
            } else if (arg == "--") {
                // pass
            } else {
                qWarning() << "Skipping unknown parameter:" << arg;
            }
        }

        OperationAttached::Status status = pug->runOperation(opName, properties, nodePath, context);

        if (printStatus) {
            pug->root()->printStatus();
        }

        qDebug() << status;
        if (status == OperationAttached::Finished)
            return 0;
        else
            return 1;
    } else {
        qDebug() << "usage:" << app.arguments()[0].toUtf8().constData() << "COMMAND" << "[-properties KEY=VALUE ...]" << "[-context KEY=VALUE ...]" << "[PATH]";
        return 1;
    }
}
