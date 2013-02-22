import QtQuick 2.0
import Pug 1.0
import PugUI 1.0

Item {
    id: self
    property NodeBase node
    property Item output: output
    
    x: node.x
    y: node.y

    Row {
        id: inputs
        anchors {
            horizontalCenter: body.horizontalCenter
            bottom: body.top
        }
        Repeater {
            model: node.inputs
            Rectangle {
                id: inputItem
                width: 30
                height: 10
                anchors { 
                    leftMargin: 10
                    rightMargin: 10 
                }
                color: "blue"
                
                GraphConnection {
                    anchors.fill: parent
                    sink: Qt.point(width/2, 0)
                    
                    signal redraw()
                    onRedraw: {
                        var inputProperty = node.inputs[index];
                        var inputNode = node[inputProperty.name];
                        if (inputNode && inputNode.childIndex() >= 0) {
                            var outputItem = self.parent.children[inputNode.childIndex()].output;
                            var mapped = mapFromItem(outputItem, outputItem.width/2, outputItem.height);
                            source = Qt.point(mapped.x, mapped.y);
                            visible = true;
                        } else {
                            source = sink;
                            visible = false;
                        }
                        update();
                    }
                    
                    Component.onCompleted: {
                        self.redrawConnections.connect(redraw);
                    }
                    
                }
            }
        }
    }
    Rectangle {
        id: body
        color: "red"
        radius: 10
        width: 100
        height: 50
        
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            drag { target: self }
            onReleased: {
                node.x = self.x
                node.y = self.y
            }
        }
    }
    Rectangle {
        id: output
        width: 30
        height: 10
        anchors { 
            leftMargin: 10
            rightMargin: 10
            horizontalCenter: body.horizontalCenter
            top: body.bottom  
        }
        color: "green"
    }
    Text {
        anchors {
            left: body.right
            verticalCenter: body.verticalCenter
        } 
        text: node.name
        verticalAlignment: Text.AlignVCenter
    }

    Component.onCompleted: {
        mouseArea.positionChanged.connect(redrawConnections);
    }

    signal redrawConnections()
    onRedrawConnections: {
        var downstreamNodes = node.downstreamNodes();
        for (var i = 0; i < downstreamNodes.length; i++) {
            var downstreamItem = parent.children[downstreamNodes[i].childIndex()];
            downstreamItem.redrawConnections();
        }         
    }    
}
