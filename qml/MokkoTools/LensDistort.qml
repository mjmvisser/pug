import QtQuick 2.0
import Pug 1.0

Process {
    id: self

    // inputs
    property Node input
    property Node nukeScript

    inputs: [
        Input { name: "input" },
        Input { name: "nukeScript" }
    ]
    
    // params
    property string format
    property string fileType: "jpg"
    property string mode: "undistort"

    params: [
        Param { name: "format" },
        Param { name: "fileType" },
        Param { name: "mode" }
    ]

    count: input ? input.count : 0

    property var inputElementsView: Util.elementsView(input)
    property var elementsView: Util.elementsView(self)

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + (self.name ? self.name + "/" : "") + inputElementsView.elements[0].path() + mode + "_" + format + "_" + index + ".%04d." + fileType;
    }

    cookable: true

    argv: try { 
            [Qt.resolvedUrl("scripts/lensdistort").replace("file://", ""),
             "--nukeNodePath", nukeScript.details[0].element.pattern,
             "--inputPath", input.details[index].element.pattern,
             "--outputFormat", format + (mode == "undistort" ? "_und" : ""),
             "--outputPath", __outputPath(index),
             "--frameStart", inputElementsView.elements[index].frameStart(),
             "--frameEnd", inputElementsView.elements[index].frameEnd()]
          } catch (e) {
              []
          } 
    
    function __setDetails(index) {
        elementsView.elements[index].pattern = __outputPath(index, details[index].context);
        elementsView.elements[index].setFrames(inputElementsView.elements[index]);
    }
    
    onCookFinished: {
        for (index = 0; index < count; index++) {
            __setDetails(0);
        }
    }
}
