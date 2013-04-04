#include <QDebug>
#include <QRegularExpression>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkProxy>

#include "shotgun.h"

static const QRegularExpression DATE_PATTERN("^(\\d{4})\\D?(0[1-9]|1[0-2])\\D?([12]\\d|0[1-9]|3[01])$");
static const QRegularExpression DATE_TIME_PATTERN("^(\\d{4})\\D?(0[1-9]|1[0-2])\\D?([12]\\d|0[1-9]|3[01])"
        "(\\D?([01]\\d|2[0-3])\\D?([0-5]\\d)\\D?([0-5]\\d)?\\D?(\\d{3})?)?$");

Shotgun *Shotgun::s_instance = 0;

ShotgunUploadRequest::ShotgunUploadRequest(QObject *parent) :
    QObject(parent),
    m_index(-1)
{
}

const QString ShotgunUploadRequest::path() const
{
    return m_path;
}

void ShotgunUploadRequest::setPath(const QString p)
{
    m_path = p;
}

const QString ShotgunUploadRequest::fieldName() const
{
    return m_fieldName;
}

void ShotgunUploadRequest::setFieldName(const QString fn)
{
    m_fieldName = fn;
}

const QString ShotgunUploadRequest::displayName() const
{
    return m_displayName;
}

void ShotgunUploadRequest::setDisplayName(const QString dn)
{
    m_displayName = dn;
}

const QString ShotgunUploadRequest::tagList() const
{
    return m_tagList;
}

void ShotgunUploadRequest::setTagList(const QString tags)
{
    m_tagList = tags;
}

int ShotgunUploadRequest::index() const
{
    return m_index;
}

void ShotgunUploadRequest::setIndex(int i)
{
    m_index = i;
}

Shotgun::Shotgun(QObject *parent) :
    PugItem(parent)
{
//    QNetworkProxy proxy;
//    proxy.setType(QNetworkProxy::HttpProxy);
//    proxy.setHostName("10.10.2.44");
//    proxy.setPort(8888);
//    QNetworkProxy::setApplicationProxy(proxy);
}

const QUrl Shotgun::baseUrl() const
{
    return m_baseUrl;
}

void Shotgun::setBaseUrl(const QUrl url)
{
    if (m_baseUrl != url) {
        m_baseUrl = url;
        m_config.apiUrl.setScheme(url.scheme());
        m_config.apiUrl.setHost(url.host());
        m_config.apiUrl.setPath(url.path() + "/" + m_config.apiVer + "/json");

        m_config.authorization = QString("Basic " + url.authority()).toUtf8().toBase64();
        emit baseUrlChanged(m_baseUrl);
    }
}

const QString Shotgun::apiKey() const
{
    return m_config.apiKey;
}

void Shotgun::setApiKey(const QString key)
{
    if (m_config.apiKey != key) {
        m_config.apiKey = key;
        emit apiKeyChanged(m_config.apiKey);
    }
}

const QString Shotgun::scriptName() const
{
    return m_config.scriptName;
}

void Shotgun::setScriptName(const QString s)
{
    if (m_config.scriptName != s) {
        m_config.scriptName = s;
        emit scriptNameChanged(m_config.scriptName);
    }
}

bool Shotgun::isConvertDateTimesToUtc() const
{
    return m_config.convertDateTimesToUtc;
}

void Shotgun::setConvertDateTimesToUtc(bool convert)
{
    if (m_config.convertDateTimesToUtc != convert) {
        m_config.convertDateTimesToUtc = convert;
        emit convertDateTimesToUtcChanged(m_config.convertDateTimesToUtc);
    }
}

const QVariantMap Shotgun::translateFilters(const QVariantList filters, const QString filterOperator) const
{
    // trnanslateFilters translates filters param into data structure
    // expected by rpc call.
    QVariantMap newFilters;

    if (filterOperator.isEmpty() || filterOperator == "all")
        newFilters["logical_operator"] = "and";
    else
        newFilters["logical_operator"] = "or";

    QVariantList conditions;
    foreach (QVariant v, filters) {
        if (v.type() == QVariant::List) {
            QVariantList sgFilter = v.toList();
            QVariantMap condition;
            condition["path"] = sgFilter[0];
            condition["relation"] = sgFilter[1];

            QVariantList values = sgFilter.mid(2);
            if (values.length() == 1 && values[0].type() == QVariant::List)
                values = values[0].toList();

            condition["values"] = values;
            conditions.append(condition);
        }
    }

    newFilters["conditions"] = conditions;

    return newFilters;
}

