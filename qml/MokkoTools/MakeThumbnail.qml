import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property File input
    property bool filmstrip: false 

    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    property string __outputPath: {
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
        var inputElementsView = Util.elementsView(input);
        return [relativePath("scripts/makeThumbnail.py"),
                "--inputPath", inputElementsView.elements[index].pattern,
                "--outputPath", __outputPath,
                "--firstFrame", inputElementsView.elements[index].firstFrame(),
                "--lastFrame", inputElementsView.elements[index].lastFrame()
        ]
    }
    
    onCookedAtIndex: {
        info("Generating thumbnail " + argv[4] + " from " + argv[2]);
        var inputElementsView = Util.elementsView(input);
        inputElementsView.elements[index].pattern = __outputPath 
        details[index].context = input.details[index].context
        detailsChanged();
    }
    
}
