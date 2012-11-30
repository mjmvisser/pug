import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property var elements: []
    property bool filmstrip: false 
    count: input !== null ? input.elements.length : 0

    component: Component {
        Script {
            property int index
            script: Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", "")
            property string inputPath: input.elements[index].pattern
            property string outputPath: input.elements[index].directory + input.elements[index].baseName + (self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg")
            property real firstFrame: input.elements[index].firstFrame
            property real lastFrame: input.elements[index].lastFrame
            property bool filmstrip: self.filmstrip

            Property {
                name: "inputPath"
            }

            Property {
                name: "outputPath"
            }

            Property {
                name: "firstFrame"
            }

            Property {
                name: "lastFrame"
            }
            
            Property {
                name: "filmstrip"
            }

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

    Property {
        input: true
        name: "input"
    }
}        
