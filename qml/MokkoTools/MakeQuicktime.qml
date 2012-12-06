import QtQuick 2.0
import Pug 1.0

CookQueue {
    id: self
    property var input
    property string outputFormat
    count: input !== null ? input.details.length : 0

    inputs: [
        Input { name: "input" },
    ]
    
    params: Param { name: "outputFormat" }

    component: Component {
        Script {
            property int index
            script: Qt.resolvedUrl("scripts/makeQuicktime.sh").replace("file://", "")

            property var inputsequence: input.details[index].element.pattern
            property string outsequence: generatePath(self.name, index, input.details[index].element.frameSpec, input.details[index].element.extension)
            property real start: input.details[index].element.firstFrame
            property real end: input.details[index].element.lastFrame
            property string distortnukefile: nukeDistortFile.details[0].element.path
            property string outformat: outputFormat
            property string __render: "True"
            property string __debug: "True"

            params: [
                Param { name: "inputsequence";
                        property bool noFlag: true },
                Param { name: "outsequence" },
                Param { name: "start" },
                Param { name: "end" },
                Param { name: "distortnukefile" },
                Param { name: "outformat" },
                Param { name: "__render" },
                Param { name: "__debug" }
            ]

            onCooked: {
                debug("onCooked");
                var newElement = Util.createElement(self, {pattern: inputsequence});
                self.details.push({"element": newElement, "env": input.details[index].env});
                
                debug("new element " + newElement.pattern);
            }
            
            onCook: {
                debug("onCook");
            }
        }
    }
}        
