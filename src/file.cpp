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
    Branch(parent),
    m_input(),
    m_linkType(File::Hard)
{
    setExactMatch(true);

    // update -> onUpdate
    connect(this, &File::update, this, &File::onUpdate);
    connect(this, &File::cook, this, &File::onCook);

    addInput("input");
}

Node *File::input()
{
    return m_input;
}
const Node *File::input() const
{
    return m_input;
}

void File::setInput(Node *node)
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

void File::onUpdate(const QVariant context)
{
    trace() << ".onUpdate(" << context << ")";

    QStringList paths = listMatchingPaths(context.toMap());

    setPaths(paths, context.toMap());

    info() << "Update matched" << paths << "from" << pattern();

    emit updated(OperationAttached::Finished);
}

void File::onCook(const QVariant context)
{
    trace() << ".onCook(" << context << ")";

    if (m_input) {
        debug() << "has input" << m_input;
        // hard link to input, pass data along

        if (m_input->details().property("length").toInt() == 0) {
            warning() << "no input details";
        }

        setCount(m_input->details().property("length").toInt());
        clearDetails();

        for (int index=0; index < count(); index++) {
            debug() << "cooking index" << index;

            const Element *inputElement = m_input->element(index);
            const QVariantMap inputContext = m_input->context(index);
            if (inputElement) {
                debug() << "input" << m_input;
                debug() << "input fields" << inputContext;
                debug() << "input element pattern" << inputElement->pattern();
                debug() << "input element" << inputElement->toString();

                Element *destElement = new Element(this);

                destElement->setPattern(map(inputContext));
                destElement->setFrames(inputElement->frames());

                // create a new field values based off our default
                QVariantMap destContext(context.toMap());

                debug() << "dest context" << destContext;
                debug() << "dest element" << destElement->toString();

                // merge input context
                QMapIterator<QString, QVariant> i(inputContext);
                while (i.hasNext()) {
                    i.next();
                    // don't overwrite
                    destContext.insert(i.key(), i.value());
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

                        debug() << "linking" << inputPath << "to" << destPath;

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

                setElement(index, destElement, false);
                setContext(index, destContext, false);
                debug() << "set detail[" << index << "] to " << details().property(index);

                info() << "Cook generated:" << destElement->paths();

            } else if (!inputDetail.property("element").isQObject()) {
                error() << "input.details[" << index << "].element does not exist or is not an Element";
            }
        }

        emit detailsChanged();
    }

    emit cooked(OperationAttached::Finished);
}
