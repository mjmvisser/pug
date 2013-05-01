import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node input
    property bool filmstrip: false 
    
    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + self.path() + ".jpg";
    }

    updatable: true
    cookable: true

    argv: try {
        if (updating) {
            ["true"]    
        } else if (cooking) {
            [Qt.resolvedUrl("scripts/shotgunThumbnail.py").replace("file://", ""),
             "--inputPath", input.File.elements[index].pattern,
             "--outputPath", __outputPath(index),
             "--firstFrame", input.File.elements[index].frameStart(),
             "--lastFrame",  input.File.elements[index].frameEnd(),
             "--" + (filmstrip ? "filmstrip" : "thumbnail")]
        } else {
            []
        }
    } catch (e) {
        []
    }
    
    UpdateOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            self.File.elements[index].pattern = __outputPath(index);
        } 
    }

    CookOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            self.File.elements[index].pattern = __outputPath(index);
        } 
    }
}
