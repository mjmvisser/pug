import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    count: scene !== null ? scene.details.length : 0
    property var scene

    inputs: [
        Input { name: "scene" },
    ]
    
    property string format: "1920x1080"
    
    params: Param { name: "format" }

    property string __outputPath: "/usr/tmp/foo.#.exr"

    argv: [
        Qt.resolvedUrl("scripts/makeTurntable.py").replace("file://", ""),
        "--format", format,
        "--scene", input.details[index].element.pattern,
        "--output", __outputPath
    ]
    
    onCookedAtIndex: {
        debug("onCooked");
        var newElement = Util.createElement(self, {pattern: __outputPath});
        setDetail(index, "element", newElement);
        setDetail(index, "env", input.details[index].env);
        debug("new element " + newElement.pattern);
    }
}        
