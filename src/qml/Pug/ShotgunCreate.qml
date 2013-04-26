import Pug 1.0

Node {
    id: self
    property var entityType
    property var data
    property var return_fields: []
    
    signal release(var context)
    signal releaseFinished(int status)
    
    signal __createFinished(var result)
    signal __createError()
    
    property var __context;
    
    onRelease: {
        addTrace("onRelease(" + JSON.stringify(context) + ")");
        __context = context;
        var reply = Shotgun.create(entityType, data, return_fields);
        reply.finished.connect(__createFinished);
        reply.error.connect(__createError);
    }
    
    on__CreateFinished: {
        addTrace("onCreateFinished(" + JSON.stringify(result) + ")");

        if (result) {
            count = 1;
            details[0].context = __context; 
            details[0].entity = result;
        } else {
            count = 0;
        }
                    
        detailsChanged();
        
        releaseFinished(Operation.Finished);
    }
    
    on__CreateError: {
        addError(sender().errorString());
        releaseFinished(Operation.Error);
    }
}
