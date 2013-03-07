import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    count: scene !== null ? scene.details.length : 0
    property var scene

    inputs: [
        Input { name: "scene" }
    ]
    
    property string format: "1920x1080"
    
    params: Param { name: "format" }

    property string __outputPath: "/usr/tmp/foo.#.exr"

    argv: [
        Qt.resolvedUrl("scripts/makeTurntable.py").replace("file://", ""),
        "--format", format,
        "--scene", scene.detail(index, "element", "pattern"),
        "--output", __outputPath
    ]
    
    onCookedAtIndex: {
        debug("onCooked");
        var newElement = Util.element();
        newElement.pattern = __outputPath;
        details[index].element = newElement;
        details[index].fields = input.details[index].fields;
        detailsChanged();
        debug("new element " + newElement.pattern);
    }
}        
