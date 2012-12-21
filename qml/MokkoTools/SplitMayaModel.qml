import Pug 1.0

Process {
    id: self
    count: input.details.length
    property var input

    onDetailsChanged: {
        console.log(self + " details changed");
    }

    inputs: Input { name: "input" }

    argv: [
        "mayapy",
        Qt.resolvedUrl("scripts/splitMayaModel.py").replace("file://", ""),
        input.detail(index, "element", "path"),
        tempFile(input.detail(index, "element", "baseName") + "_{lod}", 
                 "." + input.detail(index, "element", "extension"))   
    ]

    onCookedAtIndex: {
        if (status === Operation.Finished) {
            debug("onCooked");
            var result = JSON.parse(stdout(index));
            var inputContext = input.details[index].context;
            for (var i = 0; i < result.length; i++) {
                var element = Util.newElement(result[i].element);
                var context = {};
                // copy inputContext
                for (var k in inputContext) {
                    if (inputContext.hasOwnKey(k)) {
                        context[k] = inputContext[k];
                    }
                }
                // copy result context
                for (var k in result[i].context) {
                    if (result[i].context.hasOwnKey(k)) {
                        context[k] = result[i].context[k];
                    }                    
                }
                
                var detail = {"element": element,
                              "context": context};
                              
                details.push(detail);                              
            }
        }
    }
}        
