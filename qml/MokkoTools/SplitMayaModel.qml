import Pug 1.0
import QtQuick 2.0

Node {
    id: self
    property NodeBase input

    inputs: Input { name: "input" }

    count: input.count

    details: stdoutParser.details

    Component.onCompleted: {
        stdoutParser.detailsChanged.connect(detailsChanged);
    }

    Process {
        id: splitMayaModel
        name: "splitMayaModel"
        property NodeBase input: self.input

        inputs: Input { name: "input" }

        count: input.count

        argv: [
            "mayapy",
            Qt.resolvedUrl("scripts/splitMayaModel.py").replace("file://", ""),
            input.detail(index, "element", "path"),
            tempFile(input.detail(index, "element", "baseName") + "_{lod}", 
                     "." + input.detail(index, "element", "extension"))   
        ]
    }
    Node {
        id: stdoutParser
        name: "stdoutParser"
        property NodeBase input: splitMayaModel
        active: true
        
        inputs: Input { name: "input" }

        count: input.count
        
        signal cookAtIndex(int index, var context)
        signal cookedAtIndex(int index, int status)
                            
        onCookAtIndex: {
            trace("onCookAtIndex(" + index + ", " + JSON.stringify(context) + ")");
            // regular expression that extracts the details block from the process's stdout
            var regex = /^begin-json details$([^]*)^====$/gm;
            var groups = regex.exec(input.detail(index, "process", "stdout"));
            if (groups === null) {
                error("Can't find details in output");
                error("stdout is " + input.detail(index, "process", "stdout"));
                cookedAtIndex(index, Operation.Error);
                return;
            }
            
            try {
                var result = JSON.parse(groups[1]);
                var inputContext = input.details[index].context;
                
                debug("input detail is " + JSON.stringify(input.details[index]));
                
                for (var i = 0; i < result.length; i++) {
                    var newElement = Util.element();
                    newElement.pattern = result[i].element.path;
                    newElement.frames = result[i].element.frames;
                    var newContext = {};
                    // copy inputContext
                    for (var k in inputContext) {
                        if (inputContext.hasOwnProperty(k)) {
                            newContext[k] = inputContext[k];
                        }
                    }
                    // copy result context
                    for (var k in result[i].context) {
                        if (result[i].context.hasOwnProperty(k)) {
                            newContext[k] = result[i].context[k];
                        }
                    }
                    
                    var detail = {"element": newElement,
                                  "context": newContext};
                    
                    details[index] = detail;
                    detailsChanged();
                    cookedAtIndex(index, Operation.Finished);                              
                }
            } catch (e) {
                error(e);
                cookedAtIndex(index, Operation.Error);
                return;
            }
        }
    }
}        
