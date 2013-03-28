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

    function __outputPath(index, context) {
        var inputElementsView = Util.elementsView(input);
        return context["PUGWORK"] + (self.name ? self.name + "/" : "") + inputElementsView.elements[0].path() + mode + "_" + format + "_" + index + ".%04d." + fileType;
    }

    argv: { 
        if (cooking) {
            return [Qt.resolvedUrl("scripts/lensdistort").replace("file://", ""),
                    "--nukeNodePath", nukeScript.details[index].element.pattern,
                    "--inputPath", input.details[index].element.pattern,
                    "--outputFormat", format + (mode == "undistort" ? "_und" : ""),
                    "--outputPath", __outputPath(index, details[index].context)]
        } else {
            return [];
        }
    }

    function __setDetails(index, context) {
        var elementsView = Util.elementsView(self);
        var inputElementsView = Util.elementsView(input);
        elementsView.elements[index].pattern = __outputPath(index, context)
        elementsView.elements[index].setFrames(inputElementsView.elements[index].frames);
        details[index].context = input.details[index].context;
        for (var k in context) {
            if (context.hasOwnProperty(k) && !(k in details[index].context)) {
                details[index].context[k] = context[k];
            }
        }
    }
    
    onUpdateAtIndex: {
        __setDetails(index, context);
    }
    
    onCookAtIndex: {
        __setDetails(index, context);
    }
}
