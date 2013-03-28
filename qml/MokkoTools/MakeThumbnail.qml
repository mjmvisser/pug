import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node input
    property bool filmstrip: false 
    
    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    function __outputPath(index, context) {
        var inputElementsView = Util.elementsView(input);
        var directory = context["PUGWORK"] + (self.name ? self.name + "/" : "");
        var baseName = inputElementsView.elements[index].baseName();
        var ext = self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg";
        return directory + baseName + ext;
    }

    argv: {
        if (cooking) {
            var inputElementsView = Util.elementsView(input);
            return [Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", ""),
                    "--inputPath", inputElementsView.elements[index].pattern,
                    "--outputPath", __outputPath(index, CookOperation.context),
                    "--firstFrame", inputElementsView.elements[index].firstFrame(),
                    "--lastFrame", inputElementsView.elements[index].lastFrame(),
                    "--filmstrip", filmstrip]
        } else {
            return [];
        }
    }

    onUpdateAtIndex: {
        var elementsView = Util.elementsView(self);
        elementsView.elements[index].pattern = __outputPath(index, context);
        updateAtIndexFinished(index, Operation.Finished);
    }
    
    onCookAtIndex: {
        var elementsView = Util.elementsView(self);
        elementsView.elements[index].pattern = __outputPath(index, context); 
        cookAtIndexFinished(index, Operation.Finished);
    }
    
}