const QVariantMap Shotgun::constructReadParameters(const QString entityType, const QStringList fields,
        const QVariantMap filters, bool retiredOnly, const QStringList order) const
{
    trace() << ".constructReadParameters(" << entityType << "," << fields << "," << filters << "," << retiredOnly << "," << order << ")";
    QVariantMap params;
    params["type"] = entityType;
    params["return_fields"] = fields;
    params["filters"] = filters;
    params["return_only"] = retiredOnly ? "retired" : "active";
    params["return_paging_info"] = true;

    QVariantMap pagingParams;
    pagingParams["entities_per_page"] = m_config.recordsPerPage;
    pagingParams["current_page"] = 1;
    params["paging"] = pagingParams;

    if (!order.isEmpty()) {
        QVariantList sortList;
        foreach (QVariant v, order) {
            if (v.type() == QVariant::Map) {
                QVariantMap sort = v.toMap();

                if (sort.contains("column")) {
                    // TODO: warn about deprecation of 'column' param name
                    sort["field_name"] = sort["column"];
                }
                if (!sort.contains("direction"))
                    sort["direction"] = "asc";

                QVariantMap entry;
                entry["field_name"] = sort["field_name"];
                entry["direction"] = sort["direction"];
                sortList.append(entry);
            }
        }
        params["sorts"] = sortList;
    }
    trace() << "->" << params;
    return params;
}

ShotgunReply *Shotgun::find(const QString entityType,
        const QVariantList filters,
        const QStringList fields,
        const QStringList order,
        const QString filterOperator,
        int limit,
        bool retiredOnly,
        int page)
{
    trace() << ".find(" << entityType << "," << filters << "," << fields << "," << order << "," << filterOperator << "," << limit << "," << retiredOnly << "," << page << ")";
    QVariantMap newFilters = translateFilters(filters, filterOperator);
    QVariantMap params = constructReadParameters(entityType, fields, newFilters,
            retiredOnly, order);
    QVariantMap pagingParams = params["paging"].toMap();

    if (limit > 0 && limit <= m_config.recordsPerPage) {
        pagingParams["entities_per_page"] = limit;
        // if page isn't set and the limit doesn't require pagination,
        // then trigger the faster code path
        if (page == 0)
            page = 1;
    }

    // probably safe to assume we're accessing a server > 3.3.0
    params["api_return_image_urls"] = true;

    // if page is specified, then only return the page of records requested
    if (page != 0) {
        // no paging_info needed, so optimize it out
        params["return_paging_info"] = false;
        pagingParams["current_page"] = page;
    }

    params["paging"] = pagingParams;

    ShotgunReply *reply = callRpc("read", params);

    // stash the limit; we'll need it later
    if (reply)
        reply->setLimit(limit);

    return reply;
}

ShotgunReply *Shotgun::find(const QString entityType,
        const QVariantList filters)
{
    trace() << ".find(" << entityType << "," << filters << ")";
    QStringList fields;
    fields << "id";
    return find(entityType, filters, fields);
}

ShotgunReply *Shotgun::findOne(const QString entityType,
        const QVariantList filters,
        const QStringList fields,
        const QStringList order,
        const QString filterOperator,
        bool retiredOnly)
{
    trace() << ".findOne(" << entityType << "," << filters << "," << fields << "," << order << "," << filterOperator << "," << retiredOnly << ")";
    ShotgunReply *reply = find(entityType, filters, fields, order, filterOperator, 1, retiredOnly);
    if (reply)
        reply->setFirst(true);
    return reply;
}

ShotgunReply *Shotgun::findOne(const QString entityType,
        const QVariantList filters)
{
    trace() << ".findOne(" << entityType << "," << filters << ")";
    QStringList fields;
    fields << "id";
    return findOne(entityType, filters, fields);
}

const QVariantList Shotgun::mapToList(const QVariantMap map, const QString keyName, const QString valueName) const
{
    QVariantList list;
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        QVariantMap item;
        item[keyName] = i.key();
        item[valueName] = i.value();
        list << item;
    }

    return list;
}

