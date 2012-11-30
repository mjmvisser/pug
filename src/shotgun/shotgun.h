#ifndef SHOTGUN_H
#define SHOTGUN_H

#include <QtQml>
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "pugitem.h"

class ShotgunReply;

class ShotgunUploadRequest : public QObject
{
    Q_OBJECT
public:
    explicit ShotgunUploadRequest(QObject *parent = 0);

    const QString path() const;
    void setPath(const QString);

    const QString fieldName() const;
    void setFieldName(const QString);

    const QString displayName() const;
    void setDisplayName(const QString);

    const QString tagList() const;
    void setTagList(const QString);

    int index() const;
    void setIndex(int);

private:
    QString m_path;
    QString m_fieldName;
    QString m_displayName;
    QString m_tagList;
    int m_index;
};

class Shotgun : public PugItem
{
    friend class ShotgunReply;
    Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString scriptName READ scriptName WRITE setScriptName NOTIFY scriptNameChanged)
    Q_PROPERTY(bool convertDateTimesToUtc READ isConvertDateTimesToUtc WRITE setConvertDateTimesToUtc NOTIFY convertDateTimesToUtcChanged)
    Q_OBJECT
public:
    Shotgun(QObject *parent = 0);

    const QUrl baseUrl() const;
    void setBaseUrl(const QUrl url);

    const QString apiKey() const;
    void setApiKey(const QString key);

    const QString scriptName() const;
    void setScriptName(const QString s);

    bool isConvertDateTimesToUtc() const;
    void setConvertDateTimesToUtc(bool convert);

    Q_INVOKABLE ShotgunReply *find(const QString entityType,
            const QVariantList filters,
            const QStringList fields,
            const QStringList order = QStringList(),
            const QString filterOperator = "all",
            int limit = 0,
            bool retiredOnly = false,
            int page = 0);
    Q_INVOKABLE ShotgunReply *find(const QString entityType,
            const QVariantList filters);

    Q_INVOKABLE ShotgunReply *findOne(const QString entityType,
            const QVariantList filters,
            const QStringList fields,
            const QStringList order = QStringList(),
            const QString filterOperator="all",
            bool retiredOnly = false);
    Q_INVOKABLE ShotgunReply *findOne(const QString entityType,
            const QVariantList filters);

    Q_INVOKABLE ShotgunReply *create(const QString entityType,
            const QVariantMap data,
            const QStringList returnFields = QStringList());

    Q_INVOKABLE ShotgunReply *batch(const QVariantList requests);

    Q_INVOKABLE ShotgunReply *schemaRead();

    Q_INVOKABLE ShotgunReply *info();

    Q_INVOKABLE ShotgunReply *upload(const QString entityType,
            int entityId,
            const QString path,
            const QString fieldName=QString(),
            const QString displayName=QString(),
            const QString tagList=QString());

    Q_INVOKABLE ShotgunReply *uploadThumbnail(const QString entityType,
            int entityId,
            const QString path,
            const QString displayName=QString(),
            const QString tagList=QString());

    Q_INVOKABLE ShotgunReply *uploadFilmstripThumbnail(const QString entityType,
            int entityId,
            const QString path,
            const QString displayName=QString(),
            const QString tagList=QString());

signals:
    void baseUrlChanged(const QUrl url);
    void apiKeyChanged(const QString apiKey);
    void scriptNameChanged(const QString scriptName);
    void convertDateTimesToUtcChanged(bool convertDateTimesToUtc);

private:
    struct Config {
        Config() :
            maxRpcAttempts(3),
            timeoutSecs(-1),
            apiVer("api3"),
            convertDateTimesToUtc(true),
            recordsPerPage(500)
        {
        }

        int maxRpcAttempts;
        int timeoutSecs;
        QString apiVer;
        bool convertDateTimesToUtc;
        int recordsPerPage;
        QString apiKey;
        QString scriptName;
        QString userLogin;
        QString userPassword;
        QUuid sessionUuid;
        QUrl apiUrl;
        QVariant sessionToken; // TODO: type?
        QByteArray authorization;
        //bool noSslValidation = false;
    };

    const QVariantList mapToList(const QVariantMap map, const QString keyName="field_name", const QString valueName="value") const;
    const QVariantMap translateFilters(const QVariantList filters, const QString filterOperator) const;
    const QVariantMap constructReadParameters(const QString entityType, const QStringList fields,
            const QVariantMap filters, bool retiredOnly, const QStringList order) const;
    bool buildPayload(QJsonObject& payload, const QString method, const QJsonDocument params, bool includeScriptName=true) const;
    const QVariant transformOutbound(const QVariant params) const;
    ShotgunReply *callRpc(const QString method, const QVariant params, bool includeScriptName=true, ShotgunReply *reply=0);

    Config m_config;
    QUrl m_baseUrl;
    QNetworkAccessManager m_nas;
};

class ShotgunReply : public PugItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QNetworkReply::NetworkError errorCode READ errorCode NOTIFY errorCodeChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_ENUMS(QNetworkReply::NetworkError)
    friend class Shotgun;
public:
    const QVariant results() const;

    QNetworkReply::NetworkError errorCode() const;

    const QString errorString() const;

    // convenience typedef for error signal connections
    typedef void (ShotgunReply::*ErrorFunc)();

signals:
    void resultsChanged(const QVariant results);
    void errorCodeChanged(QNetworkReply::NetworkError code);
    void errorStringChanged(QString errorString);

    void error();
    void finished(const QVariant results);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    void setResults(const QVariant);
    void setErrorCode(QNetworkReply::NetworkError);
    void setErrorString(const QString);

protected:
    explicit ShotgunReply(QObject *parent = 0);

    void setNetworkReply(QNetworkReply *);
    void setMethod(const QString);
    void setParams(const QVariant);
    void setLimit(int);
    void setIncludeScriptName(bool);
    void setFirst(bool);

    void uploadLater(ShotgunUploadRequest *);

private slots:
    void onNetworkReplyFinished();
    void onNetworkReplyError();
    void onSslErrors(const QList<QSslError> &errors);

    void onUploadLaterFinished(const QVariant results);
    void onUploadLaterError();

private:
    const QVariant parseRecords(const QVariantList records) const;
    const QVariant parseRecord(const QVariantMap record) const;
    const QVariant transformInbound(const QVariant data) const;
    void continueUploading();

    QNetworkReply *m_networkReply;
    QVariant m_results;
    QString m_method;
    QVariant m_params;
    int m_limit;
    bool m_includeScriptNameFlag;
    bool m_first;
    QNetworkReply::NetworkError m_errorCode;
    QString m_errorString;

    QQueue<ShotgunUploadRequest *> m_uploads;
};


#endif
