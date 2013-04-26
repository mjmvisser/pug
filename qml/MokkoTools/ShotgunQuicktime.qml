import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node input
    property string format: "mp4" // mp4 or webm
    property int fps: 24
    
    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: [
        Param { name: "format" },
        Param { name: "fps" }
    ]

    property var inputElementsView: Util.elementsView(input)
    property var elementsView: Util.elementsView(self)

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + self.path() + "." + format;
    }

    updatable: true
    cookable: true

    argv: try {
        if (updating) {
            ["true"]    
        } else if (cooking) {
            [Qt.resolvedUrl("scripts/shotgunQuicktime.py").replace("file://", ""),
             "--inputPath", inputElementsView.elements[index].pattern,
             "--outputPath", __outputPath(index),
             "--firstFrame", inputElementsView.elements[index].frameStart(),
             "--format", format,
             "--fps", fps]
        } else {
            []
        }
    } catch (e) {
        []
    }
    
    UpdateOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            elementsView.elements[index].pattern = __outputPath(index);
        } 
    }

    CookOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            elementsView.elements[index].pattern = __outputPath(index);
        } 
    }
}
