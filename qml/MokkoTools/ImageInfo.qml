import QtQuick 2.0
import Pug 1.0

Process {
    id: self

    property Node input
    inputs: Input { name: "input" }

    count: input ? input.count : 0

    updatable: true
    
    function __inputPath(index) {
        if (input.File.elements[index].isSequence()) {
            return input.File.elements[index].frames[0].path();
        } else {
            return input.File.elements[index].path();
        }
    }
    
    argv: try {
        ["identify",
         "-format", "RESOLUTION=%wx%h\nCODEC=%m\nCOLORSPACE=%[colorspace]\nDEPTH=%[depth]",
        __inputPath(index)]
    } catch (e) {
        []
    } 
    
    function __parseInfo() {
        for (index = 0; index < count; index++) {
            details[index].element = input.details[index].element;
            var lines = details[index].process.stdout.replace(/\n$/, "").split("\n");
            for (var i = 0; i < lines.length; i++) {
                var tokens = lines[i].split("=");
                details[index].context[tokens[0]] = tokens[1];
            }
        }
    }
    
    UpdateOperation.onCookFinished: __parseInfo()
}        
