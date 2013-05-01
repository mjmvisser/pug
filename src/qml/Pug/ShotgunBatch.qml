import Pug 1.0

Node {
    id: self
    property var requests
    
    signal __batchFinished(var results)
    signal __batchError()
    
    property var __context;
    property var __reply;
    
    ReleaseOperation.onCook: {
        addTrace("ReleaseOperation.onCook(" + JSON.stringify(context) + ")");
        __context = context;
        var reply = Shotgun.batch(requests);
        __reply = reply;
        reply.finished.connect(__batchFinished);
        reply.error.connect(__batchError);
    }
    
    on__BatchFinished: {
        addTrace("onBatchFinished(" + JSON.stringify(results) + ")");

        count = results.length;
        for (index = 0; index < results.length; index++) {
            details[index].context = __context;
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
