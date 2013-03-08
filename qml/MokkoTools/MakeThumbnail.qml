import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property var input
    property bool filmstrip: false 

    count: input && input.details ? input.details.length : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    property string outputPath: {
        var directory = input.detail(index, "element", "directory");
        var baseName = input.detail(index, "element", "baseName");
        var ext = self.filmstrip ? "_filmstrip.jpg" : "_thumbnail.jpg";
        if (typeof directory !== "undefined" && typeof baseName !== "undefined")
            return directory + baseName + ext;
        else
            return "";
    }

    argv: [ 
        Qt.resolvedUrl("scripts/makeThumbnail.py").replace("file://", ""),
        "--inputPath", input.detail(index, "element", "pattern"),
        "--outputPath", outputPath,
        "--firstFrame", input.detail(index, "element", "firstFrame"),
        "--lastFrame", input.detail(index, "element", "lastFrame")
    ]

    onCookedAtIndex: {
        info("Generating thumbnail " + argv[4] + " from " + argv[2]);
        var newElement = Util.element();
        newElement.pattern = outputPath;
        details[index].element = newElement;
        details[index].context = input.details[index].context
        detailsChanged();
        
        debug("new element " + newElement.pattern);
    }
}
