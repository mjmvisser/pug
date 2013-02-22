import QtQuick 2.0
import Pug 1.0

Rectangle {
    id: main
    width: 1550
    height: 800
    
    NodeModel {
        id: nodeModel 
        root: PUG.root
    }
    
    Column {
        ListView {
            anchors.fill: parent
            
            model: nodeModel
            
            delegate: Text {
                text: name
            }
        }
        
        NodeGraphView {
            anchors.fill: parent
            
            model: nodeModel
        }
    }
}