Q_INVOKABLE ShotgunReply *Shotgun::create(const QString entityType,
        const QVariantMap data,
        const QStringList returnFields)
{
    trace() << ".create(" << entityType << "," << data << "," << returnFields << ")";
    QStringList createReturnFields = returnFields;
    if (createReturnFields.isEmpty())
        createReturnFields << "id";

    //debug() << "creating" << QJsonDocument::fromVariant(data);

    QVariantMap createData = data;

    QString uploadImage;
    if (createData.contains("image"))
        uploadImage = createData.take("image").toString();

    QString uploadFilmStripImage;
    if (createData.contains("filmstrip_image"))
        uploadFilmStripImage = createData.take("filmstrip_image").toString();

    QVariantMap params;
    params["type"] = entityType;
    params["fields"] = mapToList(createData);
    params["return_fields"] = createReturnFields;

    ShotgunReply *reply = callRpc("create", params);

    if (reply) {
        reply->setFirst(true);

        if (!uploadImage.isEmpty()) {
            debug() << "requesting upload of" << uploadImage;
            ShotgunUploadRequest *upload = new ShotgunUploadRequest(this);
            upload->setFieldName("thumb_image");
            upload->setPath(uploadImage);

            reply->uploadLater(upload);
        }

        if (!uploadFilmStripImage.isEmpty()) {
            debug() << "requesting upload of" << uploadFilmStripImage;
            ShotgunUploadRequest *upload = new ShotgunUploadRequest(this);
            upload->setFieldName("filmstrip_thumb_image");
            upload->setPath(uploadFilmStripImage);

            reply->uploadLater(upload);
        }
    }

    return reply;
}

ShotgunReply *Shotgun::batch(const QVariantList requests)
{
    trace() << ".batch(" << requests << ")";
    QVariantList calls;

    // TODO: handle empty list of requests

    QStringList uploadImages;
    QStringList uploadFilmstripImages;

    foreach (QVariant v, requests) {
        Q_ASSERT(v.type() == QVariant::Map);
        QVariantMap req = v.toMap();
        // TODO: check for valid requests

        QVariantMap requestParams;
        requestParams["request_type"] = req["request_type"];
        requestParams["type"] = req["entity_type"];

        if (req["request_type"] == "create") {
            QStringList returnFields;
            if (req.contains("return_fields") && req["return_fields"].toList().length() > 0)
                returnFields = req["return_fields"].toStringList();
            else
                returnFields.append("id");
            requestParams["return_fields"] = returnFields;

            QVariantMap requestData = req["data"].toMap();
            if (requestData.contains("image"))
                uploadImages.append(requestData.take("image").toString());
            else
                uploadImages.append(QString());

            if (requestData.contains("filmstrip_image"))
                uploadFilmstripImages.append(requestData.take("filmstrip_image").toString());
            else
                uploadFilmstripImages.append(QString());

            requestParams["fields"] = mapToList(requestData);
        } else {
            error() << "skipping unsupported batch request_type" << req["request_type"];
        }

        calls.append(requestParams);
    }

    ShotgunReply *reply = callRpc("batch", calls);

    if (reply) {
        Q_ASSERT(uploadImages.count() == uploadFilmstripImages.count());

        // handle image uploads
        for (int i = 0; i < uploadImages.count(); i++) {
            if (!uploadImages.at(i).isEmpty()) {
                ShotgunUploadRequest *upload = new ShotgunUploadRequest(this);
                upload->setFieldName("thumb_image");
                upload->setPath(uploadImages.at(i));
                upload->setIndex(i);

                reply->uploadLater(upload);
            }

            if (!uploadFilmstripImages.at(i).isEmpty()) {
                ShotgunUploadRequest *upload = new ShotgunUploadRequest(this);
                upload->setFieldName("filmstrip_thumb_image");
                upload->setPath(uploadFilmstripImages.at(i));
                upload->setIndex(i);

                reply->uploadLater(upload);
            }

        }
    }

    return reply;
}

ShotgunReply *Shotgun::schemaRead()
{
    return callRpc("schema_read", QVariant());
}

ShotgunReply* Shotgun::info() {
    trace() << ".info()";
    return callRpc("info", QVariant(), false);
}

