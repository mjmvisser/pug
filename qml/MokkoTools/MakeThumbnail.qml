import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property var input
    property bool filmstrip: false 

    count: input && input.details ? input.details.length : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    function __buildOutputPath() {
        var directory = detail(index, "element", "directory");
        var baseName = detail(index, "element", "baseName");
        var ext = self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg";
        if (typeof directory !== "undefined" && typeof baseName !== "undefined")
            return directory + baseName + ext;
        else
            return "";
    }

    argv: [ 
        Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", ""),
        "--inputPath", detail(index, "element", "pattern"),
        "--outputPath", __buildOutputPath(),
        "--firstFrame", detail(index, "element", "firstFrame"),
        "--lastFrame", detail(index, "element", "lastFrame")
    ]

    onCookedAtIndex: {
        debug("MakeThumbnail.onCooked");
        var newElement = Util.newElement();
        newElement.pattern = __outputPath;
        details[index].element = newElement;
        details[index].fields = input.details[index].fields;
        detailsChanged();
        
        debug("new element " + newElement.pattern);
    }
}
