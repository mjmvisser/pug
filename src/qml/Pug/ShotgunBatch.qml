import Pug 1.0

Node {
    id: self
    property var requests
    
    signal __batchFinished(var results)
    signal __batchError()
    property var reply
    
    ReleaseOperation.onCook: {
        addTrace("ReleaseOperation.onCook(" + JSON.stringify(context) + ")");
        __reply = Shotgun.batch(requests);
        if (__reply) {
            __reply.finished.connect(__batchFinished);
            __reply.error.connect(__batchError);
        } else {
            addError("Shotgun.batch failed");
            ReleaseOperation.cookFinished();
        }
    }
    
    on__BatchFinished: {
        addTrace("onBatchFinished(" + JSON.stringify(results) + ")");

        count = results.length;
        for (index = 0; index < results.length; index++) {
            details[index].context = context;
            if (requests[index].request_type == "delete") {
                details[index].result = results[index];   
            } else {
                details[index].entity = results[index];
            }
        }
        
        detailsChanged();
        
        ReleaseOperation.cookFinished();
    }
    
    on__BatchError: {
        addError(__reply.errorString);
        ReleaseOperation.cookFinished();
    }
}