ShotgunReply *Shotgun::upload(const QString entityType, int entityId, const QString path,
        const QString fieldName, const QString displayName, const QString tagList)
{
    trace() << ".upload(" << entityType << "," << entityId << "," << path << "," << fieldName << "," << displayName << "," << tagList << ")";
    QVariantMap params;
    params["entity_type"] = entityType;
    params["entity_id"] = entityId;
    params["script_name"] = m_config.scriptName;
    params["script_key"] = m_config.apiKey;

    if (!m_config.sessionUuid.isNull())
        params["session_uuid"] = m_config.sessionUuid.toString();

    QFileInfo fileInfo(path);

    if (!fileInfo.exists()) {
        //error() << path << "does not exist";
    }

    QUrl url = m_baseUrl;
    if (fieldName == "thumb_image" || fieldName == "filmstrip_thumb_image") {
        url.setPath(url.path() + "/upload/publish_thumbnail");
        params["thumb_image"] = path;
        if (fieldName == "filmstrip_thumb_image")
            params["filmstrip"] = "True";

    } else {
        url.setPath(url.path() + "/upload/upload_file");
        if (!displayName.isEmpty())
            params["display_name"] = displayName;
        else
            params["display_name"] = fileInfo.baseName();
        // we allow linking to nothing for generic reference use cases
        if (!fieldName.isEmpty())
            params["field_name"] = fieldName;
        if (!tagList.isEmpty())
            params["tag_list"] = tagList;

        params["file"] = path;
    }

    // build our upload request
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for (QVariantMap::const_iterator i = params.constBegin(); i != params.constEnd(); ++i) {
        QHttpPart part;
        if (i.key() == "thumb_image" || i.key() == "file") {
            QFile *file = new QFile(i.value().toString());
            QFileInfo fileInfo(file->fileName());

            part.setHeader(QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"%1\"; filename=\"%2\"").arg(i.key()).arg(fileInfo.fileName()));

            // TODO: replace with something a bit more robust, like FreeDesktopMime
            if (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "jpeg")
                part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
            else if (fileInfo.suffix().toLower() == "png")
                part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

            file->open(QIODevice::ReadOnly);
            part.setBodyDevice(file);
            // parent the file to the multiPart for lifetime management
            file->setParent(multiPart);
        } else {
            part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(i.key()));
            // TODO: should this be UTF-8, and should we have a charset in the content type?
            part.setBody(i.value().toString().toLatin1());
        }
        multiPart->append(part);
    }

    QNetworkRequest req(url);

    QNetworkReply *networkReply = m_nas.post(req, multiPart);

    ShotgunReply *reply = new ShotgunReply(this);

    reply->setNetworkReply(networkReply);
    reply->setMethod("upload");
    reply->setParams(params);

    return reply;
}

ShotgunReply *Shotgun::uploadThumbnail(const QString entityType, int entityId, const QString path,
        const QString displayName, const QString tagList)
{
    trace() << ".uploadThumbnail(" << entityType << "," << entityId << "," << path << "," << displayName << "," << tagList << ")";
    return upload(entityType, entityId, path, "thumb_image", displayName, tagList);
}

ShotgunReply *Shotgun::uploadFilmstripThumbnail(const QString entityType, int entityId, const QString path,
        const QString displayName, const QString tagList)
{
    trace() << ".uploadFilmstripThumbnail(" << entityType << "," << entityId << "," << path << "," << displayName << "," << tagList << ")";
    return upload(entityType, entityId, path, "filmstrip_thumb_image", displayName, tagList);
}

Shotgun *Shotgun::staticInstance()
{
    if (s_instance == 0)
        s_instance = new Shotgun();

    return s_instance;
}

