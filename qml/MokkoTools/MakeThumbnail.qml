import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property File input
    property bool filmstrip: false 

    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    function __outputPath(index) {
        var inputElementsView = Util.elementsView(input);
        var directory = inputElementsView.elements[index].directory();
        var baseName = inputElementsView.elements[index].baseName();
        var ext = self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg";
        if (typeof directory !== "undefined" && typeof baseName !== "undefined")
            return directory + baseName + ext;
        else
            return "";
    }

    argv: {
        if (cooking) {
            var inputElementsView = Util.elementsView(input);
            return [Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", ""),
                    "--inputPath", inputElementsView.elements[index].pattern,
                    "--outputPath", __outputPath(index),
                    "--firstFrame", inputElementsView.elements[index].firstFrame(),
                    "--lastFrame", inputElementsView.elements[index].lastFrame()]
        } else {
            return [];
        }
    }
    
    onCookedAtIndex: {
        info("Generating thumbnail " + argv[4] + " from " + argv[2]);
        var elementsView = Util.elementsView(self);
        elementsView.elements[index].pattern = __outputPath(index) 
    }
    
}
