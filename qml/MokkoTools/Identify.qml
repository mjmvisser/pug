import QtQuick 2.0
import Pug 1.0

Node {
    id: self
    count: input ? input.count : 0

    property Node input
    property var metadata: {}

    inputs: Input { name: "input" }
    params: Param { name: "metadata" }

    details: processor.details

    Process {
        id: identify
        name: "identify"

        count: input ? input.count : 0

        property Node input: self.input

        inputs: Input { name: "input" }
        
        function __buildFormatString(metadata) {
            var result = [];
            for (var key in metadata) {
                if (metadata.hasOwnProperty(key)) {
                    result.push(key + "=" + metadata[key]);
                }
            }
            
            return result.join("\n");
        }
    
        argv: {
            info("generating argv, updating is " + updating);
            if (updating) {
                var inputElementsView = Util.elementsView(input);
                info("created inputElementsView: " + inputElementsView);
                info("index? " + index);
                info("element? " + inputElementsView.elements[index]);
                info("sequence? " + inputElementsView.elements[index].isSequence());
                var path;
                if (inputElementsView.elements[index].isSequence()) {
                    info("is sequence");
                    path = inputElementsView.elements[index].frames[0].path();
                } else {
                    info("is not sequence");
                    path = inputElementsView.elements[index].path();
                }
                
                var format = __buildFormatString(metadata);
                
                info("path is " + path);
                info("format is " + format);
                
                return ["identify",
                        "-format", format,
                        path];
            } else {
                return [];
            }
        }
    }
    
    Node {
        id: processor
        name: "processor"
        active: true

        count: input ? input.count : 0

        property Node input: identify

        inputs: Input { name: "input" }
        
        signal updateAtIndex(int index, var context);
        signal updateAtIndexFinished(int index, int status);

        onUpdateAtIndex: {
            info("onUpdateAtIndex(" + index + ", " + JSON.stringify(context) + ")");
            info("input: " + input);
            info("input details: " + JSON.stringify(input.details));
            var lines = input.details[index].process.stdout.replace(/\n$/, "").split("\n");
            var data = {};
            debug("lines is " + JSON.stringify(lines));
            for (var i = 0; i < lines.length; i++) {
                var tokens = lines[i].split("=");
                debug("tokens is " + JSON.stringify(tokens));
                data[tokens[0]] = tokens[1];
            }

            details[index].identify = data;
            details[index].context = input.details[index].context;
            for (var k in context) {
                if (context.hasOwnProperty(k) && !(k in details[index].context)) {
                    details[index].context[k] = context[k];
                }
            }
            
            updateAtIndexFinished(index, Operation.Finished);
        }
    }
}        
