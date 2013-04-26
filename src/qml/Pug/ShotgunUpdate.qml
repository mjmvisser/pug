import Pug 1.0

Node {
    id: self
    property var entityType
    property int entityId
    property var data
    
    signal release(var context)
    signal releaseFinished(int status)
    
    signal __updateFinished(var result)
    signal __updateError()
    
    property var __context;
    
    onRelease: {
        addTrace("onRelease(" + JSON.stringify(context) + ")");
        __context = context;
        var reply = Shotgun.update(entityType, entityId, data);
        reply.finished.connect(__updateFinished);
        reply.error.connect(__updateError);
    }
    
    on__UpdateFinished: {
        addTrace("onUpdateFinished(" + JSON.stringify(result) + ")");

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
    
    on__UpdateError: {
        addError(sender().errorString());
        releaseFinished(Operation.Error);
    }
}
