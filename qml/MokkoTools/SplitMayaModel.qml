import Pug 1.0
import QtQuick 2.0

Process {
    id: self
    property Node input

    inputs: Input { name: "input" }

    count: input ? input.count : 0

    function __outputPath(index, context) {
        return context["PUGWORK"] + (self.name ? self.name + "/" : "") + "_{lod}_" + index + "." + input.File.elements[index].extension();
    }

    argv: {
        if (cooking) {
            return ["mayapy",
                    Qt.resolvedUrl("scripts/splitMayaModel.py").replace("file://", ""),
                    input.detail(index, "element", "path"),
                    __outputPath(index, CookOperation.context)]
        } else {
            return [];
    }
}        
