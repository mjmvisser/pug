import QtQuick 2.0
import Pug 1.0

Process {
    id: self
    property Node input
    property bool filmstrip: false 
    
    count: input ? input.count : 0

    inputs: Input { name: "input" }
    params: Param { name: "filmstrip" }

    property var inputElementsView: Util.elementsView(input)
    property var elementsView: Util.elementsView(self)

    function __outputPath(index) {
        return input.details[index].context["PUGWORK"] + self.path() + ".jpg";
    }

    updatable: true
    cookable: true

    argv: try {
              if (updating) {
                  ["true"]    
              } else if (cooking) {
                  [Qt.resolvedUrl("scripts/shotgunThumbnail.py").replace("file://", ""),
                   "--inputPath", inputElementsView.elements[index].pattern,
                   "--outputPath", __outputPath(index),
                   "--firstFrame", inputElementsView.elements[index].frameStart(),
                   "--lastFrame",  inputElementsView.elements[index].frameEnd(),
                   "--" + (filmstrip ? "filmstrip" : "thumbnail")]
              } else {
                  []
              }
          } catch (e) {
              []
          }
    
    onUpdateFinished: {
        for (index = 0; index < count; index++) {
            elementsView.elements[index].pattern = __outputPath(index);
        } 
    }

    onCookFinished: {
        for (index = 0; index < count; index++) {
            elementsView.elements[index].pattern = __outputPath(index);
        } 
    }
}
