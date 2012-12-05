import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property bool filmstrip: false 
    count: input !== null ? input.details.length : 0

    inputs: Input { name: "input" }
    params: [
        Param { name: "filmstrip" }
    ]

    component: Component {
        Script {
            script: Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", "")

            property int index

            property string inputPath: input.details[index].element.pattern
            property string outputPath: input.details[index].element.directory + input.details[index].element.baseName + (self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg")
            property real firstFrame: input.details[index].element.firstFrame
            property real lastFrame: input.details[index].element.lastFrame
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
                var newElement = Util.createElement(self, {pattern: outputPath});
                self.details.push({"element": newElement, "env": input.details[index].env});
                
                debug("new element " + newElement.pattern);
            }
            
            onCook: {
                debug("onCook");
            }
        }
    }
}        