ShotgunReply *Shotgun::callRpc(const QString method, const QVariant params, bool includeScriptName, ShotgunReply *reply)
{
    trace() << ".callRpc(" << method << "," << params << "," << includeScriptName << "," << reply << ")";
    QJsonObject payload;

    const QVariant newParams = transformOutbound(params);

    debug() << "transformed params" << newParams;

    if (!buildPayload(payload, method, QJsonDocument::fromVariant(newParams), includeScriptName)) {
        error() << "failed to build payload";
        return 0;
    }

    QByteArray encodedPayload = QJsonDocument(payload).toJson();

    debug() << "callRpc" << payload;

    QNetworkRequest req;
    req.setUrl(m_config.apiUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("connection", "keep-alive");
    req.setHeader(QNetworkRequest::UserAgentHeader, "shotgun-json");

    if (!m_config.authorization.isEmpty())
        req.setRawHeader("Authorization", m_config.authorization);

    QNetworkReply *networkReply = m_nas.post(req, encodedPayload);

    Q_ASSERT(networkReply);

    if (!reply)
        reply = new ShotgunReply(this);

    Q_ASSERT(reply);

    reply->setNetworkReply(networkReply);
    reply->setMethod(method);
    reply->setParams(newParams);

    return reply;
}

const QVariant Shotgun::transformOutbound(const QVariant data) const
{
    if (data.type() == QVariant::List) {
        QVariantList result;
        foreach (QVariant element, data.toList()) {
            result.append(transformOutbound(element));
        }
        return result;

    } else if (data.type() == QVariant::Map) {
        QVariantMap result;
        QMapIterator<QString, QVariant> i(data.toMap());
        while (i.hasNext()) {
            i.next();
            result.insert(i.key(), transformOutbound(i.value()));
        }
        return result;

    } else if (data.type() == QVariant::DateTime) {
        QDateTime dt = data.toDateTime();
        if (m_config.convertDateTimesToUtc)
            dt = dt.toUTC();

        return dt.toString(Qt::ISODate);

    } else if (data.type() == QVariant::Date) {
        QDate d = data.toDate();

        return d.toString(Qt::ISODate);

    } else if (data.type() == QVariant::Time) {
        QDateTime dt = QDateTime::currentDateTime();
        QTime t = data.toTime();
        dt.setTime(t);

        if (m_config.convertDateTimesToUtc)
            dt = dt.toUTC();

        return dt.toString(Qt::ISODate);

    } else {
        return data;
    }
}

bool Shotgun::buildPayload(QJsonObject& payload, const QString method, const QJsonDocument params, bool includeScriptName) const
{
    if (method.isNull() || method.isEmpty())
        return 0;

    QJsonArray callParams;

    if (includeScriptName) {
        if (m_config.scriptName.isEmpty())
            return 0;
        if (m_config.apiKey.isEmpty())
            return 0;

        QJsonObject authParams;
        if (!m_config.userLogin.isEmpty() && !m_config.userPassword.isEmpty()) {
            // Used to authenticate HumanUser credentials
            authParams["user_login"] = m_config.userLogin;
            authParams["user_password"] = m_config.userPassword;
        } else {
            // Use script name instead
            authParams["script_name"] = m_config.scriptName;
            authParams["script_key"] = m_config.apiKey;
        }

        if (!m_config.sessionUuid.isNull())
            authParams["session_uuid"] = m_config.sessionUuid.toString();

        callParams.append(authParams);

    }

    if (!params.isEmpty()) {
        if (params.isArray())
            callParams.append(params.array());
        else if (params.isObject())
            callParams.append(params.object());
    }

    payload["method_name"] = method;
    payload["params"] = callParams;

    return true;
}

ShotgunReply::ShotgunReply(QObject *parent) :
    PugItem(parent),
    m_networkReply(),
    m_limit(0),
    m_includeScriptNameFlag(false),
    m_first(false)
{
}

const QVariant ShotgunReply::results() const
{
    return m_results;
}

QNetworkReply::NetworkError ShotgunReply::errorCode() const
{
    return m_errorCode;
}

const QString ShotgunReply::errorString() const
{
    return m_errorString;
}

void ShotgunReply::setResults(const QVariant results)
{
    m_results = results;
}

void ShotgunReply::setErrorCode(QNetworkReply::NetworkError code)
{
    m_errorCode = code;
}
void ShotgunReply::setErrorString(const QString str)
{
    m_errorString = str;
}

void ShotgunReply::setNetworkReply(QNetworkReply *networkReply)
{
    if (m_networkReply)
        m_networkReply->deleteLater();

    m_networkReply = networkReply;
    m_networkReply->setParent(this);

    connect(m_networkReply, &QNetworkReply::finished,
            this, &ShotgunReply::onNetworkReplyFinished);
    connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &ShotgunReply::onNetworkReplyError);

    connect(m_networkReply, SIGNAL(sslErrors(const QList<QSslError> & )),
            this, SLOT(onSslErrors(const QList<QSslError> & )));


}

void ShotgunReply::onSslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(QObject::sender());
    //warning() << "ignoring ssl errors:";
    //foreach (QSslError err, errors) {
    //    warning() << err;
    //}

    reply->ignoreSslErrors();
}

void ShotgunReply::setMethod(const QString method)
{
    m_method = method;
}

void ShotgunReply::setParams(const QVariant params)
{
    m_params = params;
}

