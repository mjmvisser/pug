#ifndef PUG_H
#define PUG_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QUrl>
#include <QSocketNotifier>

#include "root.h"

class Pug : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(Root *root READ root NOTIFY loaded)
public:
    explicit Pug(QQmlEngine* engine, QObject *parent = 0);
    
    Root *root();

    Q_INVOKABLE void load(const QString path);

    Q_INVOKABLE const QVariantMap parse(const QString node, const QString path) const;
    Q_INVOKABLE const QString map(const QString node, const QVariantMap fields) const;

    Q_INVOKABLE OperationAttached::Status runOperation(
            const QString operationName,
            const QVariantMap properties,
            const QString path,
            const QVariantMap data);


    static void intSignalHandler(int);

signals:
    void rootPathChanged();
    void rootChanged();
    void loaded();
    void error();

private slots:
    void handleSigInt(int);
    void continueLoading();
    void onWarnings(const QList<QQmlError> &);

private:
    QQmlEngine* m_engine;
    QQmlComponent* m_rootComponent;
    Root *m_root;
    QSocketNotifier *m_sigIntSocketNotifier;
    static int s_sigIntFd[2];
};

#endif // PUG_H
