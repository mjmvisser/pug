import QtQuick 2.0
import Pug 1.0

Rectangle {
    id: self
    property NodeModel model
    
    VisualDataModel {
        id: graphView
        model: self.model
        delegate: GraphNode {
            node: self.model.data(graphView.modelIndex(index), NodeModel.NodeRole)
        }
    }
    ListView {
        id: listView
        anchors.fill: parent
        model: graphView
    }
}
