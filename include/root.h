#ifndef ROOT_H
#define ROOT_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QUrl>

#include "node.h"

class Root : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Node *scene READ scene)
    Q_PROPERTY(QUrl url READ url)
public:
    explicit Root(QQmlEngine* engine);
    
    QUrl url() { return m_sceneComponent->url(); }
    Node *scene();

    Q_INVOKABLE void loadConfig(const QString path);
    Q_INVOKABLE void loadConfig(const QUrl& url);
    Q_INVOKABLE void loadScene(const QString path);

    Q_INVOKABLE const QVariantMap parse(const QString path) const;
    Q_INVOKABLE const QString map(const QVariantMap dict) const;

signals:
    void configLoaded();
    void sceneLoaded();

public slots:

private slots:
void continueLoadingConfig();
    void continueLoadingScene();

private:
    QQmlEngine* m_engine;
    QQmlComponent* m_configComponent;
    QQmlComponent* m_sceneComponent;
    Node *m_config;
    Node *m_scene;
};

#endif // ROOT_H
