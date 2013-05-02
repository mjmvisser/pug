import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node nukeTemplate
    property Node input
    
    inputs: [
        Input { name: "nukeTemplate" },
        Input { name: "input" }
    ]
    
    property string resolution
    property string filetype
    property double fps
    property string codec
    property string project
    property string sequence
    property string scene
    property string shot
    property int version
    property string notes
    property string artist
    
    params: [
        Param { name: "resolution" },
        Param { name: "filetype" },
        Param { name: "fps" },
        Param { name: "codec" },
        Param { name: "project" },
        Param { name: "sequence" },
        Param { name: "scene" },
        Param { name: "shot" },
        Param { name: "version" },
        Param { name: "notes" },
        Param { name: "artist" }
    ]

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + self.path() + "." + filetype;
    }

    count: input ? input.count : 0

    updatable: true
    cookable: true

    argv: try {
        if (updating) {
            ["true"]    
        } else if (cooking) {
            [Qt.resolvedUrl("scripts/clientQuicktime").replace("file://", ""),
             "--nukeTemplatePath", nukeTemplate.details[0].element.pattern,
             "--inputPath", input.File.elements[index].pattern,
             "--frameStart", input.File.elements[index].frameStart(),
             "--frameEnd", input.File.elements[index].frameEnd(),
             "--outputPath", __outputPath(index),
             "--resolution", resolution,
             "--fps", fps,
             "--codec", codec,
             "--project", project,
             "--sequence", sequence,
             "--scene", scene,
             "--shot", shot,
             "--version", version,
             "--notes", notes,
             "--artist", artist]
        } else {
            []
        }
    } catch (e) {
        //console.log("argv failed: " + e);
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
