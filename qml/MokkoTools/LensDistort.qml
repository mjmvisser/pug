import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    TractorOperation.tags: "nuke"

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
    updatable: true

    argv: try { 
            if (cooking) {
                [Qt.resolvedUrl("scripts/lensdistort").replace("file://", ""),
                 "--nukeNodePath", nukeScript.details[0].element.pattern,
                 "--inputPath", input.details[index].element.pattern,
                 "--outputFormat", format + (mode == "undistort" ? "_und" : ""),
                 "--outputPath", __outputPath(index),
                 "--frameStart", inputElementsView.elements[index].frameStart(),
                 "--frameEnd", inputElementsView.elements[index].frameEnd()]
             } else if (updating) {
                 ["true"]
             } else {
                 []
             }
          } catch (e) {
              []
          } 
    
    function __setDetails(index) {
        // merge the input context
        details[index].context = Util.mergeContexts(input.details[index].context, details[index].context);

        // set the pattern and scan for file info
        elementsView.elements[index].pattern = __outputPath(index, details[index].context);
        elementsView.elements[index].scan();
    }
    
    onCookFinished: {
        for (index = 0; index < count; index++) {
            __setDetails(0);
        }
    }
    
    onUpdateFinished: {
        for (index = 0; index < count; index++) {
            __setDetails(0);
        }
    }
}
