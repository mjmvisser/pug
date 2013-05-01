import Pug 1.0

Node {
    id: self
    property File input
    
    inputs: [
        Input { name: "input"; lockInput: true }
    ]

    property var context
    
    params: [
        Param { name: "context" }
    ]
    
    signal update(var context)
    signal updateFinished(int status)
    
    function __containsContext(needle, haystack) {
        if (typeof needle !== "undefined" && typeof haystack !== "undefined") {
            for (var k in needle) {
                if (needle[k] !== haystack[k])
                    return false;
            }
            return true;
        } else {
            return false;
        }
    }
    
    onUpdate: {
        trace("onUpdate(" + JSON.stringify(context) + ")");
        details = new Array();
        
        // if context matches, copy the input        
        for (var i = 0; i < input.count; i++) {
            if (__containsContext(self.context, input.details[i].context)) {
                details.push(input.details[i]);
            }
        }
        
        self.count = details.length;
        
        if (self.count == 0) {
            error("Nothing in input " + input.name + " matched " + JSON.stringify(context));
            updateFinished();
        } else {
            updateFinished();
        }
    }
}
