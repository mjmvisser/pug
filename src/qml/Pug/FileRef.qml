import QtQuick 2.0
import Pug 1.0

Node {
    property string target
    property string __targetNode: findNode(target)
    property var paths: __targetNode.paths
}
