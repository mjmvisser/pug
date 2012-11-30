#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QQmlProperty>

#include <unistd.h>
#include <errno.h>

#include "file.h"
#include "element.h"
#include "property.h"

File::File(QObject *parent) :
    BranchBase(parent),
    m_linkType(File::Hard)
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cook, this, &File::onCook);

    Property *inputProperty = new Property(this);
    inputProperty->setName("input");
    inputProperty->setInput(true);
}

NodeBase *File::input()
{
    return m_input;
}
const NodeBase *File::input() const
{
    return m_input;
}

void File::setInput(NodeBase *node)
{
    if (m_input != node) {
        m_input = node;
        emit inputChanged(m_input);
    }
}

File::LinkType File::linkType() const
{
    return m_linkType;
}

void File::setLinkType(File::LinkType lt)
{
    if (m_linkType != lt) {
        m_linkType = lt;
        emit linkTypeChanged(lt);
    }
}

void File::onUpdate(const QVariant env)
{
    QStringList paths = listMatchingPaths(env);

    setPaths(paths);

    debug() << "onUpdate set paths" << paths;

    emit updated(OperationAttached::Finished);
}

void File::onCook(const QVariant env)
{
    debug() << "onCook";

    if (m_input) {
        copious() << "has input" << m_input;
        // hard link to input, pass data along
        clearElements();

        QList<Element *> inputElements;
        // QML isn't quite production-ready when it comes to treating both QList<Element *> and
        // javascript lists of QVariant-wrapped-Element* the same
        if (qobject_cast<BranchBase *>(m_input)) {
            BranchBase *inputBranch = qobject_cast<BranchBase *>(m_input);
            foreach (Element *element, inputBranch->elements()) {
                inputElements.append(element);
            }
        } else {
            QQmlProperty inputElementsProperty(m_input, "elements");

            if (!inputElementsProperty.isValid()) {
                error() << "input does not have an elements property";
                emit cooked(OperationAttached::Error);
                return;
            } else if (inputElementsProperty.propertyTypeCategory() != QQmlProperty::List &&
                       (inputElementsProperty.propertyTypeCategory() != QQmlProperty::Normal ||
                               !inputElementsProperty.read().canConvert<QVariantList>()))
            {
                error() << "input.elements is not a list";
                emit cooked(OperationAttached::Error);
                return;
            }

            foreach (const QVariant v, inputElementsProperty.read().toList()) {
                if (!v.canConvert<Element *>()) {
                    error() << "item in elements is not an Element: " << v;
                    emit cooked(OperationAttached::Error);
                    return;
                }
                inputElements.append(v.value<Element *>());
            }
        }

        copious() << "passed sanity checks";

        foreach (const Element *srcElement, inputElements) {
            Element *destElement = new Element(this);

            copious() << "got" << srcElement;

            // create a new env based off our default
            QVariantMap destData(env.toMap());

            // merge srcData
            QMapIterator<QString, QVariant> i(srcElement->data().toMap());
            while (i.hasNext()) {
                i.next();
                // don't overwrite
                destData.insert(i.key(), i.value());
            }


            destElement->setData(destData);
            destElement->setPattern(map(srcElement->data()));
            destElement->setFrames(srcElement->frames());

            addElement(destElement);

            debug() << srcElement->data();

            foreach (const QString srcPath, srcElement->paths()) {
                debug() << "cooking" << srcPath;

                QVariant srcFrame = srcElement->frame(srcPath);
                QString destPath = destElement->path(srcFrame);

                if (!destPath.isEmpty()) {
                    QFileInfo destInfo(destPath);
                    destInfo.absoluteDir().mkpath(".");

                    QFile destFile(destPath);
                    if (destFile.exists())
                        destFile.remove();

                    copious() << "linking" << srcPath << "to" << destPath;

                    if (m_linkType == File::Hard) {
                        // META TODO: no cross-platform hardlink support in Qt
                        if (link(srcPath.toUtf8().constData(), destPath.toUtf8().constData()) != 0) {
                            error() << "link failed:" << srcPath.toUtf8().constData() << "->" << destPath.toUtf8().constData();
                            error() << "error is:" << strerror(errno);

                            emit cooked(OperationAttached::Error);
                            return;
                        }
                    } else {
                        if (symlink(srcPath.toUtf8().constData(), destPath.toUtf8().constData()) != 0) {
                            error() << "symlink failed:" << srcPath.toUtf8().constData() << "->" << destPath.toUtf8().constData();
                            error() << "error is:" << strerror(errno);

                            emit cooked(OperationAttached::Error);
                            return;
                        }
                    }
                }
            }
        }
    } else {
        // otherwise just set the data
        foreach (Element *element, elements()) {
            debug() << "pattern" << element->pattern();
            QVariant data = parse(element->pattern());
            debug() << "data" << data;
            element->setData(data);
        }
    }

    emit cooked(OperationAttached::Finished);
}
