import QtQuick 2.0
import Pug 1.0

Process {
    id: self

    // inputs
    property File nukeNodeFile
    property File plateFile

    inputs: [
        Input { name: "nukeNodeFile" },
        Input { name: "plateFile" }
    ]
    
    // params
    property string format

    params: [
        Param { name: "format" }
    ]

    argv: [
        Qt.resolvedUrl("scripts/genundist2").replace("file://", ""),
        "--nukeNodePath", nukeNodeFile.details[index].pattern,
        "--platePath", plateFile.details[index].pattern,
        "--outputFormat", format,
        "--outputPath", temporaryFile(self.name + ".%04d.jpg")
    ]

    // additional outputs
    property MakeThumbnail thumbnail: makeThumbnail
    property MakeThumbnail filmstrip: makeFilmstrip

    MakeThumbnail {
        id: thumbnail
        input: self
        filmstrip: false
    }

    MakeThumbnail {
        id: filmstrip
        input: self
        filmstrip: true
    }
}