void ShotgunReply::setLimit(int limit)
{
    m_limit = limit;
}

void ShotgunReply::setIncludeScriptName(bool includeScriptName)
{
    m_includeScriptNameFlag = includeScriptName;
}

void ShotgunReply::setFirst(bool first)
{
    m_first = first;
}

void ShotgunReply::uploadLater(ShotgunUploadRequest *upload)
{
    m_uploads.enqueue(upload);
}

void ShotgunReply::onNetworkReplyFinished()
{
    trace() << ".onNetworkReplyFinished()";
    QByteArray respBody = m_networkReply->readAll();
    QString contentType = m_networkReply->header(QNetworkRequest::ContentTypeHeader).toString();

    QJsonDocument respJson = QJsonDocument::fromJson(respBody);

    debug() << "received" << respJson;


    // TODO: decode response
    // response = decodeResponse(respHeaders, respBody);
    // handle errors

    QVariant response = respJson.toVariant();

    // check for errors in the response
    if (response.type() == QVariant::Map && response.toMap().contains("exception") &&
            response.toMap()["exception"].type() == QVariant::Bool && response.toMap()["exception"].toBool())
    {
        setErrorCode(QNetworkReply::ProtocolFailure);
        setErrorString(response.toMap().value("message", "Unknown Error").toString());
        PugItem::error() << "onNetworkReplyFinished" << errorString() << "from request" << m_method << "with params" << QJsonDocument::fromVariant(m_params).toJson();
        emit error();
        return;
    }

    response = transformInbound(response);

    QVariant results;
    if (response.type() == QVariant::Map && response.toMap().contains("results")) {
        results = response.toMap()["results"];
    } else {
        results = response;
    }

    //m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //m_networkReply->attribute( QNetworkRequest::HttpReasonPhraseAttribute).toString();

    // some methods need to repeatedly callRpc until they're done
    bool finishedFlag = true;

    if (m_method == "info") {
        Q_ASSERT(results.type() == QVariant::Map);
        m_results = results;
    } else if (m_method == "read") {
        Q_ASSERT(m_params.toMap().contains("paging") && m_params.toMap()["paging"].type() == QVariant::Map);
        QVariantMap pagingParams = m_params.toMap()["paging"].toMap();

        Q_ASSERT(pagingParams["current_page"].type() == QVariant::Int);
        int currentPage = pagingParams["current_page"].toInt();

        Q_ASSERT(results.type() == QVariant::Map);

        if (currentPage != 0) {
            // we're only returning a single page
            QVariantList records = results.toMap()["entities"].toList();
            m_results = parseRecords(records);
        } else {
            Q_ASSERT(results.toMap().contains("paging_info") && results.toMap()["paging_info"].type() == QVariant::Map);
            QVariantMap pagingInfo = results.toMap()["paging_info"].toMap();

            Q_ASSERT(pagingInfo.contains("entity_count") && pagingInfo["entity_count"].type() == QVariant::Int);
            int entityCount = pagingInfo["entity_count"].toInt();

            bool readMoreFlag = true;
            QVariantList records = results.toMap()["entities"].toList();
            if (!records.isEmpty()) {
                if (m_limit > 0 && records.length() >= m_limit) {
                    records = records.mid(0, m_limit);
                    readMoreFlag = false;
                }

                if (records.length() == entityCount) {
                    readMoreFlag = false;
                }

                if (readMoreFlag) {
                    // we're looping until we get all the records we asked for
                    pagingParams["currentPage"] = currentPage + 1;
                    m_params.toMap()["paging"] = pagingParams;

                    Shotgun *shotgun = qobject_cast<Shotgun *>(QObject::parent());
                    Q_ASSERT(shotgun);
                    shotgun->callRpc(m_method, m_params, m_includeScriptNameFlag, this);
                    finishedFlag = false;
                }
            }

            QVariantList resultList = m_results.toList();
            resultList.append(parseRecords(records));

            m_results = resultList;
        }

        if (m_first && m_results.type() == QVariant::List) {
            if (m_results.toList().length() > 0)
                m_results = m_results.toList().first();
            else
                m_results = QVariant();
        }
    } else if (m_method == "create") {
        Q_ASSERT(results.type() == QVariant::Map);
        m_results = parseRecord(results.toMap());

        if (!m_uploads.isEmpty()) {
            // not done yet, still need to upload something
            finishedFlag = false;
            continueUploading();
        }
    } else if (m_method == "batch") {
        Q_ASSERT(results.type() == QVariant::List);
        m_results = parseRecords(results.toList());

        if (!m_uploads.isEmpty()) {
            // not done yet, still need to upload something
            finishedFlag = false;
            continueUploading();
        }
    } else {
        m_results = results;
    }

    if (finishedFlag) {
        emit finished(m_results);
    }
}

