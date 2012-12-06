import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    count: input !== null ? input.details.length : 0

    property var input
    property bool filmstrip: false 

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    property string __outputPath: input.details[index].element.directory + input.details[index].element.baseName + (self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg")

    argv: [ 
        Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", ""),
        "--inputPath", input.details[index].element.pattern,
        "--outputPath", __outputPath,
        "--firstFrame", input.details[index].element.firstFrame,
        "--lastFrame", input.details[index].element.lastFrame
    ]

    onCookedAtIndex: {
        debug("MakeThumbnail.onCooked");
        var newElement = Util.createElement(self, {pattern: __outputPath});
        self.setDetail(index, "element", newElement);
        self.setDetail(index, "env", input.details[index].env);
        
        debug("new element " + newElement.pattern);
    }
}
