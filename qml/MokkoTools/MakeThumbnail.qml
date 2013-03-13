import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property File input
    property bool filmstrip: false 

    // TODO: should this be input ? input.count : 0
    count: input && input.details ? input.details.length : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    argv: [ 
        relativePath("scripts/makeThumbnail.py"),
        "--inputPath", input.details[index].element.pattern,
        "--outputPath", temporaryFile(name + (self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg")),
        "--firstFrame", input.detail(index, "element", "firstFrame"),
        "--lastFrame", input.detail(index, "element", "lastFrame")
    ]
}
