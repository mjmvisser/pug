import Pug 1.0

Node {
    id: self
    property var entityType
    property var data
    property var returnFields: []
    
    signal __createFinished(var result)
    signal __createError()
    property var __reply
    
    ReleaseOperation.onCook: {
        addTrace("ReleaseOperation.onCook(" + JSON.stringify(context) + ")");
        __reply = Shotgun.create(entityType, data, returnFields);
        if (__reply) {
            __reply.finished.connect(__createFinished);
            __reply.error.connect(__createError);
        } else {
            addError("Shotgun.create failed");
            ReleaseOperation.cookFinished();
        }
    }
    
    on__CreateFinished: {
        addTrace("onCreateFinished(" + JSON.stringify(result) + ")");

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
    
    on__CreateError: {
        addError(__reply.errorString);
        ReleaseOperation.cookFinished();
    }
}
