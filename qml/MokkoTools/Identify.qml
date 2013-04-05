import QtQuick 2.0
import Pug 1.0

Node {
    id: self

    property Node input
    property var metadata: {}

    inputs: Input { name: "input" }
    params: Param { name: "metadata" }

    details: processor.details
    count: processor.count

    Process {
        id: identify
        name: "identify"
        
        count: input ? input.count : 0

        function __buildFormatString(metadata) {
            var result = [];
            for (var key in metadata) {
                result.push(key + "=" + metadata[key]);
            }
            
            return result.join("\n");
        }
    
        updatable: true
        
        property var inputElementsView: Util.elementsView(input)
        
        argv: {
            //var inputElementsView = Util.elementsView(input);
            var path;
            if (inputElementsView.elements[index].isSequence()) {
                path = inputElementsView.elements[index].frames[0].path();
            } else {
                path = inputElementsView.elements[index].path();
            }
            
            var format = __buildFormatString(metadata);
            
            return ["identify",
                    "-format", format,
                    path];
        }
    }
    
    Node {
        id: processor
        name: "processor"
        output: true

        count: input ? input.count : 0

        property Node input: identify

        inputs: Input { name: "input" }
        
        signal update(var context);
        signal updateFinished(int status);

        onUpdate: {
            for (index = 0; index < count; index++) {
                var lines = input.details[index].process.stdout.replace(/\n$/, "").split("\n");
                var data = {};
                for (var i = 0; i < lines.length; i++) {
                    var tokens = lines[i].split("=");
                    data[tokens[0]] = tokens[1];
                }
    
                details[index].identify = data;
                details[index].context = input.details[index].context;
                for (var k in context) {
                    if (context.hasOwnProperty(k) && !(k in details[index].context)) {
                        details[index].context[k] = context[k];
                    }
                }
            }
            
            updateFinished(Operation.Finished);
        }
    }
}        
