import Pug 1.0
import QtQuick 2.0

Process {
    id: self
    property Node input

    inputs: Input { name: "input" }

    count: input.count

    argv: [
        "mayapy",
        Qt.resolvedUrl("scripts/splitMayaModel.py").replace("file://", ""),
        input.detail(index, "element", "path"),
        tempFile(input.details[index].element.baseName + "_{lod}" + "." + input.details[index].element.extension)   
    ]
}        
