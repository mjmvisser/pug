import Pug 1.0

Node {
    id: self
    property var input
    count: input ? input.count : 0
    
    inputs: [
        Input { name: "input" }
    ]
    
    signal update(var context)
    signal updateAtIndex(int index, var context)
    signal updateAtIndexFinished(int index, int status)
    signal cook(var context)
    signal cookAtIndex(int index, var context)
    signal cookAtIndexFinished(int index, int status)
    
    onUpdate: {
        trace("onUpdate(" + JSON.stringify(context) + ")");
        details = new Array();
    }
    
    onUpdateAtIndex: {
        trace("onUpdateAtIndex(" + index + ", " + JSON.stringify(context) + ")");
        var status = __parseStdout(index, context);
        info("done, sending updateAtIndexFinished");
        updateAtIndexFinished(index, status);
    }
    
    onCook: {
        trace("onCook(" + JSON.stringify(context) + ")");
        details = new Array();
    }
    
    onCookAtIndex: {
        trace("onCookAtIndex(" + index + ", " + JSON.stringify(context) + ")");
        var status = __parseStdout(index, context);
        info("done, sending cookAtIndexFinished");
        cookAtIndexFinished(index, status);
    }
    
    function __parseStdout(index, context)
    {
        trace("__parseStdout(" + index + ")");
        // regular expression that extracts the details block from the process's stdout
        var regex = /^begin-json details$([^]*)^====$/gm;
        var groups = regex.exec(input.details[index].process.stdout);
        if (groups === null) {
            error("Can't find details in output");
            error("stdout is " + input.details[index].process.stdout);
            return Operation.Error;
        }
        
        try {
            var result = JSON.parse(groups[1]);

            debug("parsed " + JSON.stringify(result));

            var inputContext = input.details[index].context;
            
            var detailIndex = details.length;
            
            for (var i = 0; i < result.length; i++) {
                details[detailIndex] = result[i];
                if (!("context" in details[detailIndex]))
                    details[detailIndex].context = {};
                // update with inputContext
                for (var k in inputContext) {
                    if (inputContext.hasOwnProperty(k) && !(k in details[detailIndex].context)) {
                        details[detailIndex].context[k] = inputContext[k];
                    }
                }
                // update with passed context
                for (var k in context) {
                    if (context.hasOwnProperty(k) && !(k in details[detailIndex].context)) {
                        details[detailIndex].context[k] = context[k];
                    }
                }
            }
            detailsChanged();
            return Operation.Finished;                              
        } catch (e) {
            error(e);
            return Operation.Error;
        }
    }
}
