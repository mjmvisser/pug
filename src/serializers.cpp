#include <QJSValueIterator>
#include <QDateTime>
#include <QMetaProperty>
#include <QDebug>

#include "element.h"
#include "serializers.h"

enum jsvalueType { UNDEFINED_VALUE, NULL_VALUE, BOOLEAN, NUMBER, STRING, OBJECT, ARRAY, DATE, VARIANT, QOBJECT };

quint32 numKeys(const QJSValue& jsvalue)
{
    quint32 n = 0;
    QJSValueIterator it(jsvalue);
    while (it.hasNext()) {
        it.next();
        n++;
    }
    return n;
}

static const quint32 MAGIC = 0xdeadbeef;

JSDataStream& operator<<(JSDataStream &stream, const QJSValue& jsvalue)
{
    static_cast<QDataStream &>(stream) << MAGIC;

    if (jsvalue.isUndefined()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(UNDEFINED_VALUE);
    } else if (jsvalue.isNull()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(NULL_VALUE);
    } else if (jsvalue.isBool()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(BOOLEAN) << jsvalue.toBool();
    } else if (jsvalue.isNumber()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(NUMBER) << jsvalue.toNumber();
    } else if (jsvalue.isString()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(STRING) << jsvalue.toString();
    } else if (jsvalue.isDate()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(DATE) << jsvalue.toDateTime();
    } else if (jsvalue.isArray()) {
        quint32 length = qjsvalue_cast<quint32>(jsvalue.property("length"));
        static_cast<QDataStream &>(stream) << static_cast<int>(ARRAY) << length;
        for (quint32 i = 0; i < length; i++) {
            stream << jsvalue.property(i);
        }
    } else if (jsvalue.isCallable()) {
        // we ignore callable
    } else if (jsvalue.isVariant()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(VARIANT) << jsvalue.toVariant();
    } else if (jsvalue.isQObject()) {
        const QObject *obj = jsvalue.toQObject();
        QString className = obj->metaObject()->className();
        static_cast<QDataStream &>(stream) << static_cast<int>(QOBJECT) << className;
        static_cast<QDataStream &>(stream) << *obj;
    } else if (jsvalue.isObject()) {
        static_cast<QDataStream &>(stream) << static_cast<int>(OBJECT) << numKeys(jsvalue);
        QJSValueIterator it(jsvalue);
        while (it.hasNext()) {
            it.next();
            if (it.name() != "length" && it.name() != "prototype" && it.name() != "constructor" && it.name() != "arguments") {
                static_cast<QDataStream &>(stream) << it.name();
                stream << it.value();
            }
        }
    }
    // no idea how to create a js regexp

    return stream;
}

JSDataStream& operator>>(JSDataStream &stream, QJSValue &jsvalue)
{
    Q_ASSERT(stream.m_engine);

    quint32 magic;
    stream >> magic;
    Q_ASSERT(magic == MAGIC);

    int type;
    stream >> type;
    if (type == UNDEFINED_VALUE) {
        jsvalue = QJSValue(QJSValue::UndefinedValue);
    } else if (type == NULL_VALUE) {
        jsvalue = QJSValue(QJSValue::NullValue);
    } else if (type == BOOLEAN) {
        bool value;
        stream >> value;
        jsvalue = value;
    } else if (type == NUMBER) {
        double value;
        stream >> value;
        jsvalue = value;
    } else if (type == STRING) {
        QString value;
        stream >> value;
        jsvalue = value;
    } else if (type == DATE) {
        QDateTime value;
        stream >> value;
        jsvalue = stream.m_engine->toScriptValue(value);
    } else if (type == ARRAY) {
        quint32 l;
        stream >> l;
        jsvalue = stream.m_engine->newArray(l);

        for (quint32 i = 0; i < l; i++) {
            QJSValue value;
            stream >> value;
            jsvalue.setProperty(i, value);
        }
    } else if (type == OBJECT) {
        quint32 n;
        stream >> n;
        jsvalue = stream.m_engine->newObject();
        for(quint32 i = 0; i < n; i++) {
            QString key;
            QJSValue value;
            stream >> key;
            stream >> value;
            jsvalue.setProperty(key, value);
        }
    } else if (type == VARIANT) {
        QVariant value;
        stream >> value;
        jsvalue = stream.m_engine->toScriptValue(value);
    } else if (type == QOBJECT) {
        QString className;
        stream >> className;
        int id = QMetaType::type(className.toUtf8().data());
        Q_ASSERT(id != QMetaType::UnknownType);
        if (id != QMetaType::UnknownType) {
            QObject *obj = static_cast<QObject *>(QMetaType::create(id));
            if (obj) {
                stream >> *obj;
                jsvalue = stream.m_engine->newQObject(obj);
            }
        }
    }

    return stream;
}


QDataStream &operator<<(QDataStream &stream, const QObject &obj) {
    for(int i=0; i<obj.metaObject()->propertyCount(); ++i) {
        if(obj.metaObject()->property(i).isStored(&obj)) {
            QVariant var = obj.metaObject()->property(i).read(&obj);
            if (var.canConvert<QObject *>()) {
                // QVariants containing QObject* don't serialize
                QObject *p = qvariant_cast<QObject *>(var);
                if (p) {
                    stream << true;
                    stream << *p;
                } else {
                    stream << false;
                }
            } else {
                stream << var;
            }
        }
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QObject &obj) {
    QVariant var;
    for(int i=0; i<obj.metaObject()->propertyCount(); ++i) {
        if(obj.metaObject()->property(i).isStored(&obj)) {
            int id = QMetaType::type(obj.metaObject()->property(i).typeName());
            if (QMetaType::metaObjectForType(id) != 0) {
                // it's a QObject * property
                bool valid;
                stream >> valid;
                if (valid) {
                    // not null
                    QObject *p = static_cast<QObject *>(QMetaType::create(id));
                    if (p) {
                        p->setParent(&obj);
                        stream >> *p;
                    }
                    obj.metaObject()->property(i).write(&obj, QVariant::fromValue(p));
                } else {
                    obj.metaObject()->property(i).write(&obj, QVariant());
                }
            } else {
                stream >> var;
                obj.metaObject()->property(i).write(&obj, var);
            }
        }
    }

    return stream;
}
