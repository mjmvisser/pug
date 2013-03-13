import Pug 1.0

Process {
    id: self
    count: scene !== null ? scene.details.length : 0
    property var scene
    property var refs

    inputs: [
        Input { name: "scene" },
        Input { name: "refs" }
    ]

    property string outputPath: temporaryFile(name + "." + scene.details[index].element.extension) 

    function refPaths() {
        var result = [];
        for (var i = 0; i < refs.count; i++) {
            result.push(refs.details[i].element.path);
        }
        return result;
    }

    argv: [
        Qt.resolvedUrl("scripts/conformMaya.py").replace("file://", ""),
        detail(index, "element", "path")
    ].concat(refPaths(), [outputPath]);
}        
