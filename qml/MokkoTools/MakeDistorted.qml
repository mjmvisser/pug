import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property var elements: []
    property string distortNukeFile
    property string outputFormat

    component: Component {
        Script {
            property int index
            script: Qt.resolvedUrl("$NUKE_EXEC -t $PIPELINE_QTENGINE2/qtdistort.py").replace("file://", "")
            property var inputsequence: input.elements[index].pattern
            property string outsequence: generatePath(self.name, index, input.elements[index].frameSpec, input.elements[index].extension)
            property real start: input.elements[index].firstFrame
            property real end: input.elements[index].lastFrame
            property string distortnukefile: distortNukeFile
            property string out_format: outputFormat
            property string render: "True"
            property string debug: "True"

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
                name: "out_format"
            }

            Property {
                name: "render"
            }

            Property {
                name: "debug"
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
