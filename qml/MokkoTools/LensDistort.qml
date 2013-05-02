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
    property string inputResolution
    property string outputResolution
    property string fileType: "jpg"
    property string mode: "undistort"

    params: [
        Param { name: "inputResolution" },
        Param { name: "outputResolution" },
        Param { name: "fileType" },
        Param { name: "mode" }
    ]

    count: input ? input.count : 0

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + (self.name ? self.name + "/" : "") + input.File.elements[0].path() + mode + "_" + outputResolution + "_" + index + ".%04d." + fileType;
    }

    cookable: true
    updatable: true

    argv: { 
        if (cooking) {
            [Qt.resolvedUrl("scripts/lensdistort").replace("file://", ""),
             "--nukeNodePath", nukeScript.details[0].element.pattern,
             "--inputPath", input.details[index].element.pattern,
             "--inputResolution", inputResolution,
             "--outputResolution", outputResolution,
             "--outputPath", __outputPath(index),
             "--frameStart", inputElementsView.elements[index].frameStart(),
             "--frameEnd", inputElementsView.elements[index].frameEnd()];
        } else {
            ["true"];
        }
    } 
    
    function __setDetails(index) {
        // merge the input context
        details[index].context = Util.mergeContexts(input.details[index].context, details[index].context);
        details[index].context["RESOLUTION"] = outputResolution;

        // set the pattern and scan for file info
        self.File.elements[index].pattern = __outputPath(index, details[index].context);
        self.File.elements[index].scan();
    }
    
    UpdateOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            __setDetails(0);
        }
    }
    
    CookOperation.onCookFinished: {
        for (index = 0; index < count; index++) {
            __setDetails(0);
        }
    }
}