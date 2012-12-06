import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    count: input !== null ? input.details.length : 0
    property var input
    property var nukeDistortFile
    property string outputFormat

    inputs: [
        Input { name: "input" },
        Input { name: "nukeDistortFile" }
    ]
    
    params: Param { name: "outputFormat" }

    argv: [
        Qt.resolvedUrl("scripts/makeDistorted.sh").replace("file://", ""),
        "--inputsequence", input.details[index].element.pattern,
        "--outsequence", generatePath(self.name, index, input.details[index].element.frameSpec, input.details[index].element.extension),
        "--start", input.details[index].element.firstFrame,
        "--end", input.details[index].element.lastFrame,
        "--distortnukefile", nukeDistortFile.details[0].element.path,
        "--outformat", outputFormat,
        "--render", "True", "--debug", "True"
    ]
    
    onCookedAtIndex: {
        debug("onCooked");
        var newElement = Util.createElement(self, {pattern: inputsequence});
        setDetail(index, "element", newElement);
        setDetail(index, "env", input.details[index].env);
        debug("new element " + newElement.pattern);
    }
}        
