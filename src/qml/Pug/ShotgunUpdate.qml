import Pug 1.0

Node {
    id: self
    property var entityType
    property int entityId
    property var data
    
    signal __updateFinished(var result)
    signal __updateError()
    property var _reply
    
    ReleaseOperation.onCook: {
        addTrace("ReleaseOperation.onCook(" + JSON.stringify(context) + ")");
        index = 0;
        __reply = Shotgun.update(entityType, entityId, data);
        if (__reply) {
            __reply.finished.connect(__updateFinished);
            __reply.error.connect(__updateError);
        } else {
            addError("Shotgun.update failed");
            ReleaseOperation.cookFinished();
        }
    }
    
    on__UpdateFinished: {
        addTrace("onUpdateFinished(" + JSON.stringify(result) + ")");

        if (result) {
            count = 1;
            details[0].context = context; 
            details[0].entity = result;
        } else {
            count = 0;
        }
        
        detailsChanged();
        
        ReleaseOperation.cookFinished();
    }
    
    on__UpdateError: {
        addError(__reply.errorString);
        ReleaseOperation.cookFinished();
    }
}
