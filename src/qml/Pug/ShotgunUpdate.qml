import Pug 1.0

Node {
    id: self
    property var entityType
    property int entityId
    property var dataValues
    
    signal __updateFinished(var result)
    signal __updateError()
    
    property var __context;
    
    ReleaseOperation.onCook: {
        addTrace("ReleaseOperation.onCook(" + JSON.stringify(context) + ")");
        index = 0;
        __context = context;
        var reply = Shotgun.update(entityType, entityId, dataValues);
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
        
        ReleaseOperation.cookFinished(Operation.Finished);
    }
    
    on__UpdateError: {
        addError(sender().errorString());
        ReleaseOperation.cookFinished(Operation.Error);
    }
}
