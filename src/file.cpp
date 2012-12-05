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

File::File(QObject *parent) :
    BranchBase(parent),
    m_input(),
    m_linkType(File::Hard)
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cook, this, &File::onCook);

    addInput("input");
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
    QStringList paths = listMatchingPaths(env.toMap());

    setPaths(paths, env.toMap());

    debug() << "onUpdate set paths" << paths;

    if (paths.length() == 0)
        warning() << "no paths matched";

    emit updated(OperationAttached::Finished);
}

void File::onCook(const QVariant env)
{
    debug() << "onCook";

    if (m_input) {
        copious() << "has input" << m_input;
        // hard link to input, pass data along
        clearDetails();

        QVariantList inputDetails = m_input->details();

        debug() << "input" << m_input;
        debug() << "input details" << inputDetails;

        if (inputDetails.length() == 0) {
            warning() << "no input details";
        }

        for (int index=0; index < inputDetails.length(); index++) {
            if (inputDetails[index].canConvert<QVariantMap>()) {
                QVariantMap inputDetail = inputDetails[index].toMap();

                debug() << "cooking index" << index;

                if (inputDetail.contains("element") && inputDetail.contains("env")) {
                    const Element *inputElement = inputDetail.value("element").value<Element *>();
                    const QVariantMap inputEnv = inputDetail.value("env").value<QVariantMap>();

                    debug() << "input env" << inputEnv;

                    Element *destElement = new Element(this);

                    destElement->setPattern(map(inputEnv));
                    destElement->setFrames(inputElement->frames());

                    // create a new env based off our default
                    QVariantMap destEnv(env.toMap());

                    debug() << "dest env" << destEnv;

                    debug() << "dest element" << destElement->toString();

                    // merge input env
                    QMapIterator<QString, QVariant> i(inputEnv);
                    while (i.hasNext()) {
                        i.next();
                        // don't overwrite
                        destEnv.insert(i.key(), i.value());
                    }

                    foreach (const QString inputPath, inputElement->paths()) {
                        QVariant srcFrame = inputElement->frame(inputPath);
                        QString destPath = destElement->path(srcFrame);

                        debug() << "cooking" << inputPath << "->" << destPath;

                        if (!destPath.isEmpty()) {
                            QFileInfo destInfo(destPath);
                            destInfo.absoluteDir().mkpath(".");

                            QFile destFile(destPath);
                            if (destFile.exists())
                                destFile.remove();

                            copious() << "linking" << inputPath << "to" << destPath;

                            if (m_linkType == File::Hard) {
                                // META TODO: no cross-platform hardlink support in Qt
                                if (link(inputPath.toUtf8().constData(), destPath.toUtf8().constData()) != 0) {
                                    error() << "link failed:" << inputPath.toUtf8().constData() << "->" << destPath.toUtf8().constData();
                                    error() << "error is:" << strerror(errno);

                                    emit cooked(OperationAttached::Error);
                                    return;
                                }
                            } else {
                                if (symlink(inputPath.toUtf8().constData(), destPath.toUtf8().constData()) != 0) {
                                    error() << "symlink failed:" << inputPath.toUtf8().constData() << "->" << destPath.toUtf8().constData();
                                    error() << "error is:" << strerror(errno);

                                    emit cooked(OperationAttached::Error);
                                    return;
                                }
                            }
                        }
                    }

                    setDetail(index, "element", QVariant::fromValue(destElement));
                    setDetail(index, "env", destEnv);

                } else if (!inputDetail.contains("element")) {
                    error() << "input.details[" << index << "].element does not exist";
                } else if (!inputDetail.contains("env")) {
                    error() << "input.details[" << index << "].env does not exist";
                }
            } else {
                error() << "input.details[" << index << "] is not a map";
            }

        }

    }

    emit cooked(OperationAttached::Finished);
}
