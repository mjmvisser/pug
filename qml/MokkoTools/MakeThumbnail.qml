import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property var elements: []
    property bool filmstrip: false 
    count: input !== null ? input.elements.length : 0

    inputs: Input { name: "input" }
    params: [
        Param { name: "filmstrip" }
    ]

    component: Component {
        Script {
            script: Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", "")

            property int index

            property string inputPath: input.elements[index].pattern
            property string outputPath: input.elements[index].directory + input.elements[index].baseName + (self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg")
            property real firstFrame: input.elements[index].firstFrame
            property real lastFrame: input.elements[index].lastFrame
            property bool filmstrip: self.filmstrip

            params: [
                Param { name: "inputPath" },
                Param { name: "outputPath" },
                Param { name: "firstFrame" },
                Param { name: "lastFrame" },
                Param { name: "filmstrip" }
            ]

            onCooked: {
                debug("onCooked");
                var newElement = Util.createElement(self, {pattern: outputPath, data: input.elements[index].data});
                elements.push(newElement);
                
                debug("new element " + newElement.pattern);
            }
            
            onCook: {
                debug("onCook");
            }
        }
    }
}        
