import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node nukeTemplate
    property Node sequence
    
    inputs: [
        Input { name: "nukeTemplate" },
        Input { name: "sequence" }
    ]
    
    property string format
    property string filetype
    property string project
    property string shot
    property int version
    property string notes
    property string artist
    
    params: [
        Param { name: "format" },
        Param { name: "filetype" },
        Param { name: "project" },
        Param { name: "shot" },
        Param { name: "version" },
        Param { name: "notes" },
        Param { name: "artist" }
    ]

    function __outputPath(index) {
        return sequence.details[index].context["PUGWORK"] + self.path() + "." + filetype;
    }

    count: sequence ? sequence.count : 0

    updatable: true
    cookable: true

    argv: try {
              if (updating) {
                  ["true"]    
              } else if (cooking) {
                  [Qt.resolvedUrl("scripts/nukeSlate").replace("file://", ""),
                   "--nukeTemplatePath", nukeTemplate.details[0].element.pattern,
                   "--vignettePath", sequence.File.elements[index].frames[0].path(),
                   "--outputPath", __outputPath(index),
                   "--outputFormat", format,
                   "--project", project,
                   "--shot", shot,
                   "--version", version,
                   "--length", sequence.File.elements[index].frames.length,
                   "--notes", notes,
                   "--artist", artist]
              } else {
                  []
              }
          } catch (e) {
              []
          }
    
    onUpdateFinished: {
        for (index = 0; index < count; index++) {
            self.File.elements[index].pattern = __outputPath(index);
        } 
    }

    onCookFinished: {
        for (index = 0; index < count; index++) {
            self.File.elements[index].pattern = __outputPath(index);
        } 
    }
}
