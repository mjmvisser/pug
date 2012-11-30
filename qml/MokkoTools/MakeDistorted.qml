import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property var elements: []
    property var nukeDistortFile
    property string outputFormat
    count: input !== null ? input.elements.length : 0

    Property {
        input: true
        name: "input"
    }

    Property {
        input: true
        name: "nukeDistortFile"
    }
    
    Property {
        name: "outputFormat"
    }

    component: Component {
        Script {
            property int index
            script: Qt.resolvedUrl("scripts/makeDistorted.sh").replace("file://", "")
            property var inputsequence: input.elements[index].pattern
            property string outsequence: generatePath(self.name, index, input.elements[index].frameSpec, input.elements[index].extension)
            property real start: input.elements[index].firstFrame
            property real end: input.elements[index].lastFrame
            property string distortnukefile: nukeDistortFile.elements[0].path
            property string outformat: outputFormat
            property string __render: "True"
            property string __debug: "True"

            Property {
                name: "inputsequence"
                property bool noFlag: true
            }

            Property {
                name: "outsequence"
            }

            Property {
                name: "start"
            }

            Property {
                name: "end"
            }
            
            Property {
                name: "distortnukefile"
            }

            Property {
                name: "outformat"
            }

            Property {
                name: "__render"
            }

            Property {
                name: "__debug"
            }

            onCooked: {
                debug("onCooked");
                var newElement = Util.createElement(self, {pattern: inputsequence, data: input.elements[index].data});
                elements.push(newElement);
                
                debug("new element " + newElement.pattern);
            }
            
            onCook: {
                debug("onCook");
            }
        }
    }
}        