const QVariant ShotgunReply::parseRecords(const QVariantList records) const
{
    QVariantList results;
    foreach (QVariant record, records) {
        if (record.type() == QVariant::Map)
            results.append(parseRecord(record.toMap()));
    }

    return results;
}

const QVariant ShotgunReply::parseRecord(const QVariantMap record) const
{
    QVariantMap result;
    for (QVariantMap::const_iterator i = record.constBegin(); i != record.constEnd(); ++i) {
        if (!i.value().isValid())
            continue;

        QVariant value = i.value();

        // check for html entities in strings
        if (value.type() == QVariant::String)
            value = value.toString().replace("&lt;", "<");

        if (i.value().type() == QVariant::Map && i.value().toMap().value("link_type").toString() == "local") {
            // QString localPath =
            // TODO: client caps local_path_field
        }

        result.insert(i.key(), value);
    }

    return result;
}

const QVariant ShotgunReply::transformInbound(const QVariant data) const
{
    Shotgun *shotgun = qobject_cast<Shotgun *>(QObject::parent());
    Q_ASSERT(shotgun);

    if (data.type() == QVariant::List) {
        QVariantList result;
        foreach (QVariant element, data.toList()) {
            result.append(transformInbound(element));
        }
        return result;

    } else if (data.type() == QVariant::Map) {
        QVariantMap result;
        QMapIterator<QString, QVariant> i(data.toMap());
        while (i.hasNext()) {
            i.next();
            result.insert(i.key(), transformInbound(i.value()));
        }
        return result;

    } else if (data.type() == QVariant::String) {
        QString s = data.toString();

        if (s.length() == 20 && DATE_TIME_PATTERN.match(s).hasMatch()) {
            QDateTime dt = QDateTime::fromString(s, Qt::ISODate);
            if (shotgun->isConvertDateTimesToUtc())
                dt = dt.toLocalTime();
            return dt;
        } else {
            return data;
        }
    } else {
        return data;
    }
}

void ShotgunReply::onNetworkReplyError()
{
    setErrorCode(m_networkReply->error());
    setErrorString(m_networkReply->errorString());

    //error() << ".onNetworkReplyError" << errorString();

    emit error();
}

void ShotgunReply::continueUploading()
{
    trace() << ".continueUploading()";
    Q_ASSERT(!m_uploads.isEmpty());

    Shotgun *shotgun = qobject_cast<Shotgun *>(QObject::parent());
    ShotgunUploadRequest *upload = m_uploads.dequeue();

    ShotgunReply *uploadReply;
    if (upload->index() < 0) {
        uploadReply = shotgun->upload(
                m_results.toMap()["type"].toString(),
                m_results.toMap()["id"].toInt(),
                upload->path(),
                upload->fieldName(),
                upload->displayName(),
                upload->tagList());
    } else {
        uploadReply = shotgun->upload(
                m_results.toList().at(upload->index()).toMap()["type"].toString(),
                m_results.toList().at(upload->index()).toMap()["id"].toInt(),
                upload->path(),
                upload->fieldName(),
                upload->displayName(),
                upload->tagList());
    }

    connect(uploadReply, &ShotgunReply::finished,
            this, &ShotgunReply::onUploadLaterFinished);
    connect(uploadReply, &ShotgunReply::error,
            this, &ShotgunReply::onUploadLaterError);

    upload->deleteLater();
}

void ShotgunReply::onUploadLaterFinished(const QVariant results)
{
    Q_UNUSED(results);
    // TODO: (ish) the Python shotgun api does a find_one here to get the actual image URL

    // clean up the ShotgunReply
    QObject::sender()->deleteLater();

    if (m_uploads.isEmpty())
        emit finished(m_results);
    else
        continueUploading();
}

void ShotgunReply::onUploadLaterError()
{
    ShotgunReply *reply = qobject_cast<ShotgunReply *>(QObject::sender());

    setErrorCode(reply->errorCode());
    setErrorString(reply->errorString());

    reply->deleteLater();

    emit error();
}
